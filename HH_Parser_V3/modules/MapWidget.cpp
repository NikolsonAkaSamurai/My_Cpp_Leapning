// MapWidget.cpp
#include "MapWidget.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QSet>

MapWidget::MapWidget(QWidget* parent)
    : QGraphicsView(parent)
    , m_scene(new QGraphicsScene(this))
    , m_isPanning(false)
    , m_zoom(1.0)
    , m_highlightedMarker(nullptr)
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(Qt::black);
    setFrameStyle(QFrame::NoFrame);

    // Настройка GeoCalculator с параметрами для Питера
    m_calc.setReferencePoint({400, 400}, 59.95, 30.31);
    m_calc.setScale(0.001);

    loadContours();

    // Тестовый красный маркер (центр сцены)
    QGraphicsEllipseItem* testMarker = new QGraphicsEllipseItem(-5, -5, 10, 10);
    testMarker->setBrush(Qt::red);
    testMarker->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(testMarker);
}

MapWidget::~MapWidget() {
    clearContours();
    clearVacancyMarkers();
}

void MapWidget::loadContours() {
    clearContours();

    auto contourNames = IOContours::getAvailableContours();
    if (contourNames.empty()) {
        QMessageBox::warning(this, "Ошибка",
            "Нет контуров в папке data/contours/\n"
            "Сначала нарисуйте контуры в редакторе");
        return;
    }

    for (size_t i = 0; i < contourNames.size(); ++i) {
        Contour c = IOContours::readContour(contourNames[i]);
        addContour(c, QString::fromStdString(contourNames[i]), getContourColor(i));
    }

    centerOnContours();
}

void MapWidget::addContour(const Contour& c, const QString& name, const QColor& color) {
    if (c.getPointCount() < 2) return;

    QPainterPath path;
    bool first = true;

    for (size_t i = 0; i < c.getPointCount(); ++i) {
        QPointF p = geoToScreen(c.getPoint(i));

        if (first) {
            path.moveTo(p);
            first = false;
        } else {
            path.lineTo(p);
        }
    }

    // Замыкаем контур
    if (c.getPointCount() > 2) {
        QPointF first = geoToScreen(c.getPoint(0));
        path.lineTo(first);
    }

    QGraphicsPathItem* pathItem = new QGraphicsPathItem(path);
    pathItem->setPen(QPen(color, 2));
    pathItem->setBrush(Qt::NoBrush);
    m_scene->addItem(pathItem);

    QGraphicsTextItem* label = nullptr;
    if (c.getPointCount() > 0) {
        double sumX = 0, sumY = 0;
        for (size_t i = 0; i < c.getPointCount(); ++i) {
            QPointF p = geoToScreen(c.getPoint(i));
            sumX += p.x();
            sumY += p.y();
        }
        QPointF center(sumX / c.getPointCount(), sumY / c.getPointCount());

        label = new QGraphicsTextItem(name);
        label->setDefaultTextColor(color);
        label->setPos(center.x() - 30, center.y() - 10);
        m_scene->addItem(label);
    }

    m_contours.append({pathItem, label, name, color});
}

void MapWidget::clearContours() {
    for (auto& item : m_contours) {
        if (item.path) {
            m_scene->removeItem(item.path);
            delete item.path;
        }
        if (item.label) {
            m_scene->removeItem(item.label);
            delete item.label;
        }
    }
    m_contours.clear();
}

QPointF MapWidget::geoToScreen(const Point& geo) const {
    Point pixel = m_calc.geoToPixel(geo);
    return QPointF(pixel.getX(), pixel.getY());
}

QColor MapWidget::getContourColor(int index) {
    QColor colors[] = {
        QColor(0, 255, 0),    // зеленый
        QColor(255, 255, 0),  // желтый
        QColor(255, 165, 0),  // оранжевый
        QColor(135, 206, 235), // голубой
        QColor(255, 192, 203), // розовый
        QColor(50, 205, 50),  // лаймовый
        QColor(255, 215, 0),  // золотой
        QColor(238, 130, 238) // фиолетовый
    };
    return colors[index % 8];
}

void MapWidget::centerOnContours() {
    if (m_contours.isEmpty()) return;

    QRectF bounds;
    bool first = true;

    for (const auto& item : m_contours) {
        if (item.path) {
            if (first) {
                bounds = item.path->boundingRect();
                first = false;
            } else {
                bounds = bounds.united(item.path->boundingRect());
            }
        }
    }

    qreal marginX = bounds.width() * 2.0;
    qreal marginY = bounds.height() * 2.0;
    QRectF extendedBounds = bounds.adjusted(-marginX, -marginY, marginX, marginY);
    m_scene->setSceneRect(extendedBounds);

    m_centerPoint = bounds.center();
    centerOn(m_centerPoint);
}

void MapWidget::wheelEvent(QWheelEvent* event) {
    double factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    scale(factor, factor);
    m_zoom *= factor;

    for (auto& item : m_contours) {
        if (item.path) {
            QPen pen = item.path->pen();
            pen.setWidthF(2.0 / m_zoom);
            item.path->setPen(pen);
        }
    }
}

void MapWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void MapWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastPanPoint;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_lastPanPoint = event->pos();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void MapWidget::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    if (!m_contours.isEmpty()) {
        centerOn(m_centerPoint);
    }
}

// ========== РАБОТА С ВАКАНСИЯМИ ==========

void MapWidget::addVacancyMarker(const Vacancy& v) {
    if (!v.hasCoords()) return;

    Point loc = v.getLocation();
    QPointF pos = geoToScreen(loc);

    qDebug() << "📍 Vacancy" << v.getId() << "at screen pos:" << pos.x() << pos.y();

    // Если маркер уже существует — просто обновляем позицию
    if (m_vacancyMarkers.contains(v.getId())) {
        m_vacancyMarkers[v.getId()]->setPos(pos);
        return;
    }

    // Создаём новый маркер
    QGraphicsEllipseItem* marker = new QGraphicsEllipseItem(-4, -4, 8, 8);
    marker->setBrush(QBrush(QColor(255, 255, 0, 100)));
    marker->setPen(QPen(Qt::white, 1));
    marker->setData(0, v.getId());
    marker->setZValue(10);
    marker->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    marker->setPos(pos);

    m_scene->addItem(marker);
    m_vacancyMarkers[v.getId()] = marker;
}

void MapWidget::updateVacancies(const QList<Vacancy>& vacancies) {
    qDebug() << "🔄 updateVacancies called with" << vacancies.size() << "vacancies";

    QSet<QString> currentIds;

    // Добавляем или обновляем маркеры для всех вакансий с координатами
    for (const auto& v : vacancies) {
        if (v.hasCoords()) {
            addVacancyMarker(v);
            currentIds.insert(v.getId());
        }
    }

    // Удаляем маркеры, которых больше нет в списке
    QStringList toRemove;
    for (auto it = m_vacancyMarkers.begin(); it != m_vacancyMarkers.end(); ++it) {
        if (!currentIds.contains(it.key())) {
            toRemove.append(it.key());
        }
    }

    for (const auto& id : toRemove) {
        if (m_highlightedMarker == m_vacancyMarkers[id]) {
            m_highlightedMarker = nullptr;
        }
        m_scene->removeItem(m_vacancyMarkers[id]);
        delete m_vacancyMarkers[id];
        m_vacancyMarkers.remove(id);
    }

    qDebug() << "🗺️ Map updated:" << m_vacancyMarkers.size() << "vacancy markers";
}

void MapWidget::highlightVacancy(const QString& id) {
    // Сбросить предыдущую подсветку
    if (m_highlightedMarker) {
        m_highlightedMarker->setBrush(QBrush(QColor(255, 255, 0, 100)));
        m_highlightedMarker->setRect(-4, -4, 8, 8);
    }

    // Подсветить новую
    if (m_vacancyMarkers.contains(id)) {
        m_highlightedMarker = m_vacancyMarkers[id];
        m_highlightedMarker->setBrush(QBrush(Qt::yellow));
        m_highlightedMarker->setRect(-6, -6, 12, 12);
    }
}

void MapWidget::clearVacancyMarkers() {
    for (auto* marker : m_vacancyMarkers) {
        if (marker) {
            m_scene->removeItem(marker);
            delete marker;
        }
    }
    m_vacancyMarkers.clear();
    m_highlightedMarker = nullptr;
}