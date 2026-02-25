// MapWidget.h
#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QList>
#include <QPair>
#include <QHash>
#include <QMessageBox>
#include "../core/Contour.h"
#include "../core/GeoCalculator.h"
#include "../core/IOContours.h"
#include "../core/Vacancy.h"

struct ContourItem {
    QGraphicsPathItem* path;
    QGraphicsTextItem* label;
    QString name;
    QColor color;
};

class MapWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapWidget(QWidget* parent = nullptr);
    ~MapWidget();

    void loadContours();

    // Методы для работы с вакансиями
    void updateVacancies(const QList<Vacancy>& vacancies);
    void highlightVacancy(const QString& id);
    void clearVacancyMarkers();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void addContour(const Contour& c, const QString& name, const QColor& color);
    void clearContours();
    QPointF geoToScreen(const Point& geo) const;
    QColor getContourColor(int index);
    void centerOnContours();
    void addVacancyMarker(const Vacancy& v);

private:
    QGraphicsScene* m_scene;
    GeoCalculator m_calc;
    QList<ContourItem> m_contours;
    bool m_isPanning;
    QPoint m_lastPanPoint;
    double m_zoom;
    QPointF m_centerPoint;

    // Для работы с вакансиями
    QHash<QString, QGraphicsEllipseItem*> m_vacancyMarkers;
    QGraphicsEllipseItem* m_highlightedMarker;
};

#endif // MAPWIDGET_H