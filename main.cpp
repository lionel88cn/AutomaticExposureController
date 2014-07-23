#include "aec.h"
#include <QtWidgets/QApplication>
#include "MessageWindow.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qInstallMessageHandler(MessageWindow::AppendMsgWrapper);
	MessageWindow debugWindow;
	debugWindow.show();
	AEC mainApp;
	mainApp.show();
	return a.exec();
}
