#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class QAction;
class AppContext;
class QComboBox;
class QDragEnterEvent;
class QDropEvent;
class QPlainTextEdit;
class QTabWidget;
class QTreeWidget;
class QTreeWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(AppContext *context, QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void buildUi();
    void buildMenus();
    void connectSignals();
    void openPycFile();
    void saveMergedResult();
    bool openPycFileFromPath(const QString &filePath);
    void openSettings();
    void retryCurrentNodeWithAi();
    void refreshTree();
    void refreshSourceViews();
    void refreshInspectViews();
    void refreshStatus();
    void updateNodeDetails(QTreeWidgetItem *current, QTreeWidgetItem *previous = nullptr);
    void populateTreeItem(QTreeWidgetItem *parentItem, const class CodeObjectNode &node);
    QTreeWidgetItem *findTreeItemByNodeId(const QString &nodeId) const;

    /** 刷新 heroCard 中的 Provider 和 Model 下拉框 */
    void refreshProviderCombo();

    AppContext *m_context = nullptr;
    QAction *m_openAction = nullptr;
    QAction *m_saveMergedAction = nullptr;
    QAction *m_settingsAction = nullptr;
    QAction *m_retryAiAction = nullptr;
    QAction *m_exitAction = nullptr;
    QTreeWidget *m_treeWidget = nullptr;
    QTabWidget *m_sourceTabs = nullptr;
    QPlainTextEdit *m_mergedEdit = nullptr;
    QPlainTextEdit *m_nativeEdit = nullptr;
    QPlainTextEdit *m_aiEdit = nullptr;
    QTabWidget *m_inspectTabs = nullptr;
    QPlainTextEdit *m_disassemblyEdit = nullptr;
    QPlainTextEdit *m_metadataEdit = nullptr;
    QPlainTextEdit *m_promptEdit = nullptr;
    QPlainTextEdit *m_logEdit = nullptr;

    // heroCard — provider / model 选择
    QComboBox *m_providerCombo = nullptr;
    QComboBox *m_modelCombo    = nullptr;
};

#endif // MAIN_WINDOW_H
