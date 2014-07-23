#ifndef ACTIONEXECUTOR_H
#define ACTIONEXECUTOR_H
#include <QItemSelectionModel>
#include <qthread.h>
#include "listviewmodel.h"
#include "hardware.h"

class ActionExecutor:public QThread
{
	Q_OBJECT
public:
	ActionExecutor(QWidget *par,ListViewModel *lvm);
	~ActionExecutor();
	void setRunFlag(bool rf);
	bool getRunFlag();
public slots:
	void runNext();
	void pause();
	void reset();
	void devInit();
signals:
	void setListViewSelection(const QModelIndex & index, QItemSelectionModel::SelectionFlags command);
private:
	QWidget *parent;
	ListViewModel *listViewModel;
	XDDMD *dmd;
	DHCShutter *shutter;
	DHCStage *stage;
	int current;
	bool runFlag;
	bool devFlag;
	void run();
};

#endif