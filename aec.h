#ifndef AEC_H
#define AEC_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QPushButton>
#include "imageviewer.h"
#include "controlpannel.h"
#include "ui_aec.h"

class AEC : public QMainWindow
{
	Q_OBJECT

public:
	AEC();
	~AEC();
public slots:
	//void openXML();
	//void saveXML();

private:
	ImageViewer *dockImageViewer;
	ControlPannel *controlPannel;
	QMenu *functionMenu;
	QMenu *helpMenu;
	QAction *devInitAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *closeAct;
	QAction *aboutAct;
};

#endif // AEC_H
