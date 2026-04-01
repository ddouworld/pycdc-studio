#include "src/ai/ai_provider_config.h"

#include <QProcessEnvironment>
#include <QSettings>
#include <QStringList>

namespace {
QString firstNonEmpty(const QStringList &values)
{
    for (const QString &value : values) {
        if (!value.trimmed().isEmpty()) {
            return value;
        }
    }
    return QString();
}

QString readStoredOrEnv(QSettings &settings,
                        const QString &key,
                        const QStringList &envVars)
{
    const QString storedValue = settings.value(key).toString().trimmed();
    if (!storedValue.isEmpty()) {
        return storedValue;
    }

    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList envValues;
    for (const QString &envVar : envVars) {
        envValues.append(env.value(envVar));
    }
    return firstNonEmpty(envValues);
}

void writeOrRemove(QSettings &settings, const QString &key, const QString &value)
{
    if (value.trimmed().isEmpty()) {
        settings.remove(key);
    } else {
        settings.setValue(key, value);
    }
}

static const QString kDefaultSystemPrompt = QStringLiteral(
    "You reconstruct Python source code from bytecode metadata and disassembly. "
    "Return only Python source code.");
}

bool AiProviderConfig::isValid() const
{
    return !baseUrl.trimmed().isEmpty()
        && !apiKey.trimmed().isEmpty()
        && !model.trimmed().isEmpty();
}

// ── 向后兼容单 provider 接口 ────────────────────────────────────────────────

AiProviderConfig AiProviderConfig::load()
{
    QSettings settings;

    AiProviderConfig config;
    config.name = QStringLiteral("Default");
    config.baseUrl = readStoredOrEnv(settings,
                                     QStringLiteral("ai/baseUrl"),
                                     { QStringLiteral("PYCDC_STUDIO_AI_BASE_URL"),
                                       QStringLiteral("OPENAI_BASE_URL"),
                                       QStringLiteral("OPENAI_API_BASE") });
    config.apiKey = readStoredOrEnv(settings,
                                    QStringLiteral("ai/apiKey"),
                                    { QStringLiteral("PYCDC_STUDIO_AI_API_KEY"),
                                      QStringLiteral("OPENAI_API_KEY") });
    config.model = readStoredOrEnv(settings,
                                   QStringLiteral("ai/model"),
                                   { QStringLiteral("PYCDC_STUDIO_AI_MODEL"),
                                     QStringLiteral("OPENAI_MODEL") });
    config.systemPrompt = readStoredOrEnv(settings,
                                          QStringLiteral("ai/systemPrompt"),
                                          { QStringLiteral("PYCDC_STUDIO_AI_SYSTEM_PROMPT") });
    if (config.systemPrompt.isEmpty()) {
        config.systemPrompt = kDefaultSystemPrompt;
    }
    // 迁移：若已有 providers 数组则优先使用 active provider
    const QList<AiProviderConfig> all = loadAll();
    if (!all.isEmpty()) {
        const int idx = loadActiveIndex();
        const int safeIdx = (idx >= 0 && idx < all.size()) ? idx : 0;
        return all.at(safeIdx);
    }
    if (!config.model.isEmpty()) {
        config.models = QStringList{ config.model };
    }
    return config;
}

void AiProviderConfig::save() const
{
    // 将此 provider 作为 "Default" 单条存入 providers 列表
    QList<AiProviderConfig> all = loadAll();
    if (all.isEmpty()) {
        all.append(*this);
    } else {
        all[0] = *this;
    }
    saveAll(all, 0);
}

// ── 多 providers 接口 ────────────────────────────────────────────────────────

QList<AiProviderConfig> AiProviderConfig::loadAll()
{
    QSettings settings;
    const int count = settings.beginReadArray(QStringLiteral("providers"));
    QList<AiProviderConfig> result;
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        AiProviderConfig cfg;
        cfg.name        = settings.value(QStringLiteral("name")).toString();
        cfg.baseUrl     = settings.value(QStringLiteral("baseUrl")).toString().trimmed();
        cfg.apiKey      = settings.value(QStringLiteral("apiKey")).toString().trimmed();
        cfg.model       = settings.value(QStringLiteral("model")).toString().trimmed();
        cfg.models      = settings.value(QStringLiteral("models")).toStringList();
        cfg.systemPrompt = settings.value(QStringLiteral("systemPrompt")).toString();
        if (cfg.systemPrompt.isEmpty()) {
            cfg.systemPrompt = kDefaultSystemPrompt;
        }
        if (cfg.name.isEmpty()) {
            cfg.name = QStringLiteral("Provider %1").arg(i + 1);
        }
        result.append(cfg);
    }
    settings.endArray();
    return result;
}

void AiProviderConfig::saveAll(const QList<AiProviderConfig> &providers, int activeIndex)
{
    QSettings settings;
    settings.beginWriteArray(QStringLiteral("providers"), providers.size());
    for (int i = 0; i < providers.size(); ++i) {
        settings.setArrayIndex(i);
        const AiProviderConfig &cfg = providers.at(i);
        settings.setValue(QStringLiteral("name"),         cfg.name);
        settings.setValue(QStringLiteral("baseUrl"),      cfg.baseUrl);
        settings.setValue(QStringLiteral("apiKey"),       cfg.apiKey);
        settings.setValue(QStringLiteral("model"),        cfg.model);
        settings.setValue(QStringLiteral("models"),       cfg.models);
        settings.setValue(QStringLiteral("systemPrompt"), cfg.systemPrompt);
    }
    settings.endArray();
    settings.setValue(QStringLiteral("providers/activeIndex"), activeIndex);
    settings.sync();
}

int AiProviderConfig::loadActiveIndex()
{
    QSettings settings;
    return settings.value(QStringLiteral("providers/activeIndex"), 0).toInt();
}
