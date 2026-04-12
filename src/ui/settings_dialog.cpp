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
#include <QSignalBlocker>
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
    auto *footerCard = new QFrame(this);
    footerCard->setObjectName(QStringLiteral("settingsFooterCard"));
    auto *bottomRow = new QHBoxLayout(footerCard);
    bottomRow->setContentsMargins(16, 14, 16, 14);
    bottomRow->setSpacing(14);

    auto *langLabel = new QLabel(tr("Language:"), this);
    langLabel->setObjectName(QStringLiteral("fieldHint"));
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
    layout->addWidget(footerCard);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::saveAndAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    loadValues();
}

// ── build helpers ─────────────────────────────────────────────────────────────

void SettingsDialog::buildProviderListPanel(QSplitter *splitter)
{
    auto *panel = new QFrame(splitter);
    panel->setObjectName(QStringLiteral("settingsListCard"));
    auto *vl = new QVBoxLayout(panel);
    vl->setContentsMargins(14, 14, 14, 14);
    vl->setSpacing(10);

    auto *titleRow = new QHBoxLayout();
    titleRow->setSpacing(8);
    auto *listTitle = new QLabel(tr("Providers"), panel);
    listTitle->setObjectName(QStringLiteral("settingsSectionTitle"));
    m_providerCountBadge = new QLabel(panel);
    m_providerCountBadge->setObjectName(QStringLiteral("settingsCountBadge"));
    m_providerCountBadge->setAlignment(Qt::AlignCenter);
    titleRow->addWidget(listTitle);
    titleRow->addStretch();
    titleRow->addWidget(m_providerCountBadge);
    vl->addLayout(titleRow);

    auto *listCaption = new QLabel(tr("Switch between providers, keep backups for different vendors, and reorder your preferred defaults."), panel);
    listCaption->setObjectName(QStringLiteral("settingsSectionCaption"));
    listCaption->setWordWrap(true);
    vl->addWidget(listCaption);

    m_providerList = new QListWidget(panel);
    m_providerList->setObjectName(QStringLiteral("providerList"));
    m_providerList->setAlternatingRowColors(true);
    m_providerList->setSpacing(6);
    vl->addWidget(m_providerList, 1);

    // 操作按钮
    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(4);

    m_addBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::Plus, QColor("#198754"), 15),
        tr("Add"), panel);
    m_addBtn->setObjectName(QStringLiteral("dialogSecondaryButton"));
    m_addBtn->setCursor(Qt::PointingHandCursor);
    m_addBtn->setToolTip(tr("Add new provider"));

    m_removeBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::Exit, QColor("#c25027"), 15),
        tr("Remove"), panel);
    m_removeBtn->setObjectName(QStringLiteral("dangerButton"));
    m_removeBtn->setCursor(Qt::PointingHandCursor);
    m_removeBtn->setEnabled(false);
    m_removeBtn->setToolTip(tr("Remove selected provider"));

    m_upBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::ChevronUp, QColor("#9a6700"), 15),
        QString(), panel);
    m_upBtn->setObjectName(QStringLiteral("dialogToolButton"));
    m_upBtn->setCursor(Qt::PointingHandCursor);
    m_upBtn->setEnabled(false);
    m_upBtn->setFixedWidth(38);
    m_upBtn->setToolTip(tr("Move up"));

    m_downBtn = new QPushButton(
        LucideIconFactory::icon(LucideIconFactory::IconType::ChevronDown, QColor("#9a6700"), 15),
        QString(), panel);
    m_downBtn->setObjectName(QStringLiteral("dialogToolButton"));
    m_downBtn->setCursor(Qt::PointingHandCursor);
    m_downBtn->setEnabled(false);
    m_downBtn->setFixedWidth(38);
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
    panel->setObjectName(QStringLiteral("settingsFormCard"));
    auto *vl = new QVBoxLayout(panel);
    vl->setContentsMargins(16, 16, 16, 16);
    vl->setSpacing(12);

    auto *formHeaderRow = new QHBoxLayout();
    formHeaderRow->setSpacing(10);
    auto *formTitle = new QLabel(tr("Provider Configuration"), panel);
    formTitle->setObjectName(QStringLiteral("settingsSectionTitle"));
    m_providerStateBadge = new QLabel(panel);
    m_providerStateBadge->setObjectName(QStringLiteral("providerStateBadge"));
    m_providerStateBadge->setAlignment(Qt::AlignCenter);
    formHeaderRow->addWidget(formTitle);
    formHeaderRow->addStretch();
    formHeaderRow->addWidget(m_providerStateBadge);
    vl->addLayout(formHeaderRow);

    auto *formCaption = new QLabel(tr("Edit the endpoint, credentials, model preference and system prompt for the selected provider."), panel);
    formCaption->setObjectName(QStringLiteral("settingsSectionCaption"));
    formCaption->setWordWrap(true);
    vl->addWidget(formCaption);

    m_formGroup = new QGroupBox(panel);
    m_formGroup->setObjectName(QStringLiteral("providerFormGroup"));
    m_formGroup->setEnabled(false);
    auto *formLayout = new QFormLayout(m_formGroup);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formLayout->setHorizontalSpacing(14);
    formLayout->setVerticalSpacing(12);

    m_nameEdit = new QLineEdit(m_formGroup);
    m_nameEdit->setPlaceholderText(tr("e.g. OpenAI, DeepSeek, Local Ollama"));
    m_nameEdit->setClearButtonEnabled(true);

    m_baseUrlEdit = new QLineEdit(m_formGroup);
    m_baseUrlEdit->setPlaceholderText(tr("https://api.example.com/v1"));
    m_baseUrlEdit->setClearButtonEnabled(true);

    m_apiKeyEdit = new QLineEdit(m_formGroup);
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_apiKeyEdit->setPlaceholderText(tr("sk-..."));
    m_apiKeyEdit->setClearButtonEnabled(true);

    // 模型字段：输入框占满整行，Select Models 按钮在其下方
    m_modelEdit = new QLineEdit(m_formGroup);
    m_modelEdit->setPlaceholderText(tr("gpt-4o-mini / qwen-plus / ..."));
    m_modelEdit->setClearButtonEnabled(true);

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
    setCurrentProviderRow(safeIdx);
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
        m_nameEdit->clear();
        m_baseUrlEdit->clear();
        m_apiKeyEdit->clear();
        m_modelEdit->clear();
        m_systemPromptEdit->clear();
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

void SettingsDialog::setCurrentProviderRow(int row)
{
    {
        const QSignalBlocker blocker(m_providerList);
        m_providerList->setCurrentRow(row);
    }
    m_currentRow = row;
    applyProviderToForm(row);
    updateProviderActionStates(row);
    refreshProviderMeta(row);
}

void SettingsDialog::updateProviderActionStates(int row)
{
    const bool valid = (row >= 0 && row < m_providers.size());
    m_removeBtn->setEnabled(valid);
    m_upBtn->setEnabled(valid && row > 0);
    m_downBtn->setEnabled(valid && row < m_providers.size() - 1);
}

void SettingsDialog::refreshProviderMeta(int row)
{
    if (m_providerCountBadge) {
        m_providerCountBadge->setText(QString::number(m_providers.size()));
    }

    if (!m_providerStateBadge) {
        return;
    }

    if (row < 0 || row >= m_providers.size()) {
        m_providerStateBadge->clear();
        m_providerStateBadge->setVisible(false);
        return;
    }

    const QString name = m_providers.at(row).name.trimmed();
    m_providerStateBadge->setText(name.isEmpty() ? tr("(unnamed)") : name);
    m_providerStateBadge->setVisible(true);
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
    setCurrentProviderRow(m_providers.size() - 1);
}

void SettingsDialog::removeProvider()
{
    const int row = m_providerList->currentRow();
    if (row < 0 || row >= m_providers.size()) {
        return;
    }
    m_providers.removeAt(row);
    {
        const QSignalBlocker blocker(m_providerList);
        delete m_providerList->takeItem(row);
    }
    if (m_providers.isEmpty()) {
        setCurrentProviderRow(-1);
    } else {
        const int newRow = qMin(row, m_providers.size() - 1);
        setCurrentProviderRow(newRow);
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
    {
        const QSignalBlocker blocker(m_providerList);
        auto *item = m_providerList->takeItem(row);
        m_providerList->insertItem(row - 1, item);
    }
    setCurrentProviderRow(row - 1);
}

void SettingsDialog::moveProviderDown()
{
    const int row = m_providerList->currentRow();
    if (row < 0 || row >= m_providers.size() - 1) {
        return;
    }
    flushCurrentProviderToCache();
    m_providers.swapItemsAt(row, row + 1);
    {
        const QSignalBlocker blocker(m_providerList);
        auto *item = m_providerList->takeItem(row);
        m_providerList->insertItem(row + 1, item);
    }
    setCurrentProviderRow(row + 1);
}

void SettingsDialog::onProviderSelected(int row)
{
    flushCurrentProviderToCache();
    m_currentRow = row;
    applyProviderToForm(row);
    updateProviderActionStates(row);
    refreshProviderMeta(row);
}

void SettingsDialog::onProviderNameEdited(const QString &text)
{
    const int row = m_providerList->currentRow();
    if (row < 0 || row >= m_providers.size()) {
        return;
    }
    const QString display = text.trimmed().isEmpty() ? tr("(unnamed)") : text.trimmed();
    m_providerList->item(row)->setText(display);
    if (row == m_currentRow && m_providerStateBadge) {
        m_providerStateBadge->setText(display);
        m_providerStateBadge->setVisible(true);
    }
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
