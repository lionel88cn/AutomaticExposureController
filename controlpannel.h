#ifndef CONTROLPANNEL_H
#define CONTROLPANNEL_H

#include <QWidget>
#include <qfile.h>
#include <qqueue.h>
#include <qgridlayout.h>
#include <QVBoxLayout>
#include <qpushbutton.h>
#include <qlistview.h>
#include "listviewmodel.h"
#include "actionexecutor.h"

class ControlPannel: public QWidget
{
	Q_OBJECT
public:
	ControlPannel(QWidget* parent=0);
	~ControlPannel();
public slots:
	void addDispAction();
	void addShutAction();
	void addStgAction();
	void addDlyAction();
	void deleteAction();
	void deviceInit();
	void processSelection(const QItemSelection &selected,const QItemSelection &deselected);
	void saveToFile();
	void readFromFile();
signals:
	void showImage(QImage *img);
private:
	ActionExecutor *actionExecutor;
	ListViewModel *listViewModel;
	QGridLayout *mainLayout;
	QVBoxLayout *buttonLayout1;
	QVBoxLayout *buttonLayout2;
	QPushButton *startButton;
	QPushButton *pauseButton;
	QPushButton *resetButton;
	QPushButton *addDispActionButton;
	QPushButton *addShutActionButton;
	QPushButton *addStgActionButton;
	QPushButton *addDlyActionButton;
	QPushButton *delActionButton;
	QListView	*listView;
};
#endif