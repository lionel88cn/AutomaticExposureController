#include "imageviewer.h"


ImageViewer::ImageViewer(QWidget* parent,Qt::WindowFlags flag){
	setFixedSize(320,240);
	graphicsView=new QGraphicsView;
	graphicsScene=new QGraphicsScene;
	graphicsView->setScene(graphicsScene);
	setWidget(graphicsView);
	setWindowTitle(tr("ImageViewer"));
}

ImageViewer::~ImageViewer(){}

void ImageViewer::showImage(QImage *img){
	QImage scaledImg;
	scaledImg=img->scaled(graphicsView->rect().width(),graphicsView->rect().height()-5,Qt::KeepAspectRatio);
	graphicsScene->clear();
	graphicsScene->addPixmap(QPixmap::fromImage(scaledImg));
}