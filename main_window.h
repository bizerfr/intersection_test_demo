#ifndef _MAIN_WINDOW_
#define _MAIN_WINDOW_

// Qt
#include <QWidget>
#include <QString>

#include "scene.h"
#include "ui_intersection.h"


class MainWindow : public QMainWindow, public Ui_MainWindow
{
	Q_OBJECT

private:
	Scene* m_scene;

public:
	MainWindow();
	~MainWindow();

	void update();


public slots:
    // menu
	void on_actionClearAll_triggered(); 
	void on_actionClear_Circles_triggered();
    void on_actionClear_Ray_triggered();

	// algorithms
	void on_actionRandom_Circles_triggered();
	void on_actionRandom_Ray_triggered();
	void on_actionBuild_KDTree_triggered();
	void on_actionDetect_Intersection_triggered();


};

#endif // _MAIN_WINDOW_
