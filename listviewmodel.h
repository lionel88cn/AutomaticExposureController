#ifndef LISTVIEWMODEL_H
#define LISTVIEWMODEL_H

#include<QAbstractListModel>
#include<qdom.h>
#include<qimage.h>
#include<qvector.h>

struct ExpoAction{
	enum ActionName{display,shutter,stage,delay};
	ActionName actionName;
	int numData;
	QString fileName;
	QImage *img;
};

class ListViewModel:public QAbstractListModel{
	Q_OBJECT
public:
	ListViewModel(QObject *parent=0);
	~ListViewModel();
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	bool setData(int row,ExpoAction actionData);
	bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
	void addAction(int row,ExpoAction newAction);
	void deleteAction(int row);
	ExpoAction getAction(int row);
	QImage *getImage(int row);
	QDomDocument *getDom();
	bool readDom(QDomDocument *doc);
private:
	QVector<ExpoAction> actionList;
};

#endif