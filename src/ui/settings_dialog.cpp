#include "src/ui/settings_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFrame>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QVBoxLayout>

#include "src/ai/ai_provider_config.h"
#include "src/app/app_settings.h"
#include "src/ui/lucide_icon_factory.h"
#include "src/ui/model_picker_dialog.h"

// ── SettingsDialog ────────────────────────────────────────────────────────────

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setObjectName(QStringLiteral("settingsDialog"));
    setWindowTitle(tr("Settings"));
    setWindowIcon(LucideIconFactory::icon(LucideIconFactory::IconType::Settings, QColor("#1b3b5d"), 20));
    resize(820, 580);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    // ── 标题卡片 ──────────────────────────────────────────────────────────────
    auto *headerCard = new QFrame(this);
    headerCard->setObjectName(QStringLiteral("headerCard"));
    auto *headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setContentsMargins(18, 16, 18, 16);
    headerLayout->setSpacing(12);

    auto *iconBadge = new QLabel(headerCard);
    iconBadge->setObjectName(QStringLiteral("dialogIconBadge"));
    iconBadge->setPixmap(LucideIconFactory::pixmap(LucideIconFactory::IconType::Settings, 24, QColor("#ffffff")));
    iconBadge->setFixedSize(48, 48);
    iconBadge->setAlignment(Qt::AlignCenter);

    auto *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);
    auto *titleLabel = new QLabel(tr("Application Settings"), headerCard);
    titleLabel->setObjectName(QStringLiteral("dialogTitle"));
    auto *hintLabel = new QLabel(
        tr("Manage AI providers and models. The active provider is used for AI reconstruction. Language changes apply after restart."),
        headerCard);
    hintLabel->setObjectName(QStringLiteral("dialogHint"));
    hintLabel->setWordWrap(true);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(hintLabel);
    headerLayout->addWidget(iconBadge, 0, Qt::AlignTop);
    headerLayout->addLayout(titleLayout, 1);
    layout->addWidget(headerCard);

    // ── 主分割区（左: provider 列表，右: 表单）────────────────────────────────
    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setObjectName(QStringLiteral("workspaceSplitter"));
    buildProviderListPanel(splitter);
    buildProviderFormPanel(splitter);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setSizes({ 200, 560 });
    layout->addWidget(splitter, 1);

    // ── 语言 + 底部按钮 ───────────────────────────────────────────────────────
    auto *bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(14);

    auto *langLabel = new QLabel(tr("Language:"), this);
    m_languageCombo = new QComboBox(this);
    m_languageCombo->addItem(tr("English"),  QStringLiteral("en"));
    m_languageCombo->addItem(tr("简体中文"), QStringLiteral("zh_CN"));
    bottomRow->addWidget(langLabel);
    bottomRow->addWidget(m_languageCombo);
    bottomRow->addStretch();

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    if (QPushButton *saveButton = buttonBox->button(QDialogButtonBox::Save)) {
        saveButton->setText(tr("Save"));
        saveButton->setObjectName(QStringLiteral("dialogPrimaryButton"));
    }
    if (QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel)) {
        cancelButton->setText(tr("Cancel"));
        cancelButton->setObjectName(QStringLiteral("dialogSecondaryButton"));
    }
    bottomRow->addWidget(buttonBox);
    layout->addLayout(bottomRow);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::saveAndAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    loadValues();
}

// ── build helpers ─────────────────────────────────────────────────────────────

void SettingsDialog::buildProviderListPanel(QSplitter *splitter)
{
    auto *panel = new QFrame(splitter);
    panel->setObjectName(QStringLiteral("formCard"));
    auto *vl = new QVBoxLayout(panel);
    vl->setContentsMargins(12, 12, 12, 12);
    vl->setSpacing(8);

    auto *listTitle = new QLabel(tr("Providers"), panel);
    listTitle->setObjectName(QStringLiteral("dialogTitle"));
    vl->addWidget(listTitle);

    m_providerList = new QListWidget(panel);
    m_providerList->setObjectName(QStringLiteral("codeTree"));
    m_providerList->setAlternatingRowColors(true);
    vl->addWidget(m_providerList, 1);

    // 操作按钮
    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(4);

    m_addBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::StatusOk, QColor("#198754"), 15),
        tr("Add"), panel);
    m_addBtn->setObjectName(QStringLiteral("dialogSecondaryButton"));
    m_addBtn->setCursor(Qt::PointingHandCursor);
    m_addBtn->setToolTip(tr("Add new provider"));

    m_removeBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::Exit, QColor("#c25027"), 15),
        tr("Remove"), panel);
    m_removeBtn->setObjectName(QStringLiteral("dialogSecondaryButton"));
    m_removeBtn->setCursor(Qt::PointingHandCursor);
    m_removeBtn->setEnabled(false);
    m_removeBtn->setToolTip(tr("Remove selected provider"));

    m_upBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::StatusWarning, QColor("#9a6700"), 15),
        QString(), panel);
    m_upBtn->setObjectName(QStringLiteral("dialogSecondaryButton"));
    m_upBtn->setCursor(Qt::PointingHandCursor);
    m_upBtn->setEnabled(false);
    m_upBtn->setFixedWidth(32);
    m_upBtn->setToolTip(tr("Move up"));

    m_downBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::StatusWarning, QColor("#9a6700"), 15),
        QString(), panel);
    m_downBtn->setObjectName(QStringLiteral("dialogSecondaryButton"));
    m_downBtn->setCursor(Qt::PointingHandCursor);
    m_downBtn->setEnabled(false);
    m_downBtn->setFixedWidth(32);
    m_downBtn->setToolTip(tr("Move down"));

    btnRow->addWidget(m_addBtn);
    btnRow->addWidget(m_removeBtn);
    btnRow->addStretch();
    btnRow->addWidget(m_upBtn);
    btnRow->addWidget(m_downBtn);
    vl->addLayout(btnRow);

    connect(m_addBtn,        &QPushButton::clicked, this, &SettingsDialog::addProvider);
    connect(m_removeBtn,     &QPushButton::clicked, this, &SettingsDialog::removeProvider);
    connect(m_upBtn,         &QPushButton::clicked, this, &SettingsDialog::moveProviderUp);
    connect(m_downBtn,       &QPushButton::clicked, this, &SettingsDialog::moveProviderDown);
    connect(m_providerList,  &QListWidget::currentRowChanged, this, &SettingsDialog::onProviderSelected);
}

void SettingsDialog::buildProviderFormPanel(QSplitter *splitter)
{
    auto *panel = new QFrame(splitter);
    panel->setObjectName(QStringLiteral("formCard"));
    auto *vl = new QVBoxLayout(panel);
    vl->setContentsMargins(12, 12, 12, 12);
    vl->setSpacing(8);

    m_formGroup = new QGroupBox(tr("Provider Configuration"), panel);
    m_formGroup->setEnabled(false);
    auto *formLayout = new QFormLayout(m_formGroup);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->setHorizontalSpacing(14);
    formLayout->setVerticalSpacing(10);

    m_nameEdit = new QLineEdit(m_formGroup);
    m_nameEdit->setPlaceholderText(tr("e.g. OpenAI, DeepSeek, Local Ollama"));

    m_baseUrlEdit = new QLineEdit(m_formGroup);
    m_baseUrlEdit->setPlaceholderText(tr("https://api.example.com/v1"));

    m_apiKeyEdit = new QLineEdit(m_formGroup);
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_apiKeyEdit->setPlaceholderText(tr("sk-..."));

    // 模型字段：输入框占满整行，Select Models 按钮在其下方
    m_modelEdit = new QLineEdit(m_formGroup);
    m_modelEdit->setPlaceholderText(tr("gpt-4o-mini / qwen-plus / ..."));

    // Select Models 按钮单独一行，左对齐
    auto *modelBtnRow = new QHBoxLayout();
    modelBtnRow->setSpacing(0);
    m_modelPickBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::Ai, QColor("#315efb"), 15),
        tr("Select Models..."), m_formGroup);
    m_modelPickBtn->setObjectName(QStringLiteral("dialogSecondaryButton"));
    m_modelPickBtn->setCursor(Qt::PointingHandCursor);
    m_modelPickBtn->setToolTip(tr("Open model manager to add, fetch, test and select models"));
    modelBtnRow->addWidget(m_modelPickBtn);
    modelBtnRow->addStretch();

    m_systemPromptEdit = new QPlainTextEdit(m_formGroup);
    m_systemPromptEdit->setObjectName(QStringLiteral("systemPromptEdit"));
    m_systemPromptEdit->setPlaceholderText(tr("Optional custom system prompt"));
    m_systemPromptEdit->setMinimumHeight(100);

    formLayout->addRow(tr("Name"),          m_nameEdit);
    formLayout->addRow(tr("Base URL"),      m_baseUrlEdit);
    formLayout->addRow(tr("API Key"),       m_apiKeyEdit);
    formLayout->addRow(tr("Model"),         m_modelEdit);
    formLayout->addRow(QString(),           modelBtnRow);
    formLayout->addRow(tr("System Prompt"), m_systemPromptEdit);

    vl->addWidget(m_formGroup, 1);

    connect(m_nameEdit,    &QLineEdit::textChanged, this, &SettingsDialog::onProviderNameEdited);
    connect(m_modelPickBtn, &QPushButton::clicked,  this, &SettingsDialog::openModelPicker);
}

// ── load / flush ──────────────────────────────────────────────────────────────

void SettingsDialog::loadValues()
{
    m_providers = AiProviderConfig::loadAll();

    // 若无已存数据，创建一个空 provider
    if (m_providers.isEmpty()) {
        AiProviderConfig def;
        def.name         = tr("Default");
        def.systemPrompt = QStringLiteral(
            "You reconstruct Python source code from bytecode metadata and disassembly. "
            "Return only Python source code.");
        m_providers.append(def);
    }

    m_providerList->clear();
    for (const AiProviderConfig &p : std::as_const(m_providers)) {
        m_providerList->addItem(p.name.isEmpty() ? tr("(unnamed)") : p.name);
    }

    m_initialLanguage = AppSettings::language();
    const int langIdx = m_languageCombo->findData(m_initialLanguage);
    if (langIdx >= 0) {
        m_languageCombo->setCurrentIndex(langIdx);
    }

    // 选中 active provider
    const int activeIdx = AiProviderConfig::loadActiveIndex();
    const int safeIdx   = (activeIdx >= 0 && activeIdx < m_providers.size()) ? activeIdx : 0;
    m_providerList->setCurrentRow(safeIdx);
}

void SettingsDialog::flushCurrentProviderToCache()
{
    if (m_currentRow < 0 || m_currentRow >= m_providers.size()) {
        return;
    }
    AiProviderConfig &cfg = m_providers[m_currentRow];
    cfg.name         = m_nameEdit->text().trimmed();
    cfg.baseUrl      = m_baseUrlEdit->text().trimmed();
    cfg.apiKey       = m_apiKeyEdit->text().trimmed();
    cfg.model        = m_modelEdit->text().trimmed();
    cfg.systemPrompt = m_systemPromptEdit->toPlainText().trimmed();
    // models 列表在 openModelPicker 时已写入，无需再处理
}

void SettingsDialog::applyProviderToForm(int index)
{
    if (index < 0 || index >= m_providers.size()) {
        m_formGroup->setEnabled(false);
        return;
    }
    m_formGroup->setEnabled(true);
    const AiProviderConfig &cfg = m_providers.at(index);
    m_nameEdit->setText(cfg.name);
    m_baseUrlEdit->setText(cfg.baseUrl);
    m_apiKeyEdit->setText(cfg.apiKey);
    m_modelEdit->setText(cfg.model);
    m_systemPromptEdit->setPlainText(cfg.systemPrompt);
}

// ── slots ─────────────────────────────────────────────────────────────────────

void SettingsDialog::addProvider()
{
    // 先保存当前编辑
    flushCurrentProviderToCache();

    AiProviderConfig p;
    p.name         = tr("New Provider");
    p.systemPrompt = QStringLiteral(
        "You reconstruct Python source code from bytecode metadata and disassembly. "
        "Return only Python source code.");
    m_providers.append(p);
    m_providerList->addItem(p.name);
    m_providerList->setCurrentRow(m_providers.size() - 1);
}

void SettingsDialog::removeProvider()
{
    const int row = m_providerList->currentRow();
    if (row < 0 || row >= m_providers.size()) {
        return;
    }
    m_providers.removeAt(row);
    delete m_providerList->takeItem(row);
    m_currentRow = -1;
    if (m_providers.isEmpty()) {
        m_formGroup->setEnabled(false);
    } else {
        const int newRow = qMin(row, m_providers.size() - 1);
        m_providerList->setCurrentRow(newRow);
    }
}

void SettingsDialog::moveProviderUp()
{
    const int row = m_providerList->currentRow();
    if (row <= 0) {
        return;
    }
    flushCurrentProviderToCache();
    m_providers.swapItemsAt(row, row - 1);
    auto *item = m_providerList->takeItem(row);
    m_providerList->insertItem(row - 1, item);
    m_currentRow = -1;
    m_providerList->setCurrentRow(row - 1);
}

void SettingsDialog::moveProviderDown()
{
    const int row = m_providerList->currentRow();
    if (row < 0 || row >= m_providers.size() - 1) {
        return;
    }
    flushCurrentProviderToCache();
    m_providers.swapItemsAt(row, row + 1);
    auto *item = m_providerList->takeItem(row);
    m_providerList->insertItem(row + 1, item);
    m_currentRow = -1;
    m_providerList->setCurrentRow(row + 1);
}

void SettingsDialog::onProviderSelected(int row)
{
    flushCurrentProviderToCache();
    m_currentRow = row;
    applyProviderToForm(row);

    const bool valid = (row >= 0 && row < m_providers.size());
    m_removeBtn->setEnabled(valid);
    m_upBtn->setEnabled(valid && row > 0);
    m_downBtn->setEnabled(valid && row < m_providers.size() - 1);
}

void SettingsDialog::onProviderNameEdited(const QString &text)
{
    const int row = m_providerList->currentRow();
    if (row < 0 || row >= m_providers.size()) {
        return;
    }
    const QString display = text.trimmed().isEmpty() ? tr("(unnamed)") : text.trimmed();
    m_providerList->item(row)->setText(display);
}

void SettingsDialog::openModelPicker()
{
    // 先把当前编辑同步到缓存，以便获取最新 baseUrl/apiKey
    flushCurrentProviderToCache();

    const int row = m_providerList->currentRow();
    if (row < 0 || row >= m_providers.size()) {
        return;
    }
    AiProviderConfig &cfg = m_providers[row];

    ModelPickerDialog dlg(cfg.baseUrl, cfg.apiKey, cfg.models, cfg.model, this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    cfg.models = dlg.models();
    const QString chosen = dlg.selectedModel();
    if (!chosen.isEmpty()) {
        cfg.model = chosen;
        m_modelEdit->setText(chosen);
    }
}

void SettingsDialog::saveAndAccept()
{
    flushCurrentProviderToCache();

    const int activeRow = qMax(0, m_providerList->currentRow());
    AiProviderConfig::saveAll(m_providers, activeRow);

    AppSettings::setLanguage(m_languageCombo->currentData().toString());
    m_restartRequired = (m_languageCombo->currentData().toString() != m_initialLanguage);
    accept();
}
