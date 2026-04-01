#include "src/ui/model_picker_dialog.h"

#include <QApplication>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPointer>
#include <QPushButton>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

#include "src/ui/lucide_icon_factory.h"

// ── 内部角色常量 ──────────────────────────────────────────────────────────────
static const int kModelNameRole  = Qt::UserRole + 1;   // item->data：模型名（纯文本）
static const int kStatusLabelRole = Qt::UserRole + 2;  // item->data：QLabel* 指针（以 quintptr 存储）

// ── helper ────────────────────────────────────────────────────────────────────
namespace {

QString normalizeBase(QString url)
{
    url = url.trimmed();
    while (url.endsWith(QLatin1Char('/')))
        url.chop(1);
    for (const QString &suffix : { QStringLiteral("/chat/completions"),
                                   QStringLiteral("/responses") }) {
        if (url.endsWith(suffix)) {
            url.chop(suffix.length());
            break;
        }
    }
    return url;
}

} // namespace

// ── ModelPickerDialog ─────────────────────────────────────────────────────────

ModelPickerDialog::ModelPickerDialog(const QString &baseUrl,
                                     const QString &apiKey,
                                     const QStringList &existingModels,
                                     const QString &currentModel,
                                     QWidget *parent)
    : QDialog(parent)
    , m_baseUrl(baseUrl)
    , m_apiKey(apiKey)
{
    setObjectName(QStringLiteral("modelPickerDialog"));
    setWindowTitle(tr("Select Models"));
    setWindowIcon(LucideIconFactory::icon(LucideIconFactory::IconType::Ai, QColor("#315efb"), 20));
    resize(620, 500);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 14);
    mainLayout->setSpacing(12);

    // ── 标题卡片 ──────────────────────────────────────────────────────────────
    auto *headerCard = new QFrame(this);
    headerCard->setObjectName(QStringLiteral("headerCard"));
    auto *headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setContentsMargins(16, 14, 16, 14);
    headerLayout->setSpacing(10);

    auto *iconLabel = new QLabel(headerCard);
    iconLabel->setObjectName(QStringLiteral("dialogIconBadge"));
    iconLabel->setPixmap(LucideIconFactory::pixmap(LucideIconFactory::IconType::Ai, 22, QColor("#ffffff")));
    iconLabel->setFixedSize(42, 42);
    iconLabel->setAlignment(Qt::AlignCenter);

    auto *titleCol = new QVBoxLayout();
    titleCol->setSpacing(2);
    auto *titleLbl = new QLabel(tr("Model Manager"), headerCard);
    titleLbl->setObjectName(QStringLiteral("dialogTitle"));
    auto *hintLbl = new QLabel(
        tr("Add models manually or fetch from the provider. Check items to enable. Click a row to select it as the active model."),
        headerCard);
    hintLbl->setObjectName(QStringLiteral("dialogHint"));
    hintLbl->setWordWrap(true);
    titleCol->addWidget(titleLbl);
    titleCol->addWidget(hintLbl);

    headerLayout->addWidget(iconLabel, 0, Qt::AlignTop);
    headerLayout->addLayout(titleCol, 1);
    mainLayout->addWidget(headerCard);

    // ── 手动添加行 ────────────────────────────────────────────────────────────
    auto *addRow = new QHBoxLayout();
    addRow->setSpacing(8);
    m_manualEdit = new QLineEdit(this);
    m_manualEdit->setPlaceholderText(tr("Enter model name, e.g. gpt-4o-mini"));
    auto *addButton = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::StatusOk, QColor("#198754"), 16),
        tr("Add"), this);
    addButton->setObjectName(QStringLiteral("dialogSecondaryButton"));
    addButton->setCursor(Qt::PointingHandCursor);
    addButton->setFixedWidth(80);
    addRow->addWidget(m_manualEdit, 1);
    addRow->addWidget(addButton);
    mainLayout->addLayout(addRow);

    // ── 模型列表 ──────────────────────────────────────────────────────────────
    // 使用 NoSelection 模式避免默认高亮干扰自定义 widget 的点击；
    // 改用 SingleSelection 让行可以被高亮选中（for "Use Selected"）。
    m_modelList = new QListWidget(this);
    m_modelList->setObjectName(QStringLiteral("codeTree"));
    m_modelList->setAlternatingRowColors(true);
    m_modelList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_modelList->setSpacing(1);
    mainLayout->addWidget(m_modelList, 1);

    // ── 操作按钮行 ────────────────────────────────────────────────────────────
    auto *actionRow = new QHBoxLayout();
    actionRow->setSpacing(8);

    m_fetchButton = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::Open, QColor("#2d6cdf"), 16),
        tr("Fetch Models"), this);
    m_fetchButton->setObjectName(QStringLiteral("secondaryActionButton"));
    m_fetchButton->setCursor(Qt::PointingHandCursor);
    m_fetchButton->setToolTip(tr("Fetch available models from the provider's /models endpoint"));

    m_testButton = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::Sparkles, QColor("#315efb"), 16),
        tr("Test Checked"), this);
    m_testButton->setObjectName(QStringLiteral("secondaryActionButton"));
    m_testButton->setCursor(Qt::PointingHandCursor);
    m_testButton->setEnabled(false);
    m_testButton->setToolTip(tr("Test all checked models and show status and latency per model"));

    m_removeButton = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::Exit, QColor("#c25027"), 16),
        tr("Remove"), this);
    m_removeButton->setObjectName(QStringLiteral("dialogSecondaryButton"));
    m_removeButton->setCursor(Qt::PointingHandCursor);
    m_removeButton->setEnabled(false);

    actionRow->addWidget(m_fetchButton);
    actionRow->addWidget(m_testButton);
    actionRow->addWidget(m_removeButton);
    actionRow->addStretch();
    mainLayout->addLayout(actionRow);

    // ── 全局状态标签（显示进度、fetch 结果等） ────────────────────────────────
    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QStringLiteral("dialogHint"));
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setText(tr("No test run yet."));
    mainLayout->addWidget(m_statusLabel);

    // ── 底部按钮 ──────────────────────────────────────────────────────────────
    auto *btnBox = new QDialogButtonBox(this);
    m_selectButton = btnBox->addButton(tr("Use Selected"), QDialogButtonBox::AcceptRole);
    m_selectButton->setObjectName(QStringLiteral("dialogPrimaryButton"));
    m_selectButton->setEnabled(false);
    auto *cancelButton = btnBox->addButton(QDialogButtonBox::Cancel);
    cancelButton->setObjectName(QStringLiteral("dialogSecondaryButton"));
    cancelButton->setText(tr("Cancel"));
    mainLayout->addWidget(btnBox);

    // ── 初始填充 ──────────────────────────────────────────────────────────────
    for (const QString &id : existingModels) {
        QListWidgetItem *item = appendModelRow(id, true);
        if (id == currentModel) {
            m_modelList->setCurrentItem(item);
        }
    }
    if (!m_modelList->currentItem() && m_modelList->count() > 0) {
        m_modelList->setCurrentRow(0);
    }
    updateTestButtonState();

    // ── 信号 ──────────────────────────────────────────────────────────────────
    connect(addButton,      &QPushButton::clicked,       this, &ModelPickerDialog::addManualModel);
    connect(m_manualEdit,   &QLineEdit::returnPressed,   this, &ModelPickerDialog::addManualModel);
    connect(m_fetchButton,  &QPushButton::clicked,       this, &ModelPickerDialog::fetchModels);
    connect(m_testButton,   &QPushButton::clicked,       this, &ModelPickerDialog::testCheckedModels);
    connect(m_removeButton, &QPushButton::clicked,       this, &ModelPickerDialog::removeSelectedModel);
    connect(m_modelList,    &QListWidget::itemSelectionChanged,
            this, &ModelPickerDialog::onItemSelectionChanged);
    connect(m_modelList,    &QListWidget::itemChanged,   this, &ModelPickerDialog::updateTestButtonState);
    connect(m_selectButton, &QPushButton::clicked,       this, &ModelPickerDialog::acceptSelection);
    connect(cancelButton,   &QPushButton::clicked,       this, &QDialog::reject);
}

// ── public ────────────────────────────────────────────────────────────────────

QString ModelPickerDialog::selectedModel() const
{
    QListWidgetItem *cur = m_modelList->currentItem();
    if (!cur)
        return {};
    return cur->data(kModelNameRole).toString();
}

QStringList ModelPickerDialog::models() const
{
    QStringList result;
    for (int i = 0; i < m_modelList->count(); ++i) {
        const QString name = m_modelList->item(i)->data(kModelNameRole).toString();
        if (!name.isEmpty())
            result.append(name);
    }
    return result;
}

// ── private slots ─────────────────────────────────────────────────────────────

void ModelPickerDialog::addManualModel()
{
    const QString name = m_manualEdit->text().trimmed();
    if (name.isEmpty())
        return;

    // 去重
    for (int i = 0; i < m_modelList->count(); ++i) {
        if (m_modelList->item(i)->data(kModelNameRole).toString() == name) {
            m_modelList->setCurrentRow(i);
            m_manualEdit->clear();
            return;
        }
    }
    QListWidgetItem *item = appendModelRow(name, true);
    m_modelList->setCurrentItem(item);
    m_manualEdit->clear();
    updateTestButtonState();
}

void ModelPickerDialog::fetchModels()
{
    const QString base = normalizedEndpointBase();
    if (base.isEmpty() || m_apiKey.trimmed().isEmpty()) {
        m_statusLabel->setText(tr("Base URL or API Key is missing; cannot fetch models."));
        return;
    }

    m_fetchButton->setEnabled(false);
    m_statusLabel->setText(tr("Fetching models..."));
    QApplication::processEvents();

    QNetworkRequest request(QUrl(base + QStringLiteral("/models")));
    request.setRawHeader("Authorization",
                         QStringLiteral("Bearer %1").arg(m_apiKey).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QNetworkAccessManager manager;
    QPointer<QNetworkReply> reply = manager.get(request);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout,         &loop, &QEventLoop::quit);
    QObject::connect(reply,  &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(15000);
    loop.exec();

    m_fetchButton->setEnabled(true);

    if (!reply) {
        m_statusLabel->setText(tr("Request was canceled."));
        return;
    }
    if (!timer.isActive()) {
        reply->abort();
        reply->deleteLater();
        m_statusLabel->setText(tr("Fetch timed out."));
        return;
    }
    timer.stop();

    const QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError pe;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &pe);
    if (pe.error != QJsonParseError::NoError || !doc.isObject()) {
        m_statusLabel->setText(tr("Failed to parse response from /models."));
        return;
    }

    const QJsonObject root = doc.object();
    if (root.contains(QStringLiteral("error"))) {
        const QString msg = root.value(QStringLiteral("error")).toObject()
                                .value(QStringLiteral("message"))
                                .toString(tr("Unknown error"));
        m_statusLabel->setText(tr("API error: %1").arg(msg));
        return;
    }

    QStringList fetched;
    for (const QJsonValue &v : root.value(QStringLiteral("data")).toArray()) {
        const QString id = v.toObject().value(QStringLiteral("id")).toString();
        if (!id.isEmpty())
            fetched.append(id);
    }
    fetched.sort(Qt::CaseInsensitive);

    if (fetched.isEmpty()) {
        m_statusLabel->setText(tr("No models returned by the provider."));
        return;
    }

    const QStringList existing = models();
    int added = 0;
    for (const QString &id : std::as_const(fetched)) {
        if (!existing.contains(id)) {
            appendModelRow(id, true);
            ++added;
        }
    }
    m_statusLabel->setText(tr("Fetched %1 model(s); %2 new added.").arg(fetched.size()).arg(added));
    updateTestButtonState();
}

void ModelPickerDialog::testCheckedModels()
{
    // 收集所有勾选的 items
    QList<QListWidgetItem *> checkedItems;
    for (int i = 0; i < m_modelList->count(); ++i) {
        QListWidgetItem *item = m_modelList->item(i);
        if (item->checkState() == Qt::Checked)
            checkedItems.append(item);
    }
    if (checkedItems.isEmpty()) {
        m_statusLabel->setText(tr("No checked models to test."));
        return;
    }

    const QString base = normalizedEndpointBase();
    if (base.isEmpty() || m_apiKey.trimmed().isEmpty()) {
        m_statusLabel->setText(tr("Base URL or API Key is missing; cannot test."));
        return;
    }

    m_testButton->setEnabled(false);
    m_fetchButton->setEnabled(false);
    m_statusLabel->setText(tr("Testing %1 checked model(s)...").arg(checkedItems.size()));

    for (QListWidgetItem *item : std::as_const(checkedItems)) {
        const QString modelName = item->data(kModelNameRole).toString();
        // 先显示"测试中..."
        setRowStatus(item, tr("Testing..."), true);
        QApplication::processEvents();

        const TestResult r = doTestOne(modelName);

        if (r.ok) {
            setRowStatus(item, tr("OK  %1 ms").arg(r.ms), true);
        } else {
            setRowStatus(item, tr("FAIL  %1 ms").arg(r.ms), false);
        }
        QApplication::processEvents();
    }

    m_testButton->setEnabled(true);
    m_fetchButton->setEnabled(true);
    m_statusLabel->setText(tr("All tests completed."));
}

void ModelPickerDialog::removeSelectedModel()
{
    QListWidgetItem *cur = m_modelList->currentItem();
    if (!cur)
        return;
    // 先移除 item widget，再 takeItem
    m_modelList->removeItemWidget(cur);
    delete m_modelList->takeItem(m_modelList->row(cur));
    updateTestButtonState();
}

void ModelPickerDialog::onItemSelectionChanged()
{
    const bool hasSelection = (m_modelList->currentItem() != nullptr);
    m_removeButton->setEnabled(hasSelection);
    m_selectButton->setEnabled(hasSelection);
}

void ModelPickerDialog::updateTestButtonState()
{
    bool hasChecked = false;
    for (int i = 0; i < m_modelList->count(); ++i) {
        if (m_modelList->item(i)->checkState() == Qt::Checked) {
            hasChecked = true;
            break;
        }
    }
    m_testButton->setEnabled(hasChecked);
}

void ModelPickerDialog::acceptSelection()
{
    if (selectedModel().isEmpty())
        return;
    accept();
}

// ── private helpers ───────────────────────────────────────────────────────────

QListWidgetItem *ModelPickerDialog::appendModelRow(const QString &modelId,
                                                   bool checked,
                                                   const QString &statusText)
{
    // 创建 item，设置角色数据
    auto *item = new QListWidgetItem();
    item->setData(kModelNameRole, modelId);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    // 自定义行 widget：[模型名 label，弹性空间，状态 label]
    auto *rowWidget = new QWidget();
    rowWidget->setAttribute(Qt::WA_TranslucentBackground);
    auto *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(4, 2, 8, 2);
    rowLayout->setSpacing(8);

    auto *nameLbl = new QLabel(modelId, rowWidget);
    nameLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto *statusLbl = new QLabel(statusText, rowWidget);
    statusLbl->setObjectName(QStringLiteral("modelStatusLabel"));
    statusLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    statusLbl->setMinimumWidth(120);

    rowLayout->addWidget(nameLbl, 1);
    rowLayout->addWidget(statusLbl, 0);

    // 把 statusLbl 指针存到 item 中，后续测试时更新
    item->setData(kStatusLabelRole, QVariant::fromValue(static_cast<void *>(statusLbl)));

    m_modelList->addItem(item);
    item->setSizeHint(rowWidget->sizeHint());
    m_modelList->setItemWidget(item, rowWidget);

    return item;
}

void ModelPickerDialog::setRowStatus(QListWidgetItem *item, const QString &text, bool ok)
{
    auto *lbl = static_cast<QLabel *>(item->data(kStatusLabelRole).value<void *>());
    if (!lbl)
        return;
    lbl->setText(text);
    // 成功用绿色，失败用红色
    lbl->setStyleSheet(ok
        ? QStringLiteral("color: #198754; font-weight: bold;")
        : QStringLiteral("color: #c25027; font-weight: bold;"));
}

ModelPickerDialog::TestResult ModelPickerDialog::doTestOne(const QString &modelName)
{
    const QString base = normalizedEndpointBase();

    QJsonObject sysMsg;
    sysMsg[QStringLiteral("role")]    = QStringLiteral("system");
    sysMsg[QStringLiteral("content")] = QStringLiteral("Reply only with the word OK.");

    QJsonObject userMsg;
    userMsg[QStringLiteral("role")]    = QStringLiteral("user");
    userMsg[QStringLiteral("content")] = QStringLiteral("ping");

    QJsonObject body;
    body[QStringLiteral("model")]       = modelName;
    body[QStringLiteral("messages")]    = QJsonArray{ sysMsg, userMsg };
    body[QStringLiteral("max_tokens")]  = 16;
    body[QStringLiteral("temperature")] = 0.0;

    QNetworkRequest request(QUrl(base + QStringLiteral("/chat/completions")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization",
                         QStringLiteral("Bearer %1").arg(m_apiKey).toUtf8());

    QNetworkAccessManager manager;
    QElapsedTimer elapsed;
    elapsed.start();

    QPointer<QNetworkReply> reply =
        manager.post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout,         &loop, &QEventLoop::quit);
    QObject::connect(reply,  &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(30000);
    loop.exec();

    if (!reply)
        return { false, elapsed.elapsed(), tr("Test canceled.") };

    if (!timer.isActive()) {
        reply->abort();
        reply->deleteLater();
        return { false, 30000, tr("Timed out") };
    }
    timer.stop();
    const qint64 ms = elapsed.elapsed();

    const QByteArray data  = reply->readAll();
    const int        code  = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const bool       netOk = (reply->error() == QNetworkReply::NoError);
    reply->deleteLater();

    QJsonParseError pe;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &pe);

    if (netOk && pe.error == QJsonParseError::NoError && doc.isObject()) {
        const QJsonObject root = doc.object();
        if (root.contains(QStringLiteral("error"))) {
            const QString msg = root.value(QStringLiteral("error")).toObject()
                                    .value(QStringLiteral("message"))
                                    .toString(tr("API error"));
            return { false, ms, msg };
        }
        if (!root.value(QStringLiteral("choices")).toArray().isEmpty()) {
            return { true, ms, tr("Response received.") };
        }
        return { false, ms, tr("Unexpected response format.") };
    }
    if (!netOk)
        return { false, ms, tr("HTTP %1").arg(code) };
    return { false, ms, tr("Invalid JSON response.") };
}

QString ModelPickerDialog::normalizedEndpointBase() const
{
    return normalizeBase(m_baseUrl);
}
