#include <QtGui>
#include "main_window.h"

int main(int argv, char **args)
{	
	QApplication app(argv, args);
	app.setApplicationName("detecting intersection");
	MainWindow window;
	window.show();
	return app.exec();
}
