#include "aec.h"

AEC::AEC()
{
	dockImageViewer=new ImageViewer;
	addDockWidget(Qt::RightDockWidgetArea,dockImageViewer);
	controlPannel=new ControlPannel(this);
	setCentralWidget(controlPannel);
	connect(controlPannel,SIGNAL(showImage(QImage*)),dockImageViewer,SLOT(showImage(QImage*)));

	devInitAct=new QAction("&Initialize Devices",this);
	devInitAct->setStatusTip("Initialize Devices");
	connect(devInitAct,SIGNAL(triggered()),controlPannel,SLOT(deviceInit()));

	openAct=new QAction("&Open...", this); 
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip("Read action sequence from an XML file"); 
    connect(openAct, SIGNAL(triggered()), controlPannel, SLOT(readFromFile()));

	saveAct=new QAction("&Save", this); 
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip("Save action sequence to an XML file"); 
    connect(saveAct, SIGNAL(triggered()), controlPannel, SLOT(saveToFile()));

	closeAct=new QAction("&Close", this); 
    closeAct->setStatusTip("Close the program"); 
    //connect(closeAct, SIGNAL(triggered()), this, SLOT(saveXML()));

	aboutAct=new QAction("&About", this); 
    aboutAct->setStatusTip("About the program"); 
	//connect(closeAct, SIGNAL(triggered()), this, SLOT(saveXML()));


	functionMenu=menuBar()->addMenu("&Functions");
	functionMenu->addAction(devInitAct);
	functionMenu->addAction(openAct);
	functionMenu->addAction(saveAct);
	functionMenu->addAction(closeAct);
	helpMenu=menuBar()->addMenu("&Help");
	helpMenu->addAction(aboutAct);
}

AEC::~AEC()
{

}
