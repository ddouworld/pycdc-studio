#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <QObject>

#include "src/ai/openai_compatible_client.h"
#include "src/ai/prompt_builder.h"
#include "src/model/project_session.h"
#include "src/service/decompiler_service.h"
#include "src/service/fallback_service.h"
#include "src/service/pyarmor_import_service.h"
#include "src/service/pycdc_process_runner.h"
#include "src/service/pycdas_process_runner.h"

class AppContext : public QObject
{
    Q_OBJECT

public:
    explicit AppContext(QObject *parent = nullptr);

    ProjectSession &session() { return m_session; }
    DecompilerService &decompilerService() { return m_decompilerService; }
    FallbackService &fallbackService() { return m_fallbackService; }
    PyarmorImportService &pyarmorImportService() { return m_pyarmorImportService; }
    PycdcProcessRunner &pycdcRunner() { return m_pycdcRunner; }
    PycdasProcessRunner &pycdasRunner() { return m_pycdasRunner; }
    OpenAiCompatibleClient &aiClient() { return m_aiClient; }

private:
    ProjectSession m_session;
    PromptBuilder m_promptBuilder;
    OpenAiCompatibleClient m_aiClient;
    PycdcProcessRunner m_pycdcRunner;
    PycdasProcessRunner m_pycdasRunner;
    DecompilerService m_decompilerService;
    FallbackService m_fallbackService;
    PyarmorImportService m_pyarmorImportService;
};

#endif // APP_CONTEXT_H
