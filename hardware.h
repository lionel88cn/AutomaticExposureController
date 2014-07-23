#ifndef XDDMD_H
#define XDDMD_H
#include <qimage.h>
#include "windows.h"
#include "CyAPI.h"
#include "CH375DLL.H"
#include "qextserialport.h"
using namespace std;

class XDDMD{
public:
	enum FrameRate{
		HZ2000=1,
		HZ1800=2,
		HZ1600=3,
		HZ1400=4,
		HZ1200=5,
		HZ1000=6,
		HZ800=7,
		HZ600=8,
		HZ500=9,
		HZ450=10,
		HZ400=11,
		HZ350=12,
		HZ300=13,
		HZ250=14,
		HZ200=15,
		HZ180=16,
		HZ150=17,
		HZ120=18,
		HZ100=19,
		HZ90=20,
		HZ80=21,
		HZ70=22,
		HZ60=23,
		HZ50=24,
		HZ40=25,
		HZ30=26,
		HZ20=27,
		HZ10=28,
		HZ5=29,
		HZ1=30,
		HZ_HALF=31

	};
	enum SyncMode{Internal,External};
	XDDMD(CCyUSBDevice *UD,QextSerialPort *CD);
	~XDDMD();
private:
	CCyUSBDevice *USBDev;
	QextSerialPort *COMDev;
public:
	void loadImage(QImage *img);
	void changeMap(QImage *input,char *output);
	bool comTest();
	bool reset();
	bool addrReset();
	bool start();
	bool stop();
	bool setSyncMode(SyncMode mode);
	bool setPlayFrames(int num);
	bool setFrameRate(FrameRate rate);
	bool setReadAddr(int addr);
	bool setWriteAddr(int addr);
//private:
	bool comSendChar(char c);
	bool usbSendData(char *d,int len);
};

class DHCShutter{
public:
	DHCShutter();
	~DHCShutter();
	void openShutter();
	void closeShutter();
};

class DHCStage{
private:
	QextSerialPort *COMDev;
public:
	DHCStage(QextSerialPort *CD);
	~DHCStage();
	void move(int amount);
};

#endif 