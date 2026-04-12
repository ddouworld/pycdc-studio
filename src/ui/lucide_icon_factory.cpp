#include "src/ui/lucide_icon_factory.h"

#include <cmath>

#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

namespace {
constexpr qreal kPi = 3.14159265358979323846;

void drawFileOutline(QPainter &painter, const QRectF &rect)
{
    const qreal left = rect.left() + rect.width() * 0.23;
    const qreal top = rect.top() + rect.height() * 0.14;
    const qreal right = rect.right() - rect.width() * 0.20;
    const qreal bottom = rect.bottom() - rect.height() * 0.16;
    const qreal fold = rect.width() * 0.18;

    QPainterPath path;
    path.moveTo(left, top);
    path.lineTo(right - fold, top);
    path.lineTo(right, top + fold);
    path.lineTo(right, bottom);
    path.lineTo(left, bottom);
    path.closeSubpath();
    painter.drawPath(path);
    painter.drawLine(QPointF(right - fold, top), QPointF(right - fold, top + fold));
    painter.drawLine(QPointF(right - fold, top + fold), QPointF(right, top + fold));
}

void drawSparkle(QPainter &painter, const QPointF &center, qreal radius)
{
    painter.drawLine(QPointF(center.x(), center.y() - radius), QPointF(center.x(), center.y() + radius));
    painter.drawLine(QPointF(center.x() - radius, center.y()), QPointF(center.x() + radius, center.y()));
    painter.drawLine(QPointF(center.x() - radius * 0.65, center.y() - radius * 0.65),
                     QPointF(center.x() + radius * 0.65, center.y() + radius * 0.65));
    painter.drawLine(QPointF(center.x() - radius * 0.65, center.y() + radius * 0.65),
                     QPointF(center.x() + radius * 0.65, center.y() - radius * 0.65));
}

void drawCurlyBrace(QPainter &painter, const QRectF &rect, bool left)
{
    const qreal x = left ? rect.left() + rect.width() * 0.30 : rect.right() - rect.width() * 0.30;
    const qreal dir = left ? -1.0 : 1.0;
    const qreal top = rect.top() + rect.height() * 0.18;
    const qreal mid = rect.center().y();
    const qreal bottom = rect.bottom() - rect.height() * 0.18;
    const qreal bend = rect.width() * 0.12;

    QPainterPath path;
    path.moveTo(x, top);
    path.cubicTo(x + dir * bend, top,
                 x + dir * bend, mid - rect.height() * 0.14,
                 x, mid - rect.height() * 0.06);
    path.cubicTo(x - dir * bend * 0.45, mid - rect.height() * 0.02,
                 x - dir * bend * 0.45, mid + rect.height() * 0.02,
                 x, mid + rect.height() * 0.06);
    path.cubicTo(x + dir * bend, mid + rect.height() * 0.14,
                 x + dir * bend, bottom,
                 x, bottom);
    painter.drawPath(path);
}

void drawCube(QPainter &painter, const QRectF &rect)
{
    const QRectF front(rect.left() + rect.width() * 0.22,
                       rect.top() + rect.height() * 0.30,
                       rect.width() * 0.42,
                       rect.height() * 0.42);
    const QPointF offset(rect.width() * 0.16, -rect.height() * 0.12);
    const QRectF back = front.translated(offset);
    painter.drawRect(front);
    painter.drawRect(back);
    painter.drawLine(front.topLeft(), back.topLeft());
    painter.drawLine(front.topRight(), back.topRight());
    painter.drawLine(front.bottomLeft(), back.bottomLeft());
    painter.drawLine(front.bottomRight(), back.bottomRight());
}
}

QIcon LucideIconFactory::icon(IconType type, const QColor &color, int baseSize)
{
    QIcon icon;
    for (const int size : { baseSize, 24, 32, 40, 48, 64 }) {
        icon.addPixmap(pixmap(type, size, color));
    }
    return icon;
}

QIcon LucideIconFactory::appIcon()
{
    return icon(IconType::App, QColor("#ffffff"), 32);
}

QPixmap LucideIconFactory::pixmap(IconType type, int size, const QColor &color)
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    drawIcon(painter, type, QRectF(0, 0, size, size), color, type == IconType::App);
    return pixmap;
}

void LucideIconFactory::drawIcon(QPainter &painter,
                                 IconType type,
                                 const QRectF &rect,
                                 const QColor &color,
                                 bool appMode)
{
    if (appMode) {
        QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
        gradient.setColorAt(0.0, QColor("#2f7cff"));
        gradient.setColorAt(1.0, QColor("#153a74"));
        painter.setPen(Qt::NoPen);
        painter.setBrush(gradient);
        painter.drawRoundedRect(rect.adjusted(rect.width() * 0.06,
                                              rect.height() * 0.06,
                                              -rect.width() * 0.06,
                                              -rect.height() * 0.06),
                                rect.width() * 0.22,
                                rect.height() * 0.22);
    }

    QPen pen(color, qMax<qreal>(1.8, rect.width() * 0.08), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    switch (type) {
    case IconType::App:
        painter.drawRoundedRect(rect.adjusted(rect.width() * 0.24,
                                              rect.height() * 0.24,
                                              -rect.width() * 0.24,
                                              -rect.height() * 0.24),
                                rect.width() * 0.08,
                                rect.height() * 0.08);
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.34, rect.center().y()),
                         QPointF(rect.right() - rect.width() * 0.34, rect.center().y()));
        painter.drawLine(QPointF(rect.center().x(), rect.top() + rect.height() * 0.34),
                         QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.34));
        break;
    case IconType::Open: {
        QPainterPath path;
        path.moveTo(rect.left() + rect.width() * 0.14, rect.top() + rect.height() * 0.38);
        path.lineTo(rect.left() + rect.width() * 0.36, rect.top() + rect.height() * 0.38);
        path.lineTo(rect.left() + rect.width() * 0.45, rect.top() + rect.height() * 0.26);
        path.lineTo(rect.right() - rect.width() * 0.14, rect.top() + rect.height() * 0.26);
        path.lineTo(rect.right() - rect.width() * 0.20, rect.bottom() - rect.height() * 0.24);
        path.lineTo(rect.left() + rect.width() * 0.20, rect.bottom() - rect.height() * 0.24);
        path.closeSubpath();
        painter.drawPath(path);
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.28, rect.top() + rect.height() * 0.52),
                         QPointF(rect.center().x(), rect.top() + rect.height() * 0.52));
        painter.drawLine(QPointF(rect.center().x(), rect.top() + rect.height() * 0.52),
                         QPointF(rect.center().x(), rect.top() + rect.height() * 0.38));
        painter.drawLine(QPointF(rect.center().x(), rect.top() + rect.height() * 0.52),
                         QPointF(rect.center().x() + rect.width() * 0.12, rect.top() + rect.height() * 0.40));
        break;
    }
    case IconType::Settings: {
        const QPointF c = rect.center();
        const qreal outer = rect.width() * 0.30;
        const qreal inner = rect.width() * 0.12;
        for (int i = 0; i < 8; ++i) {
            const qreal angle = i * kPi / 4.0;
            const QPointF p1(c.x() + std::cos(angle) * (outer - rect.width() * 0.07),
                             c.y() + std::sin(angle) * (outer - rect.width() * 0.07));
            const QPointF p2(c.x() + std::cos(angle) * outer,
                             c.y() + std::sin(angle) * outer);
            painter.drawLine(p1, p2);
        }
        painter.drawEllipse(c, outer - rect.width() * 0.08, outer - rect.width() * 0.08);
        painter.drawEllipse(c, inner, inner);
        break;
    }
    case IconType::Sparkles:
        drawSparkle(painter, QPointF(rect.left() + rect.width() * 0.38, rect.top() + rect.height() * 0.42), rect.width() * 0.16);
        drawSparkle(painter, QPointF(rect.left() + rect.width() * 0.68, rect.top() + rect.height() * 0.66), rect.width() * 0.10);
        break;
    case IconType::Plus:
        painter.drawLine(QPointF(rect.center().x(), rect.top() + rect.height() * 0.24),
                         QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.24));
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.24, rect.center().y()),
                         QPointF(rect.right() - rect.width() * 0.24, rect.center().y()));
        break;
    case IconType::ChevronUp:
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.28, rect.bottom() - rect.height() * 0.36),
                         QPointF(rect.center().x(), rect.top() + rect.height() * 0.30));
        painter.drawLine(QPointF(rect.center().x(), rect.top() + rect.height() * 0.30),
                         QPointF(rect.right() - rect.width() * 0.28, rect.bottom() - rect.height() * 0.36));
        break;
    case IconType::ChevronDown:
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.28, rect.top() + rect.height() * 0.34),
                         QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.28));
        painter.drawLine(QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.28),
                         QPointF(rect.right() - rect.width() * 0.28, rect.top() + rect.height() * 0.34));
        break;
    case IconType::Exit:
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.25, rect.top() + rect.height() * 0.25),
                         QPointF(rect.right() - rect.width() * 0.25, rect.bottom() - rect.height() * 0.25));
        painter.drawLine(QPointF(rect.right() - rect.width() * 0.25, rect.top() + rect.height() * 0.25),
                         QPointF(rect.left() + rect.width() * 0.25, rect.bottom() - rect.height() * 0.25));
        break;
    case IconType::Merged:
        painter.drawRoundedRect(rect.adjusted(rect.width() * 0.18, rect.height() * 0.24,
                                              -rect.width() * 0.18, -rect.height() * 0.18),
                                rect.width() * 0.08, rect.height() * 0.08);
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.30, rect.top() + rect.height() * 0.46),
                         QPointF(rect.right() - rect.width() * 0.30, rect.top() + rect.height() * 0.46));
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.30, rect.top() + rect.height() * 0.62),
                         QPointF(rect.center().x(), rect.top() + rect.height() * 0.62));
        break;
    case IconType::Native:
        drawFileOutline(painter, rect);
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.34, rect.center().y()),
                         QPointF(rect.center().x() - rect.width() * 0.06, rect.center().y()));
        painter.drawLine(QPointF(rect.center().x() + rect.width() * 0.06, rect.center().y()),
                         QPointF(rect.right() - rect.width() * 0.34, rect.center().y()));
        painter.drawLine(QPointF(rect.center().x() - rect.width() * 0.06, rect.center().y()),
                         QPointF(rect.center().x(), rect.center().y() - rect.height() * 0.08));
        painter.drawLine(QPointF(rect.center().x() + rect.width() * 0.06, rect.center().y()),
                         QPointF(rect.center().x(), rect.center().y() + rect.height() * 0.08));
        break;
    case IconType::Ai:
        drawSparkle(painter, rect.center(), rect.width() * 0.18);
        break;
    case IconType::Disassembly:
        drawCurlyBrace(painter, rect, true);
        drawCurlyBrace(painter, rect, false);
        painter.drawLine(QPointF(rect.center().x() - rect.width() * 0.10, rect.top() + rect.height() * 0.34),
                         QPointF(rect.center().x() + rect.width() * 0.10, rect.top() + rect.height() * 0.34));
        painter.drawLine(QPointF(rect.center().x() - rect.width() * 0.10, rect.bottom() - rect.height() * 0.34),
                         QPointF(rect.center().x() + rect.width() * 0.10, rect.bottom() - rect.height() * 0.34));
        break;
    case IconType::Metadata:
        painter.drawEllipse(rect.adjusted(rect.width() * 0.20, rect.height() * 0.20,
                                          -rect.width() * 0.20, -rect.height() * 0.20));
        painter.drawLine(QPointF(rect.center().x(), rect.top() + rect.height() * 0.42),
                         QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.34));
        painter.drawPoint(QPointF(rect.center().x(), rect.top() + rect.height() * 0.30));
        break;
    case IconType::Prompt: {
        QPainterPath bubble;
        bubble.moveTo(rect.left() + rect.width() * 0.20, rect.top() + rect.height() * 0.28);
        bubble.lineTo(rect.right() - rect.width() * 0.20, rect.top() + rect.height() * 0.28);
        bubble.lineTo(rect.right() - rect.width() * 0.20, rect.bottom() - rect.height() * 0.28);
        bubble.lineTo(rect.center().x() + rect.width() * 0.08, rect.bottom() - rect.height() * 0.28);
        bubble.lineTo(rect.center().x() - rect.width() * 0.04, rect.bottom() - rect.height() * 0.14);
        bubble.lineTo(rect.center().x() - rect.width() * 0.02, rect.bottom() - rect.height() * 0.28);
        bubble.lineTo(rect.left() + rect.width() * 0.20, rect.bottom() - rect.height() * 0.28);
        bubble.closeSubpath();
        painter.drawPath(bubble);
        break;
    }
    case IconType::Log:
        painter.drawRoundedRect(rect.adjusted(rect.width() * 0.22, rect.height() * 0.16,
                                              -rect.width() * 0.22, -rect.height() * 0.16),
                                rect.width() * 0.08, rect.height() * 0.08);
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.34, rect.top() + rect.height() * 0.40),
                         QPointF(rect.right() - rect.width() * 0.34, rect.top() + rect.height() * 0.40));
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.34, rect.top() + rect.height() * 0.56),
                         QPointF(rect.right() - rect.width() * 0.40, rect.top() + rect.height() * 0.56));
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.34, rect.top() + rect.height() * 0.72),
                         QPointF(rect.right() - rect.width() * 0.46, rect.top() + rect.height() * 0.72));
        break;
    case IconType::ModuleNode:
        drawFileOutline(painter, rect);
        break;
    case IconType::ClassNode:
        drawCube(painter, rect);
        break;
    case IconType::FunctionNode:
        drawCurlyBrace(painter, rect, true);
        drawCurlyBrace(painter, rect, false);
        break;
    case IconType::LambdaNode:
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.26, rect.top() + rect.height() * 0.28),
                         QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.28));
        painter.drawLine(QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.28),
                         QPointF(rect.right() - rect.width() * 0.24, rect.top() + rect.height() * 0.28));
        break;
    case IconType::ComprehensionNode:
        painter.drawEllipse(QPointF(rect.left() + rect.width() * 0.34, rect.center().y()), rect.width() * 0.08, rect.width() * 0.08);
        painter.drawEllipse(QPointF(rect.center().x(), rect.center().y()), rect.width() * 0.08, rect.width() * 0.08);
        painter.drawEllipse(QPointF(rect.right() - rect.width() * 0.34, rect.center().y()), rect.width() * 0.08, rect.width() * 0.08);
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.42, rect.center().y()), QPointF(rect.center().x() - rect.width() * 0.08, rect.center().y()));
        painter.drawLine(QPointF(rect.center().x() + rect.width() * 0.08, rect.center().y()), QPointF(rect.right() - rect.width() * 0.42, rect.center().y()));
        break;
    case IconType::StatusOk:
        painter.drawEllipse(rect.adjusted(rect.width() * 0.20, rect.height() * 0.20,
                                          -rect.width() * 0.20, -rect.height() * 0.20));
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.34, rect.center().y()),
                         QPointF(rect.center().x() - rect.width() * 0.02, rect.bottom() - rect.height() * 0.32));
        painter.drawLine(QPointF(rect.center().x() - rect.width() * 0.02, rect.bottom() - rect.height() * 0.32),
                         QPointF(rect.right() - rect.width() * 0.30, rect.top() + rect.height() * 0.32));
        break;
    case IconType::StatusAi:
        drawSparkle(painter, rect.center(), rect.width() * 0.18);
        break;
    case IconType::StatusWarning:
        painter.drawLine(QPointF(rect.center().x(), rect.top() + rect.height() * 0.24),
                         QPointF(rect.left() + rect.width() * 0.24, rect.bottom() - rect.height() * 0.20));
        painter.drawLine(QPointF(rect.left() + rect.width() * 0.24, rect.bottom() - rect.height() * 0.20),
                         QPointF(rect.right() - rect.width() * 0.24, rect.bottom() - rect.height() * 0.20));
        painter.drawLine(QPointF(rect.right() - rect.width() * 0.24, rect.bottom() - rect.height() * 0.20),
                         QPointF(rect.center().x(), rect.top() + rect.height() * 0.24));
        painter.drawLine(QPointF(rect.center().x(), rect.top() + rect.height() * 0.42),
                         QPointF(rect.center().x(), rect.top() + rect.height() * 0.62));
        painter.drawPoint(QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.30));
        break;
    }
}
