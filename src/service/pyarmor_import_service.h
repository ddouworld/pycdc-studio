#ifndef PYARMOR_IMPORT_SERVICE_H
#define PYARMOR_IMPORT_SERVICE_H

#include <QObject>

#include "src/model/project_session.h"

class PyarmorImportService : public QObject
{
    Q_OBJECT

public:
    explicit PyarmorImportService(ProjectSession &session, QObject *parent = nullptr);

    bool importProject(const QString &projectDirectory);

private:
    QString resolvePythonProgram() const;
    QString resolveShotScript() const;
    QString resolveExtraPythonPath() const;
    QString createOutputDirectory(const QString &projectDirectory) const;

    bool runShotProcess(const QString &projectDirectory,
                        const QString &outputDirectory,
                        QString &stdoutText,
                        QString &stderrText) const;
    bool loadGeneratedOutputs(const QString &projectDirectory,
                              const QString &outputDirectory) const;
    QList<CodeObjectNode> buildInitialTree(const QString &sourceFilePath,
                                           const QString &disassembly,
                                           const QString &nativeSource,
                                           const QString &nativeError,
                                           bool nativeSuccess) const;

    ProjectSession &m_session;
};

#endif // PYARMOR_IMPORT_SERVICE_H
