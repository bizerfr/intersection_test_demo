// Qt
#include <QtGui>
#include <QActionGroup>
#include <QFileDialog>
#include <QInputDialog>
#include <QClipboard>
#include <QMessageBox>
#include <QDialog>
#include <QInputDialog>

#include "main_window.h"

#include <fstream>

MainWindow::MainWindow() : 
QMainWindow(), Ui_MainWindow()
{
	setupUi(this);
    
	// init scene
	m_scene = new Scene;
	viewer->set_scene(m_scene);
	
	// accepts drop events
	setAcceptDrops(true);
    
}

MainWindow::~MainWindow()
{
	delete m_scene;
}

void MainWindow::update()
{
	viewer->repaint();
}

void MainWindow::on_actionClearAll_triggered()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
  	m_scene->clear_all();
	QApplication::restoreOverrideCursor();
	update();
}

void MainWindow::on_actionClear_Circles_triggered()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
  	m_scene->clear_circles();
	QApplication::restoreOverrideCursor();
	update();
}

void MainWindow::on_actionClear_Ray_triggered()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
  	m_scene->clear_ray();
	QApplication::restoreOverrideCursor();
	update();
}

void MainWindow::on_actionRandom_Circles_triggered()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
  	m_scene->generate_random_circles();
	QApplication::restoreOverrideCursor();
	update();
}

void MainWindow::on_actionRandom_Ray_triggered()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
  	m_scene->generate_random_ray();
	QApplication::restoreOverrideCursor();
	update();
}

void MainWindow::on_actionBuild_KDTree_triggered()
{
	m_scene->build_kdtree();
}

void MainWindow::on_actionDetect_Intersection_triggered()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	m_scene->detect_intersection();
	QApplication::restoreOverrideCursor();
	update();
}


