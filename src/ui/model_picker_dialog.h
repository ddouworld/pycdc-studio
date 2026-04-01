#ifndef MODEL_PICKER_DIALOG_H
#define MODEL_PICKER_DIALOG_H

#include <QDialog>
#include <QStringList>

class QCheckBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;

/**
 * ModelPickerDialog — 模型选择与批量测试窗口
 *
 * 功能：
 *  - 手动输入模型名并添加到列表
 *  - 点击"拉取模型"从 /models 接口自动获取并合并
 *  - 每行：勾选框控制是否启用 + 模型名 + 右侧测试状态标签
 *  - "测试勾选项"：对所有勾选的模型逐一发测试请求，
 *    结果（OK/FAIL + 耗时 ms）实时更新到各行右侧
 *  - 鼠标单击选中行 → "使用选中模型"按钮可用
 *
 * 关闭后通过 selectedModel() 获取当前高亮行的模型名，
 * 通过 models() 获取完整已配置列表（含未勾选项）。
 */
class ModelPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelPickerDialog(const QString &baseUrl,
                               const QString &apiKey,
                               const QStringList &existingModels,
                               const QString &currentModel,
                               QWidget *parent = nullptr);

    /** 当前鼠标选中（高亮）的模型名 */
    QString selectedModel() const;

    /** 完整的已配置模型列表（所有行，无论是否勾选） */
    QStringList models() const;

private slots:
    void addManualModel();
    void fetchModels();
    void testCheckedModels();   // 批量测试所有勾选项
    void removeSelectedModel();
    void onItemSelectionChanged();
    void updateTestButtonState();
    void acceptSelection();

private:
    // 向列表添加一行，返回新建的 item
    QListWidgetItem *appendModelRow(const QString &modelId,
                                   bool checked,
                                   const QString &statusText = {});

    // 在指定 item 行右侧更新状态文字
    void setRowStatus(QListWidgetItem *item, const QString &text, bool ok);

    QString normalizedEndpointBase() const;

    // 对单个模型发同步测试请求，返回 (success, ms, detail)
    struct TestResult { bool ok; qint64 ms; QString detail; };
    TestResult doTestOne(const QString &modelName);

    QString m_baseUrl;
    QString m_apiKey;

    QLineEdit   *m_manualEdit   = nullptr;
    QListWidget *m_modelList    = nullptr;
    QPushButton *m_fetchButton  = nullptr;
    QPushButton *m_testButton   = nullptr;
    QPushButton *m_removeButton = nullptr;
    QLabel      *m_statusLabel  = nullptr;
    QPushButton *m_selectButton = nullptr;
};

#endif // MODEL_PICKER_DIALOG_H
