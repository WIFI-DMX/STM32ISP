#pragma once
 
#ifndef __HEX_H_
#define __HEX_H_
 
typedef struct{
 
int len; //bin文件大小
UINT startAddress; //刷写的起始地址
BYTE *pContent;//转化后的内容
 
}HexToBinData;
 
typedef struct{
 
BYTE data[16];//数据
BYTE len;//数据长度
UINT pos;//偏移地址
BYTE type;//类型
 
}HexLinData;
 
int ConvertHexToBin(const char *str,HexToBinData *pData);
int GetStrFrHex(CString &FileSrc,CString &File);
 
#endif
//它的作用是将hex文件流str直接转化为bin文件流，并存储到HexToBinData结构体中，若成功则返回0，失败则返回非0.