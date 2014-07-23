#include "listviewmodel.h"
#include <qimagereader.h>

ListViewModel::ListViewModel(QObject *parent){}

ListViewModel::~ListViewModel(){}

int ListViewModel::rowCount(const QModelIndex & parent) const{
	return actionList.count();
}

QVariant ListViewModel::data(const QModelIndex &index, int role) const{
	if(role==Qt::DisplayRole)
	{
		int row=index.row();
		ExpoAction  action=actionList[row];
		QString dispData;
		switch(action.actionName){
		case ExpoAction::display:
			dispData+=tr("DISPLAY");
			return dispData;
		case ExpoAction::shutter:
			dispData+=tr("SHUTTER\t");
			if (action.numData) dispData+=tr("OPEN");
			else dispData+=tr("CLOSE");
			return dispData;
		case ExpoAction::stage:
			dispData+=QString("STAGE\tMOVE %1").arg(action.numData);
			return dispData;
		case ExpoAction::delay:
			dispData+=QString("DELAY\t%1 MS").arg(action.numData);
			return dispData;
		default:return tr("Error");
		}
	}
	return QVariant();
}

bool ListViewModel::setData(int row,ExpoAction actionData){
	if (row<0||row>=rowCount()) return 0;
	actionList[row]=actionData;
	return 1;
}

bool ListViewModel::insertRows(int row, int count, const QModelIndex & parent){
	if (row<0||row>rowCount()) return 0;
	this->beginInsertRows(parent,row,row+count-1);
	ExpoAction emptyAction;
	actionList.insert(row,count,emptyAction);
	this->endInsertRows();
	return 1;
}

bool ListViewModel::removeRows(int row, int count, const QModelIndex & parent){
	int first=row;
	int last=row+count-1;
	if(first<0||first>=rowCount()) return 0;
	if(last<0||last>=rowCount()) return 0;
	this->beginRemoveRows(QModelIndex(),first,last);
	actionList.remove(row,count);
	this->endRemoveRows();
	return 1;
}


void ListViewModel::addAction(int row,ExpoAction newAction){
	insertRows(row,1);
	setData(row,newAction);
}

void ListViewModel::deleteAction(int row){
	if(actionList[row].actionName==ExpoAction::display) delete actionList[row].img;
	removeRows(row,1);
}

ExpoAction ListViewModel::getAction(int row){
	return actionList[row];
}

QImage *ListViewModel::getImage(int row){
	if(row<0||row>=rowCount()) return NULL;
	if(actionList[row].actionName==ExpoAction::display) return actionList[row].img;
	else return NULL;
}

QDomDocument *ListViewModel::getDom(){
	QDomDocument *doc=new QDomDocument("AECXML");
	QDomElement root=doc->createElement("actionList");
	doc->appendChild(root);
	for(int i=0;i<rowCount();++i){
		QDomElement tmpElement=doc->createElement("Action");
		tmpElement.setAttribute("Type",actionList[i].actionName);
		if(actionList[i].actionName==ExpoAction::display) tmpElement.setAttribute("Data",actionList[i].fileName);
		else tmpElement.setAttribute("Data",actionList[i].numData);
		root.appendChild(tmpElement);
	}
	return doc;
}

bool ListViewModel::readDom(QDomDocument *doc){
	QDomElement root=doc->documentElement();
	if(root.tagName()!="actionList") return 0;
	removeRows(0,rowCount());
	ExpoAction tmpAction;
	QDomNode n=root.firstChild();
	while(!n.isNull()){
		QDomElement e=n.toElement();
		if(!e.isNull()){
			tmpAction.actionName=(ExpoAction::ActionName)e.attribute("Type","").toInt();
			if(tmpAction.actionName==ExpoAction::display){
				tmpAction.fileName=e.attribute("Data","");
				QImageReader reader;
				reader.setFileName(tmpAction.fileName);
				QImage *tmpImg=new QImage;
				reader.read(tmpImg);
				tmpAction.img=tmpImg;
				tmpAction.numData=0;
			}
			else{
				tmpAction.numData=e.attribute("Data","0").toInt();
				tmpAction.fileName="";
				tmpAction.img=0;
			}
		}
		addAction(rowCount(),tmpAction);
		n=n.nextSibling();
	}
}