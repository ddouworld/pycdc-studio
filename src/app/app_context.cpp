#include "src/app/app_context.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>

namespace {
QString chooseProgramPath(const QString &envVarName,
                         const QStringList &bundledExecutableNames,
                         const QString &defaultProgram)
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString envValue = env.value(envVarName).trimmed();
    if (!envValue.isEmpty()) {
        return envValue;
    }

    const QString appDir = QCoreApplication::applicationDirPath();
    if (!appDir.isEmpty()) {
        for (const QString &bundledExecutableName : bundledExecutableNames) {
            const QString bundledPath = QDir(appDir).filePath(bundledExecutableName);
            const QFileInfo bundledInfo(bundledPath);
            if (bundledInfo.exists() && bundledInfo.isFile()) {
                return bundledInfo.absoluteFilePath();
            }
        }
    }

    return defaultProgram;
}
}

AppContext::AppContext(QObject *parent)
    : QObject(parent)
    , m_session(this)
    , m_aiClient(this)
    , m_pycdcRunner(this)
    , m_pycdasRunner(this)
    , m_decompilerService(m_session, m_pycdcRunner, m_pycdasRunner, this)
    , m_fallbackService(m_session, m_promptBuilder, m_aiClient, this)
    , m_pyarmorImportService(m_session, this)
{
    m_pycdcRunner.setProgram(chooseProgramPath(QStringLiteral("PYCDC_STUDIO_PYCDC"),
                                               { QStringLiteral("pycdc.exe"),
                                                 QStringLiteral("pycdc") },
                                               QStringLiteral("pycdc")));
    m_pycdasRunner.setProgram(chooseProgramPath(QStringLiteral("PYCDC_STUDIO_PYCDAS"),
                                                { QStringLiteral("pycdas.exe"),
                                                  QStringLiteral("pycdas") },
                                                QStringLiteral("pycdas")));
}
