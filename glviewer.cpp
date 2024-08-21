#include <QtGui>
#include "glviewer.h"

GlViewer::GlViewer(QWidget *pParent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), pParent)
{
	m_scene = NULL;
	m_center_x = m_center_y = 0.0;
	m_scale = 0.5;

	setAutoFillBackground(false);
}

void GlViewer::set_scene(Scene *pScene)
{
	m_scene = pScene;

	double length = std::min(height(), width()) / 2.0;
	QPoint qrect_bottem_left(0.5 * width() - 0.5 * length, 0.5 * height() + 0.5 * length);
	QPoint qrect_top_right(0.5 * width() + 0.5 * length, 0.5 * height() - 0.5 * length); 
	Point rect_bottem_left, rect_top_right;
	convert_to_world_space(qrect_bottem_left, rect_bottem_left);
	convert_to_world_space(qrect_top_right, rect_top_right);
	m_scene->set_rect(BBox(rect_bottem_left, rect_top_right));
    
	// rect: 1000-by-1000, radius: 5; keep the ratio
	double radius_ndc = (rect_top_right.x - rect_bottem_left.x) / 200.0;
	m_scene->set_circle_radius(radius_ndc);

	update_viewer_rect();
}

void GlViewer::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	 if (w <= h) {
        glOrtho(-1.0, 1.0, -1.0 * h / w, 1.0 * h / w, -1.0, 1.0);
    } else {
        glOrtho(-1.0 * w / h, 1.0 * w / h, -1.0, 1.0, -1.0, 1.0);
    }
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GlViewer::initializeGL()
{
	glClearColor(1., 1., 1., 0.);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SMOOTH);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GlViewer::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (!m_scene)
		return;

	glPushMatrix();
	glScaled(m_scale, m_scale, m_scale);
	glTranslated(-m_center_x, -m_center_y, 0.0);
	m_scene->render();
	glPopMatrix();
}


void GlViewer::move_camera(const QPoint &p0, const QPoint &p1)
{
	m_center_x -= double(p1.x() - p0.x()) / double(width());
	m_center_y += double(p1.y() - p0.y()) / double(height());
}

void GlViewer::convert_to_world_space(const QPoint& qpoint, Point& ndc_point)
{
	double aspect_ratio = double(height()) / double(width());

	double x = double(qpoint.x()) / double(width());
	x = (2.0 * x - 1.0) / m_scale;
	x += m_center_x;
	ndc_point.x = x;

	double y = 1.0 - double(qpoint.y()) / double(height());
	y = (2.0 * y - 1.0) * aspect_ratio / m_scale;
	y += m_center_y;
	ndc_point.y=y;
}
               
void GlViewer::convert_to_screen_space(const Point& ndc_point, QPoint& qpoint)
{
	double aspect_ratio = double(height()) / double(width());

    double x = (ndc_point.x - m_center_x) * m_scale;
    double y = (ndc_point.y - m_center_y) * m_scale / aspect_ratio;

    x = (x + 1.0) / 2.0;
    y = (1.0 - y) / 2.0;

    qpoint.setX(static_cast<int>(x * width()));
    qpoint.setY(static_cast<int>(y * height()));
}

void GlViewer::update_viewer_rect()
{
	QPoint qviewer_bottom_left(0, height()); 
    QPoint qviewer_top_right(width(), 0); 

	Point viewer_bottom_left;
	convert_to_world_space(qviewer_bottom_left, viewer_bottom_left);

	Point viewer_top_right;
	convert_to_world_space(qviewer_top_right, viewer_top_right);

	m_scene->set_viewer_rect(BBox(viewer_bottom_left, viewer_top_right));
}

void GlViewer::wheelEvent(QWheelEvent *event)
{
	if (!m_scene)
		return;
	m_scale += 0.05 * (event->delta() / 120.0);
	if (m_scale <= 0.0)
		m_scale = 0.0;
    
	update_viewer_rect();
	updateGL();
}

void GlViewer::sample_mouse_path(const QPoint &qpoint)
{
    Point point(0.0, 0.0);
	convert_to_world_space(qpoint, point);

	m_scene->set_mouse_pos(point);
}

void GlViewer::mouseMoveEvent(QMouseEvent *event)
{
	if (!m_scene)
	{
		return;
	}
    
	m_mouse_move = event->pos();

	move_camera(m_mouse_click, m_mouse_move);
	m_mouse_click = m_mouse_move;

	updateGL();
}

void GlViewer::mousePressEvent(QMouseEvent *event)
{
	if (!m_scene)
	{
		return;
	}

	m_mouse_click = event->pos();

	updateGL();
}

void GlViewer::mouseReleaseEvent(QMouseEvent *event)
{
	if (!m_scene)
	{
		return;
	}
		
	m_mouse_move = event->pos();

	setCursor(QCursor(Qt::ArrowCursor));
	updateGL();
}
