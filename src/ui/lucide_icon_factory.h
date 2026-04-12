#ifndef LUCIDE_ICON_FACTORY_H
#define LUCIDE_ICON_FACTORY_H

#include <QColor>
#include <QIcon>
#include <QPixmap>

class LucideIconFactory
{
public:
    enum class IconType {
        App,
        Open,
        Settings,
        Sparkles,
        Plus,
        ChevronUp,
        ChevronDown,
        Exit,
        Merged,
        Native,
        Ai,
        Disassembly,
        Metadata,
        Prompt,
        Log,
        ModuleNode,
        ClassNode,
        FunctionNode,
        LambdaNode,
        ComprehensionNode,
        StatusOk,
        StatusAi,
        StatusWarning,
    };

    static QIcon icon(IconType type,
                      const QColor &color = QColor("#1b3b5d"),
                      int baseSize = 20);
    static QIcon appIcon();
    static QPixmap pixmap(IconType type,
                          int size,
                          const QColor &color = QColor("#1b3b5d"));

private:
    static void drawIcon(class QPainter &painter,
                         IconType type,
                         const QRectF &rect,
                         const QColor &color,
                         bool appMode = false);
};

#endif // LUCIDE_ICON_FACTORY_H
