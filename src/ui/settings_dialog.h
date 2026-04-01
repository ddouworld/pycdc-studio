#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QList>

#include "src/ai/ai_provider_config.h"

class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QPushButton;

/**
 * SettingsDialog — 应用设置对话框
 *
 * 支持多 AI Provider 管理：
 *  - 左侧列表显示已配置的 providers
 *  - 右侧表单编辑选中 provider 的各字段
 *  - 模型字段旁有"选择模型"按钮，打开 ModelPickerDialog
 *  - 底部显示语言选择
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    bool restartRequired() const { return m_restartRequired; }

private slots:
    void addProvider();
    void removeProvider();
    void moveProviderUp();
    void moveProviderDown();
    void onProviderSelected(int row);
    void onProviderNameEdited(const QString &text);
    void openModelPicker();
    void saveAndAccept();

private:
    void buildProviderListPanel(class QSplitter *splitter);
    void buildProviderFormPanel(QSplitter *splitter);

    void loadValues();
    void flushCurrentProviderToCache();
    void applyProviderToForm(int index);

    // Provider list panel
    QListWidget   *m_providerList   = nullptr;
    QPushButton   *m_addBtn         = nullptr;
    QPushButton   *m_removeBtn      = nullptr;
    QPushButton   *m_upBtn          = nullptr;
    QPushButton   *m_downBtn        = nullptr;

    // Provider form fields
    QGroupBox     *m_formGroup      = nullptr;
    QLineEdit     *m_nameEdit       = nullptr;
    QLineEdit     *m_baseUrlEdit    = nullptr;
    QLineEdit     *m_apiKeyEdit     = nullptr;
    QLineEdit     *m_modelEdit      = nullptr;
    QPushButton   *m_modelPickBtn   = nullptr;
    QPlainTextEdit *m_systemPromptEdit = nullptr;

    // Language
    QComboBox     *m_languageCombo  = nullptr;

    // State
    QList<AiProviderConfig> m_providers;
    int               m_currentRow      = -1;
    QString           m_initialLanguage;
    bool              m_restartRequired = false;
};

#endif // SETTINGS_DIALOG_H
