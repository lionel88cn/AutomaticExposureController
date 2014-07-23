#include "controlpannel.h"
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qimagereader.h>
#include <qimage.h>
#include <qmessagebox.h>
#include <qinputdialog.h>

ControlPannel::ControlPannel(QWidget *parent){
	mainLayout=new QGridLayout;
	buttonLayout1=new QVBoxLayout;
	buttonLayout2=new QVBoxLayout;
	startButton=new QPushButton(tr("Start"));
	pauseButton=new QPushButton(tr("Pause"));
	resetButton=new QPushButton(tr("Reset"));
	addDispActionButton=new QPushButton(tr("Add display action"));
	connect(addDispActionButton,SIGNAL(clicked()),this,SLOT(addDispAction()));
	addShutActionButton=new QPushButton(tr("Add shutter action"));
	connect(addShutActionButton,SIGNAL(clicked()),this,SLOT(addShutAction()));
	addStgActionButton=new QPushButton(tr("Add stage action"));
	connect(addStgActionButton,SIGNAL(clicked()),this,SLOT(addStgAction()));
	addDlyActionButton=new QPushButton(tr("Add delay"));
	connect(addDlyActionButton,SIGNAL(clicked()),this,SLOT(addDlyAction()));
	delActionButton=new QPushButton(tr("Delete action"));
	connect(delActionButton,SIGNAL(clicked()),this,SLOT(deleteAction()));

	listViewModel=new ListViewModel;
	listView=new QListView;
	listView->setModel(listViewModel);
	connect(listView->selectionModel(),
			SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
			this,
			SLOT(processSelection(const QItemSelection &,const QItemSelection &)));

	actionExecutor=new ActionExecutor(this,listViewModel);
	connect(actionExecutor,
			SIGNAL(setListViewSelection(const QModelIndex &, QItemSelectionModel::SelectionFlags)),
			listView->selectionModel(),SLOT(select(const QModelIndex &, QItemSelectionModel::SelectionFlags)));
	qRegisterMetaType<QItemSelectionModel::SelectionFlags>("QItemSelectionModel::SelectionFlags");
	mainLayout->addWidget(listView,0,1);
	buttonLayout1->addWidget(startButton);
	connect(startButton,SIGNAL(clicked()),actionExecutor,SLOT(start()));
	buttonLayout1->addWidget(pauseButton);
	connect(pauseButton,SIGNAL(clicked()),actionExecutor,SLOT(pause()));
	buttonLayout1->addWidget(resetButton);
	connect(resetButton,SIGNAL(clicked()),actionExecutor,SLOT(reset()));
	mainLayout->addLayout(buttonLayout1,0,0);
	buttonLayout2->addWidget(addDispActionButton);
	buttonLayout2->addWidget(addShutActionButton);
	buttonLayout2->addWidget(addStgActionButton);
	buttonLayout2->addWidget(addDlyActionButton);
	buttonLayout2->addWidget(delActionButton);
	buttonLayout2->addStretch();
	mainLayout->addLayout(buttonLayout2,0,2);
	setLayout(mainLayout);
}

ControlPannel::~ControlPannel(){}


void ControlPannel::addDispAction(){                        
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),"",tr("All Files (*)"));
	QImageReader reader;
    reader.setFileName(fileName);
	if(fileName.isEmpty()) return;
	if(!reader.canRead()){
		QMessageBox::information(this, tr("Error"),tr("Format Not Supported!"));
		return;
	}
	QImage *tmpImg=new QImage;
	reader.read(tmpImg);
	if(tmpImg->format()!=QImage::Format_Mono){
		QMessageBox::information(this, tr("Error"),tr("Only binary image supported!"));
		delete tmpImg;
		return;
	}
	ExpoAction newDispAction;
	newDispAction.actionName=ExpoAction::display;
	newDispAction.img=tmpImg;
	newDispAction.fileName=fileName;
	emit showImage(newDispAction.img);
	QModelIndex index=listView->selectionModel()->currentIndex();
	if(index.isValid()) listViewModel->addAction(index.row(),newDispAction);
	else listViewModel->addAction(listViewModel->rowCount(),newDispAction);
}

void ControlPannel::addShutAction(){
	QStringList tmpList;
	bool ok;
	tmpList<<tr("Open")<<tr("Close");
	QString returnStr=QInputDialog::getItem(this,tr("Shutter action:"),tr("Select an action:"),tmpList,0,false,&ok);
	if(!ok) return;
	ExpoAction newShutAction;
	newShutAction.actionName=ExpoAction::shutter;
	if(returnStr==QString("Open")) newShutAction.numData=1;
	else newShutAction.numData=0;
	QModelIndex index=listView->selectionModel()->currentIndex();
	if(index.isValid()) listViewModel->addAction(index.row(),newShutAction);
	else listViewModel->addAction(listViewModel->rowCount(),newShutAction);
}

void ControlPannel::addStgAction(){
	bool ok;
	int returnInt=QInputDialog::getInt(this,tr("Set stage movement"),tr("Amount:(mm)"),0,-1000,1000,1,&ok);
	if(!ok) return;
	ExpoAction newStgAction;
	newStgAction.actionName=ExpoAction::stage;
	newStgAction.numData=returnInt;
	QModelIndex index=listView->selectionModel()->currentIndex();
	if(index.isValid()) listViewModel->addAction(index.row(),newStgAction);
	else listViewModel->addAction(listViewModel->rowCount(),newStgAction);
}

void ControlPannel::addDlyAction(){
	bool ok;
	int returnInt=QInputDialog::getInt(this,tr("Set delay time"),tr("Amount:(ms)"),0,0,100000,1,&ok);
	if(!ok) return;
	ExpoAction newDlyAction;
	newDlyAction.actionName=ExpoAction::delay;
	newDlyAction.numData=returnInt;
	QModelIndex index=listView->selectionModel()->currentIndex();
	if(index.isValid()) listViewModel->addAction(index.row(),newDlyAction);
	else listViewModel->addAction(listViewModel->rowCount(),newDlyAction);
}

void ControlPannel::deleteAction(){
	QModelIndex index=listView->selectionModel()->currentIndex();
	if(index.isValid())	listViewModel->deleteAction(index.row());
}

void ControlPannel::deviceInit(){
	actionExecutor->devInit();
}

void ControlPannel::processSelection(const QItemSelection &selected,const QItemSelection &deselected){
	QModelIndexList modelIndexList=selected.indexes();
	if(modelIndexList.isEmpty()) return;
	int pos=modelIndexList[0].row();
	QImage *tmpImg=listViewModel->getImage(pos);
	if(tmpImg) emit showImage(tmpImg);
}

void ControlPannel::saveToFile(){
	QString fileName=QFileDialog::getSaveFileName(this,tr("Save Action List to XML File"),"",tr("XML files (*.xml)"));
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly)) QMessageBox::information(this,tr("Error"),tr("Fail to open the file"));
	QDomDocument *dom=listViewModel->getDom();
	QTextStream textStream(&file);
	textStream<<dom->toString();
	file.close();
	delete dom;
}

void ControlPannel::readFromFile(){
	QString fileName=QFileDialog::getOpenFileName(this,tr("Read from XML file"),"",tr("XML files (*.xml)"));
	if(fileName.isEmpty()) return;
	QDomDocument doc("AECXML");
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)){
		QMessageBox::information(this,tr("Error"),tr("Fail to open the file"));
		return;
	}
	if(!doc.setContent(&file)){
		QMessageBox::information(this,tr("Error"),tr("Wrong XML file type"));
		return;
	}
	file.close();
	listViewModel->readDom(&doc);
}