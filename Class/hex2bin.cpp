#include "stdafx.h"
#include "Hex2Bin.h"
 
#define BUFLEN 1000000
static BYTE HexCharToByte(char c)
{
 
if(c>='0' && c<='9')
return c -'0';
else if(c>='a' && c<='f')
return c-'a'+0x0a;
else if(c>='A' && c <='F')
return c-'A'+0x0a;
 
return -1;
 
}
static BYTE GetFirstByte(const char *str)
{
 
ASSERT(str !=NULL);
 
BYTE tmp =0;
tmp =HexCharToByte(str[0]);
tmp <<=4;
tmp +=HexCharToByte(str[1]);
 
return tmp;
 
}
//从字符串中获取一行
static int GetLine(const char *str,char *pBuf)
{
 
ASSERT(str !=NULL);
ASSERT(pBuf !=NULL);
 
char *start =strchr((char *)str,':');
if(NULL ==start){
 
return -1;
 
}
 
char *end =strstr(start,"\r\n");
char *p =start;
char *p2 =pBuf;
int len=0;
for (;p<end+2;p++,p2++)
{
 
if(*p =='\0')
break;
*p2 =*p;
len ++;
 
}
*p2 ='\0';
 
return len;
 
}
 
//获取一行的数据
static int GetHexLineData(const char *line,HexLinData *pData)
{
 
ASSERT(line !=NULL);
ASSERT(pData !=NULL);
 
if(line[0] !=':')
return -1;
int i=1;
pData->len =GetFirstByte(&line[i]);
i +=2;
pData->pos =GetFirstByte(&line[i]);
i +=2;
pData->pos <<=8;
pData->pos +=GetFirstByte(&line[i]);
i +=2;
pData->type =GetFirstByte(&line[i]);
i +=2;
for(int j=0;j<pData->len;i+=2,j++){
 
pData->data[j] =GetFirstByte(&line[i]);
 
}
return 0;
 
}
//获取第一行指定类型的数据
static int GetFirstDataLine(const char *str,BYTE type,HexLinData *pData)
{
 
 
ASSERT(str !=NULL);
ASSERT(pData !=NULL);
 
char *p =(char *)str;
char line[128];
HexLinData data ={
0
};
int len =strlen(str);
int dataLen =0;
 
for(;p<str+len;p+=dataLen){
 
memset(line,0,128);
dataLen =GetLine(p,line);
if(dataLen <0)
return -1;
memset(&data,0x00,sizeof(HexLinData));
if(0 !=GetHexLineData(line,&data))
return -1;
 
if(data.type ==type){
 
memcpy(pData,&data,sizeof(HexLinData));
return 0;
 
}
 
}
return -1;
 
}
 
static int GetStartAddress(const char *str,UINT *pStartAddress)
{
 
HexLinData data ={
0
};
UINT basePos=0;
UINT pos;
 
if(0==GetFirstDataLine(str,4,&data))
{
//return -1;
for(int i=0;i<data.len;i++)
{
 
basePos <<=8;
basePos +=data.data[i];
 
}
 
}
memset(&data,0x00,sizeof(HexLinData));
if(0 !=GetFirstDataLine(str,0,&data))
return -1;
pos =data.pos;
 
*pStartAddress =(basePos<<16) +pos;
return 0;
 
}
 
int ConvertHexToBin(const char *str,HexToBinData *pData)
{
 
ASSERT(str !=NULL);
ASSERT(pData !=NULL);
 
 
UINT startAddress =0;
char line[128] ={
0
};
HexLinData data={
0
};
if(0 !=GetStartAddress(str,&startAddress))
return -1;
 
pData->startAddress =startAddress;
 
char *p =(char *)str;
int binLen =0;
int len =0;
int size =strlen(str);
 
/*FILE *file =fopen("test.hex","wb+");
fseek(file,0,SEEK_SET);
FILE *file1 =fopen("test.bin","wb+");
fseek(file1,0,SEEK_SET);*/
for(binLen=0;p<str+size;p +=len){
 
memset(line,0,128);
len =GetLine(p,line);
if(len <0)
break;
/*fwrite(line,1,len,file);*/
memset(&data,0x00,sizeof(HexLinData));
if(0 !=GetHexLineData(line,&data))
return -1;
if(data.type ==0){
 
binLen +=data.len;
/*fwrite(data.data,1,data.len,file1);*/
 
}
 
}
/*fclose(file);
fclose(file1);*/
pData->len =binLen;
pData->pContent =(BYTE *)malloc(pData->len+1);
if(pData->pContent ==NULL)
return -1;
 
p =(char *)str;
binLen =0;
len =0;
for(binLen=0;p<str+size;p +=len){
 
memset(line,0,128);
len =GetLine(p,line);
if(len <0)
break;
memset(&data,0x00,sizeof(HexLinData));
if(0 !=GetHexLineData(line,&data))
return -1;
if(data.type ==0){
 
memcpy(pData->pContent+binLen,data.data,data.len);
binLen +=data.len;
 
}
 
}
 
return 0;
 
}
 
int GetStrFrHex(CString &FileSrc,CString &File)
{
 
CFile rfile,wfile;
if(!rfile.Open((FileSrc),CFile::modeRead))
return 1;
if(!wfile.Open(File,CFile::modeCreate|CFile::modeWrite))
return 1;
BYTE buf[BUFLEN];
memset(buf,0x01,BUFLEN);
UINT readed;
HexToBinData* pData=new HexToBinData();
while((readed=rfile.Read(buf,BUFLEN))>0)
{
 
ConvertHexToBin((char*)buf,pData);
wfile.Write((char*)pData->pContent,pData->len);
 
}
 
rfile.Close();
wfile.Close(); 
return 0;
 
}