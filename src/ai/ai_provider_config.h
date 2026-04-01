#ifndef AI_PROVIDER_CONFIG_H
#define AI_PROVIDER_CONFIG_H

#include <QString>
#include <QList>
#include <QStringList>

struct AiProviderConfig
{
    QString name;           // provider 显示名称
    QString baseUrl;
    QString apiKey;
    QString model;          // 当前选用的模型
    QStringList models;     // 该 provider 下已配置的模型列表
    QString systemPrompt;

    bool isValid() const;

    // 单 provider 兼容接口（向后兼容）
    static AiProviderConfig load();
    void save() const;

    // 多 providers 接口
    static QList<AiProviderConfig> loadAll();
    static void saveAll(const QList<AiProviderConfig> &providers, int activeIndex);
    static int loadActiveIndex();
};

#endif // AI_PROVIDER_CONFIG_H
