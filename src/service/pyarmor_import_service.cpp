#include "src/service/pyarmor_import_service.h"

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QUuid>

#include "src/service/pycdas_tree_parser.h"

namespace {
QString readTextFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

void appendProcessOutput(ProjectSession &session, const QString &prefix, const QString &text)
{
    const QStringList lines = text.split('\n');
    for (const QString &line : lines) {
        const QString trimmed = line.trimmed();
        if (!trimmed.isEmpty()) {
            session.appendLogLine(QStringLiteral("%1 %2").arg(prefix, trimmed));
        }
    }
}

void applyStatusRecursive(QList<CodeObjectNode> &nodes, CodeObjectNode::Status status)
{
    for (CodeObjectNode &node : nodes) {
        node.status = status;
        if (!node.children.isEmpty()) {
            applyStatusRecursive(node.children, status);
        }
    }
}

void prefixNodeIdsRecursive(CodeObjectNode &node, const QString &prefix)
{
    node.id = prefix + QStringLiteral("|") + node.id;
    for (CodeObjectNode &child : node.children) {
        prefixNodeIdsRecursive(child, prefix);
    }
}

void prefixNodeIds(QList<CodeObjectNode> &nodes, const QString &filePath)
{
    const QString prefix = QStringLiteral("file:%1")
        .arg(QString::number(qHash(filePath), 16));
    for (CodeObjectNode &node : nodes) {
        prefixNodeIdsRecursive(node, prefix);
    }
}

void applySourceFileRecursive(QList<CodeObjectNode> &nodes, const QString &filePath)
{
    for (CodeObjectNode &node : nodes) {
        node.sourceFile = filePath;
        if (!node.children.isEmpty()) {
            applySourceFileRecursive(node.children, filePath);
        }
    }
}

QStringList candidateRoots()
{
    QStringList roots;
    roots.append(QDir::currentPath());

    const QString appDir = QCoreApplication::applicationDirPath();
    if (!appDir.isEmpty()) {
        roots.append(appDir);
        roots.append(QDir(appDir).filePath(QStringLiteral("..")));
        roots.append(QDir(appDir).filePath(QStringLiteral("../..")));
    }

    roots.removeDuplicates();
    return roots;
}

QString findExistingPath(const QStringList &candidates)
{
    for (const QString &candidate : candidates) {
        const QFileInfo info(QDir::cleanPath(candidate));
        if (info.exists()) {
            return info.absoluteFilePath();
        }
    }
    return QString();
}
}

PyarmorImportService::PyarmorImportService(ProjectSession &session, QObject *parent)
    : QObject(parent)
    , m_session(session)
{
}

bool PyarmorImportService::importProject(const QString &projectDirectory)
{
    const QFileInfo projectInfo(projectDirectory);
    if (!projectInfo.exists() || !projectInfo.isDir()) {
        m_session.setStatusMessage(tr("The selected Pyarmor project directory is invalid."));
        return false;
    }

    const QString pythonProgram = resolvePythonProgram();
    const QString shotScript = resolveShotScript();
    if (pythonProgram.isEmpty() || shotScript.isEmpty()) {
        m_session.setStatusMessage(tr("Pyarmor oneshot tool is not available."));
        m_session.appendLogLine(tr("[pyarmor] missing Python or shot.py; bundle pyarmor-oneshot with the app, or configure PYCDC_STUDIO_PYARMOR_PYTHON / PYCDC_STUDIO_PYARMOR_SHOT"));
        return false;
    }

    const QString outputDirectory = createOutputDirectory(projectDirectory);
    if (outputDirectory.isEmpty()) {
        m_session.setStatusMessage(tr("Failed to prepare a Pyarmor output directory."));
        return false;
    }

    m_session.clear();
    m_session.setOpenedFilePath(projectInfo.absoluteFilePath());
    m_session.setStatusMessage(tr("Importing Pyarmor project..."));
    m_session.appendLogLine(tr("[pyarmor] project = %1").arg(projectInfo.absoluteFilePath()));
    m_session.appendLogLine(tr("[pyarmor] python = %1").arg(pythonProgram));
    m_session.appendLogLine(tr("[pyarmor] shot = %1").arg(shotScript));
    m_session.appendLogLine(tr("[pyarmor] output = %1").arg(outputDirectory));

    QString stdoutText;
    QString stderrText;
    if (!runShotProcess(projectInfo.absoluteFilePath(), outputDirectory, stdoutText, stderrText)) {
        appendProcessOutput(m_session, QStringLiteral("[pyarmor]"), stdoutText);
        appendProcessOutput(m_session, QStringLiteral("[pyarmor]"), stderrText);
        m_session.setStatusMessage(tr("Pyarmor import failed."));
        return false;
    }

    appendProcessOutput(m_session, QStringLiteral("[pyarmor]"), stdoutText);
    appendProcessOutput(m_session, QStringLiteral("[pyarmor]"), stderrText);

    if (!loadGeneratedOutputs(projectInfo.absoluteFilePath(), outputDirectory)) {
        m_session.setStatusMessage(tr("Pyarmor import produced no readable output."));
        return false;
    }

    return true;
}

QString PyarmorImportService::resolvePythonProgram() const
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString envValue = env.value(QStringLiteral("PYCDC_STUDIO_PYARMOR_PYTHON")).trimmed();
    if (!envValue.isEmpty()) {
        return envValue;
    }

#ifdef Q_OS_WIN
    const QStringList candidates{ QStringLiteral("python.exe"), QStringLiteral("python") };
#else
    const QStringList candidates{ QStringLiteral("python3"), QStringLiteral("python") };
#endif
    for (const QString &candidate : candidates) {
        const QString executable = QStandardPaths::findExecutable(candidate);
        if (!executable.isEmpty()) {
            return executable;
        }
    }

#ifdef Q_OS_WIN
    return QStringLiteral("python");
#else
    return QStringLiteral("python3");
#endif
}

QString PyarmorImportService::resolveShotScript() const
{
    QStringList candidates;
    for (const QString &root : candidateRoots()) {
        candidates.append(QDir(root).filePath(QStringLiteral("pyarmor-oneshot/shot.py")));
        candidates.append(QDir(root).filePath(QStringLiteral("oneshot/shot.py")));
        candidates.append(QDir(root).filePath(QStringLiteral("external/Pyarmor-Static-Unpack-1shot/oneshot/shot.py")));
    }
    const QString bundledPath = findExistingPath(candidates);
    if (!bundledPath.isEmpty()) {
        return bundledPath;
    }

    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString envValue = env.value(QStringLiteral("PYCDC_STUDIO_PYARMOR_SHOT")).trimmed();
    if (!envValue.isEmpty() && QFileInfo::exists(envValue)) {
        return QFileInfo(envValue).absoluteFilePath();
    }

    return QString();
}

QString PyarmorImportService::resolveExtraPythonPath() const
{
    QStringList candidates;
    for (const QString &root : candidateRoots()) {
        candidates.append(QDir(root).filePath(QStringLiteral(".oneshot-packages")));
        candidates.append(QDir(root).filePath(QStringLiteral("pyarmor-oneshot/python-packages")));
    }
    const QString bundledPath = findExistingPath(candidates);
    if (!bundledPath.isEmpty()) {
        return bundledPath;
    }

    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString envValue = env.value(QStringLiteral("PYCDC_STUDIO_PYARMOR_PYTHONPATH")).trimmed();
    if (!envValue.isEmpty() && QFileInfo::exists(envValue)) {
        return QFileInfo(envValue).absoluteFilePath();
    }

    return QString();
}

QString PyarmorImportService::createOutputDirectory(const QString &projectDirectory) const
{
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString baseDir = env.value(QStringLiteral("PYCDC_STUDIO_PYARMOR_OUTPUT_ROOT")).trimmed();
    if (baseDir.isEmpty()) {
        baseDir = QDir(QDir::tempPath()).filePath(QStringLiteral("pycdc-studio-pyarmor"));
    }

    QDir dir;
    if (!dir.mkpath(baseDir)) {
        return QString();
    }

    const QString runName = QStringLiteral("%1-%2")
        .arg(QFileInfo(projectDirectory).fileName(),
             QUuid::createUuid().toString(QUuid::WithoutBraces));
    const QString outputDirectory = QDir(baseDir).filePath(runName);
    if (!dir.mkpath(outputDirectory)) {
        return QString();
    }
    return QDir(outputDirectory).absolutePath();
}

bool PyarmorImportService::runShotProcess(const QString &projectDirectory,
                                          const QString &outputDirectory,
                                          QString &stdoutText,
                                          QString &stderrText) const
{
    const QString pythonProgram = resolvePythonProgram();
    const QString shotScript = resolveShotScript();

    QProcess process;
    process.setProgram(pythonProgram);
    process.setArguments({ shotScript,
                           projectDirectory,
                           QStringLiteral("-o"),
                           outputDirectory,
                           QStringLiteral("--no-banner") });

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString extraPythonPath = resolveExtraPythonPath();
    if (!extraPythonPath.isEmpty()) {
        const QString existing = env.value(QStringLiteral("PYTHONPATH"));
        const QString separator = QString(QDir::listSeparator());
        env.insert(QStringLiteral("PYTHONPATH"),
                   existing.isEmpty() ? extraPythonPath : extraPythonPath + separator + existing);
    }
    process.setProcessEnvironment(env);
    process.setWorkingDirectory(QFileInfo(shotScript).absolutePath());
    process.start();

    if (!process.waitForStarted(5000)) {
        stderrText = tr("Failed to start Pyarmor oneshot from '%1'.").arg(pythonProgram);
        return false;
    }

    process.waitForFinished(300000);
    stdoutText = QString::fromUtf8(process.readAllStandardOutput());
    stderrText = QString::fromUtf8(process.readAllStandardError());

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (stderrText.trimmed().isEmpty()) {
            stderrText = tr("Pyarmor oneshot exited with code %1.").arg(process.exitCode());
        }
        return false;
    }

    return true;
}

bool PyarmorImportService::loadGeneratedOutputs(const QString &projectDirectory,
                                                const QString &outputDirectory) const
{
    QList<CodeObjectNode> aggregatedRoots;
    QString firstNativeText;
    QString firstDisassemblyText;
    int importedCount = 0;

    QDirIterator it(outputDirectory,
                    QStringList{ QStringLiteral("*.1shot.das") },
                    QDir::Files,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString dasPath = it.next();
        const QString cdcPath = dasPath.left(dasPath.size() - 4) + QStringLiteral(".cdc.py");
        const QString relativeDasPath = QDir(outputDirectory).relativeFilePath(dasPath);
        const QString sourceRelativePath = relativeDasPath.left(relativeDasPath.size() - QStringLiteral(".1shot.das").size());
        const QString sourceFilePath = QDir(projectDirectory).filePath(sourceRelativePath);

        const QString disassembly = readTextFile(dasPath);
        const QString nativeSource = readTextFile(cdcPath);
        const bool nativeSuccess = !nativeSource.trimmed().isEmpty();
        const QString nativeError = nativeSuccess
            ? QString()
            : tr("No .1shot.cdc.py output was generated for %1.").arg(sourceRelativePath);

        QList<CodeObjectNode> roots = buildInitialTree(sourceFilePath,
                                                       disassembly,
                                                       nativeSource,
                                                       nativeError,
                                                       nativeSuccess);
        applySourceFileRecursive(roots, sourceFilePath);
        prefixNodeIds(roots, sourceFilePath);
        aggregatedRoots.append(roots);

        if (firstNativeText.isEmpty()) {
            firstNativeText = nativeSource;
        }
        if (firstDisassemblyText.isEmpty()) {
            firstDisassemblyText = disassembly;
        }

        ++importedCount;
        m_session.appendLogLine(tr("[pyarmor] imported %1").arg(sourceRelativePath));
    }

    if (aggregatedRoots.isEmpty()) {
        m_session.appendLogLine(tr("[pyarmor] no .1shot.das files found in %1").arg(outputDirectory));
        return false;
    }

    m_session.setDisassemblyText(firstDisassemblyText);
    m_session.setNativeSource(firstNativeText);
    m_session.setMergedSource(firstNativeText);
    m_session.setCodeObjectTree(aggregatedRoots);
    m_session.setStatusMessage(tr("Imported %1 Pyarmor file(s) into the workspace.").arg(importedCount));
    return true;
}

QList<CodeObjectNode> PyarmorImportService::buildInitialTree(const QString &sourceFilePath,
                                                             const QString &disassembly,
                                                             const QString &nativeSource,
                                                             const QString &nativeError,
                                                             bool nativeSuccess) const
{
    PycdasTreeParser parser;
    QList<CodeObjectNode> nodes = parser.parse(disassembly, sourceFilePath);
    const CodeObjectNode::Status initialStatus = nativeSuccess
        ? CodeObjectNode::Status::NativeOk
        : CodeObjectNode::Status::NativeFailed;

    if (nodes.isEmpty()) {
        QFileInfo info(sourceFilePath);

        CodeObjectNode moduleNode;
        moduleNode.id = QStringLiteral("module:%1").arg(info.completeBaseName());
        moduleNode.qualifiedName = QStringLiteral("<module>");
        moduleNode.displayName = info.fileName();
        moduleNode.objectType = QStringLiteral("module");
        moduleNode.sourceFile = sourceFilePath;
        moduleNode.disassembly = disassembly;
        moduleNode.nativeSource = nativeSource;
        moduleNode.mergedSource = nativeSource;
        moduleNode.nativeError = nativeError;
        moduleNode.status = initialStatus;
        return { moduleNode };
    }

    applyStatusRecursive(nodes, initialStatus);

    CodeObjectNode &root = nodes.first();
    root.sourceFile = sourceFilePath;
    root.disassembly = disassembly;
    root.nativeSource = nativeSource;
    root.mergedSource = nativeSource;
    root.nativeError = nativeError;
    root.status = initialStatus;
    return nodes;
}
