#include <Qt\qapplication.h>
//#include "MeGlWindow.h"
#include "Prj3Window.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	// Prj2
	//MeGlWindow meWindow;
	//meWindow.show();

	// Prj3
	Prj3Window meWindow;
	meWindow.show();

	return app.exec();
}