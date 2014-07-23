#include "actionexecutor.h"
#include "qextserialenumerator.h"
#include <QStringList>
#include <QInputDialog>
#include <QMessageBox>
#include <qdebug.h>

ActionExecutor::ActionExecutor(QWidget *par,ListViewModel *lvm){
	parent=par;
	listViewModel=lvm;
	current=0;
	runFlag=0;
	devFlag=0;
	connect(this,SIGNAL(finished()),this,SLOT(runNext()));
}

ActionExecutor::~ActionExecutor(){
	if(devFlag){
		delete dmd;
		delete shutter;
		delete stage;
	}
}

void ActionExecutor::setRunFlag(bool rf){
	runFlag=rf;
}

bool ActionExecutor::getRunFlag(){
	return runFlag;
}

void ActionExecutor::runNext(){
	if(current<listViewModel->rowCount()){
		current++;
		if (runFlag) this->start();
	}
}

void ActionExecutor::pause(){
	runFlag=0;
}

void ActionExecutor::reset(){
	runFlag=0;
	current=0;
}

void ActionExecutor::devInit(){
	CCyUSBDevice *dmdUSBDev;
	QextSerialPort *dmdCOMDev;
	QextSerialPort *stageCOMDev;
	QStringList portNameList;
	QString dmdComPortName;
	QString stageComPortName;

	bool ok;
	if(devFlag) return;
	QList<QextPortInfo> portList=QextSerialEnumerator::getPorts();
	for(int i=0;i<portList.count();++i) portNameList<<portList[i].portName;
    dmdComPortName = QInputDialog::getItem(parent, tr("Select COM Port for DMD"), tr("Port:"), portNameList, 0, false, &ok);
	if (!ok) return;
	if (dmdComPortName.isEmpty()) {
		QMessageBox::information(parent, tr("Error"),tr("No COM Port selected"));
		return;
	}
	int index=portNameList.indexOf(dmdComPortName);
	portNameList.removeAt(index);
	stageComPortName = QInputDialog::getItem(parent, tr("Select COM Port for Stage"), tr("Port:"), portNameList, 0, false, &ok);
	if (!ok) return;
	if (stageComPortName.isEmpty()) {
		QMessageBox::information(parent, tr("Error"),tr("No COM Port selected"));
		return;
	}
	PortSettings dmdCOMsettings = {BAUD19200, DATA_8, PAR_EVEN, STOP_1, FLOW_OFF, 10};
	PortSettings stageCOMsettings = {BAUD19200, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
	dmdCOMDev=new QextSerialPort(dmdComPortName,dmdCOMsettings,QextSerialPort::Polling);
	dmdUSBDev=new CCyUSBDevice(NULL);
	stageCOMDev=new QextSerialPort(stageComPortName,stageCOMsettings,QextSerialPort::Polling);
	if (!dmdUSBDev->IsOpen()) QMessageBox::information(parent, tr("Error"),tr("USB is not open!"));
	dmd=new XDDMD(dmdUSBDev,dmdCOMDev);
	shutter=new DHCShutter();
	stage=new DHCStage(stageCOMDev);
	devFlag=1;
}

void ActionExecutor::run(){
	if(!devFlag){
		qDebug()<<"Devices not connected";
		return;
	}
	runFlag=1;
	if (current>=listViewModel->rowCount()){
		runFlag=0;
		qDebug()<<"No more actions to execute";
		return;
	}
	ExpoAction currentAction=listViewModel->getAction(current);
	QModelIndex index=listViewModel->index(current,0);
	emit setListViewSelection(index,QItemSelectionModel::ClearAndSelect);
	switch(currentAction.actionName){
	case ExpoAction::display:
		qDebug()<<"display action";
		dmd->loadImage(currentAction.img);
		break;
	case ExpoAction::shutter:
		qDebug()<<"shutter action";
		if(currentAction.numData) shutter->openShutter();
		else shutter->closeShutter();
		break;
	case ExpoAction::stage:
		qDebug()<<"stage action";
		stage->move(currentAction.numData);
		break;
	case ExpoAction::delay:
		qDebug()<<"delay action";
		msleep(currentAction.numData);
		break;
	default:
		qDebug()<<"Error!";
	}
}