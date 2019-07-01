#include "fifo.h"
//wifi-dmx.com
FIFO_ST MyFifo={0};

void FifoInit(FIFO_ST *Queue){
	Queue->front=Queue->rear;
	Queue->count=0;
}

unsigned char FifoIn(FIFO_ST *Queue,FifoType Idat){
	if((Queue->front==Queue->rear)&&(Queue->count==FifoSize)){
		return FifoFull;
	}
	else{
		Queue->Buf[Queue->rear]=Idat;
		Queue->rear=(Queue->rear+1)%FifoSize;
		Queue->count=Queue->count+1;
		return FifoNormal;
	}
}

unsigned char FifoOut(FIFO_ST *Queue,FifoType *Odat){
	if((Queue->front==Queue->rear)&&(Queue->count==0)){
		return FifoEmpty;
	}
	else{
		*Odat=Queue->Buf[Queue->front];
		Queue->front=(Queue->front+1)%FifoSize;
		Queue->count=Queue->count-1;
		return FifoNormal;
	}
}
//wifi-dmx.com