#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QDockWidget>
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>

class ImageViewer : public QDockWidget
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent=0,Qt::WindowFlags flags=0);
	~ImageViewer();
public slots:
	void showImage(QImage *img);

private:
	QGraphicsView *graphicsView;
	QGraphicsScene *graphicsScene;

};

#endif // IMAGEVIEWER_H