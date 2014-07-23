#include "hardware.h"
#include <qdebug.h>


XDDMD::XDDMD(CCyUSBDevice *UD,QextSerialPort *CD){
	this->USBDev=UD;
	this->COMDev=CD;
	COMDev->open(QIODevice::ReadWrite);
}
XDDMD::~XDDMD(){
	COMDev->close();
	delete COMDev;
	delete USBDev;
}
bool XDDMD::usbSendData(char *d,int len){
	OVERLAPPED OvLap;
    OvLap.hEvent = CreateEvent(NULL, false, false, (LPCWSTR)"CYUSB_CTL");
    LONG rLen;
    PUCHAR Context;
    CCyControlEndPoint *ept = USBDev->ControlEndPt;
    LONG length		= 0;
    ept->Target     = TGT_DEVICE;
    ept->ReqType	= REQ_VENDOR;
    ept->Direction	= DIR_TO_DEVICE;
    ept->ReqCode	= 0xb1;  //ÃüÁîÖµ
    ept->Value      = 0;
    ept->Index      = 0;
	Context=ept->BeginDataXfer((PUCHAR)d, length, &OvLap);
	ept->WaitForXfer(&OvLap,100);
	ept->FinishDataXfer((PUCHAR)d, length, &OvLap,Context);
	Sleep(10);
	LONG dataLen=len;
	if(USBDev->BulkOutEndPt){
		USBDev->BulkOutEndPt->XferData((PUCHAR)d,dataLen,NULL,false);
		return 1;
	}
	return 0;
}
bool XDDMD::comSendChar(char c){	
	COMDev->putChar(c);
	Sleep(10);
	QByteArray rData=COMDev->readAll();
	if(rData.length()==1&&rData[0]==c) return 1;
	return 0;
}
bool XDDMD::comTest(){
	for(int i=0;i<10;++i){
		if(!comSendChar(0x50)) return 0;
	}
	return 1;
}
bool XDDMD::addrReset(){
	return comSendChar(0xF9);
}
bool XDDMD::reset(){
	return comSendChar(0x41);
}
bool XDDMD::start(){
	return comSendChar(0x51);
}
bool XDDMD::stop(){
	return comSendChar(0x50);
}
bool XDDMD::setReadAddr(int addr){
	char ctrlByte;
	if(addr<0||addr>1023) return 0;
	ctrlByte=0x80|(0x1F&addr);
	if (!comSendChar(ctrlByte)) return 0;
	Sleep(10);
	ctrlByte=0x60|(addr>>5);
	if (!comSendChar(ctrlByte)) return 0;
	if (!comSendChar(0xe1)) return 0;
	return 1;
}
bool XDDMD::setWriteAddr(int addr){
	char ctrlByte;
	if(addr<0||addr>1023) return 0;
	ctrlByte=0x80|(0x1F&addr);
	if (!comSendChar(ctrlByte)) return 0;
	Sleep(10);
	ctrlByte=0x60|(addr>>5);
	if (!comSendChar(ctrlByte)) return 0;
	if (!comSendChar(0xe9)) return 0;
	return 1;
}
bool XDDMD::setFrameRate(FrameRate rate){
	char ctrlByte=rate&0x1F;
	return comSendChar(ctrlByte);
}
bool XDDMD::setPlayFrames(int num){
	char ctrlByte;
	if(num<0||num>1023) return 0;
	ctrlByte=0xC0|(num&0x1F);
	if (!comSendChar(ctrlByte)) return 0;
	Sleep(10);
	ctrlByte=0xA0|((num>>5)&0x1F);
	if (!comSendChar(ctrlByte)) return 0;
	return 1;
}
bool XDDMD::setSyncMode(SyncMode mode){
	if (mode==Internal) return comSendChar(0x49);
	if (mode==External) return comSendChar(0x48);
	return 0;
}
void XDDMD::loadImage(QImage *img){
	while(!stop());
	while(!addrReset());
	while(!setSyncMode(XDDMD::Internal));
	while(!setFrameRate(XDDMD::HZ1));
	while(!setPlayFrames(1));
	char *data=new char[128*768];
	changeMap(img,data);
	usbSendData(data,128*768);
	Sleep(10);	
	while(!start());
	delete data;
}
void XDDMD::changeMap(QImage *input,char *output){
	char *buff=new char [1024*768];
	int m,i,j,k;
	ZeroMemory(buff,1024*768);
	ZeroMemory(output,128*768);
	for(i=0;i<768;++i){
		for(j=0;j<1024;++j){
			buff[1024*i+j]=(~input->pixelIndex(j,767-i))&0x01;
		}
	}
    for(m=0;m<768;m++){
        for(j=0;j<16;j++){
            for(i=0;i<8;i++)
            {
                for(k=0;k<8;k++) {
					output[128*m + 8*j + i] = (output[128*m + 8*j + i] << 1) | (buff[1024*m - j + 128*i + (127-16*k)] & 1);
				}
            }
		}
	}
	delete buff;
}

DHCShutter::DHCShutter(){
	CH375OpenDevice(0);
}
DHCShutter::~DHCShutter(){
	CH375CloseDevice(0);
}
void DHCShutter::openShutter(){
	UCHAR ctrlByte;
	ULONG len;
	ctrlByte=0x8A;
	len=(ULONG)1;
	CH375WriteData(0,&ctrlByte,&len);
}
void DHCShutter::closeShutter(){
	UCHAR ctrlByte;
	ULONG len;
	ctrlByte=0x0A;
	len=(ULONG)1;
	CH375WriteData(0,&ctrlByte,&len);
}

DHCStage::DHCStage(QextSerialPort *CD){
	COMDev=CD;
	COMDev->open(QIODevice::ReadWrite);
}
DHCStage::~DHCStage(){
	COMDev->close();
	delete COMDev;
}
void DHCStage::move(int amount){
	int pulseNum;
	bool dir;
	if(amount>=0){
		pulseNum=amount*1000;
		dir=1;
	}else{
		pulseNum=-amount*1000;
		dir=0;
	}
	QByteArray instruction;
	instruction.resize(20);
	instruction[0]='@';
	instruction[1]='1';
	instruction[2]='D';
	instruction[3]='0';
	instruction[4]=dir?'1':'0';
	instruction[5]='S';
	instruction[6]='F';
	instruction[7]='0';
	instruction[8]='P';
	int tmp;
	for(int i=0;i<8;++i) {
		tmp=(pulseNum>>i*4)&0x0F;
		if(tmp<10) instruction[16-i]=tmp+'0';
		else instruction[16-i]=tmp-10+'A';
	}
	instruction[17]='G';
	instruction[18]='\r';
	instruction[19]='\n';
	COMDev->write(instruction);
	qDebug()<<"Stage:"<<QString(instruction);
}

