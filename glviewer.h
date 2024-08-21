#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QPaintEvent>
#include "scene.h"


class GlViewer : public QGLWidget 
{
    Q_OBJECT
    
private:
    Scene* m_scene;
    
    // camera
    double m_scale;
    double m_center_x, m_center_y;

    // mouse
    QPoint m_mouse_click, m_mouse_move;
    

public:
    GlViewer(QWidget *parent);

    void set_scene(Scene* pScene) ;

    void set_camera(const double x, const double y, const double s) 
    {
        m_center_x = x;
        m_center_y = y;
        m_scale = s;
    }
    
protected:
    // GL
    void paintGL();
    void initializeGL();
    void resizeGL(int width, int height);

    void move_camera(const QPoint& p0, const QPoint& p1);
    void convert_to_world_space(const QPoint& qpoint, Point& ndc_point);
    void convert_to_screen_space(const Point& ndc_point, QPoint& qpoint);
    void sample_mouse_path(const QPoint& point);

    // mouse
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    // viewer port
    void update_viewer_rect();
};

#endif
