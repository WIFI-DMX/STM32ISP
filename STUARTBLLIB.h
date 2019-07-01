/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : STUARTBLLIB.h
* Author             : MCD Application Team
* Version            : v2.8.0
* Date               : 01-September-2015
* Description        : Defines the UART  boot loader protocol interface
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
********************************************************************************
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE
* "MCD-ST Liberty SW License Agreement V2.pdf"
*******************************************************************************/
//wifi-dmx.com
#ifndef STDLIB_H
#define STDLIB_H

#include "StdAfx.h"
//nclude "rs232.h"

#ifdef STUARTBLLIB_EXPORTS
#define STUARTBLLIB_API __declspec(dllexport)
#else
#define STUARTBLLIB_API //__declspec(dllimport)
#endif

const unsigned char INIT_CON                    = 0x7F;

const unsigned char GET_CMD                     = 0x00; //Get the version and the allowed commands supported by the current version of the boot loader
const unsigned char GET_VER_ROPS_CMD            = 0x01; //Get the BL version and the Read Protection status of the NVM
const unsigned char GET_ID_CMD                  = 0x02; //Get the chip ID
const unsigned char SET_SPEED_CMD               = 0x03; //set the new baudrate
const unsigned char READ_CMD                    = 0x11; //Read up to 256 unsigned chars of memory starting from an address specified by the user
const unsigned char GO_CMD                      = 0x21; //Jump to an address specified by the user to execute (a loaded) code
const unsigned char WRITE_CMD                   = 0x31; //Write maximum 256 unsigned chars to the RAM or the NVM starting from an address specified by the user
const unsigned char ERASE_CMD                   = 0x43; //Erase from one to all the NVM sectors
const unsigned char ERASE_EXT_CMD               = 0x44; //Erase from one to all the NVM sectors
const unsigned char WRITE_PROTECT_CMD           = 0x63; //Enable the write protection in a permanent way for some sectors
const unsigned char WRITE_TEMP_UNPROTECT_CMD    = 0x71; //Disable the write protection in a temporary way for all NVM sectors
const unsigned char WRITE_PERM_UNPROTECT_CMD    = 0x73; //Disable the write protection in a permanent way for all NVM sectors
const unsigned char READOUT_PROTECT_CMD         = 0x82; //Enable the readout protection in a permanent way
const unsigned char READOUT_TEMP_UNPROTECT_CMD  = 0x91; //Disable the readout protection in a temporary way
const unsigned char READOUT_PERM_UNPROTECT_CMD  = 0x92; //Disable the readout protection in a permanent way


const unsigned char SUCCESS                     = 0x00; // No error 
const unsigned char ERROR_OFFSET                = 0x00; //error offset 

const unsigned char COM_ERROR_OFFSET            = ERROR_OFFSET + 0x00;
const unsigned char NO_CON_AVAILABLE            = COM_ERROR_OFFSET + 0x01;  // No serial port opened
const unsigned char COM_ALREADY_OPENED          = COM_ERROR_OFFSET + 0x02;  // Serial port already opened
const unsigned char CANT_OPEN_COM               = COM_ERROR_OFFSET + 0x03;  // Fail to open serial port
const unsigned char SEND_FAIL                   = COM_ERROR_OFFSET + 0x04;  // send over serial port fail
const unsigned char READ_FAIL                   = COM_ERROR_OFFSET + 0x05;  // Read from serial port fail

const unsigned char SYS_MEM_ERROR_OFFSET        = ERROR_OFFSET + 0x10;
const unsigned char CANT_INIT_BL                = SYS_MEM_ERROR_OFFSET + 0x01; // Fail to start system memory BL
const unsigned char UNREOGNIZED_DEVICE          = SYS_MEM_ERROR_OFFSET + 0x02; // Unreconized device
const unsigned char CMD_NOT_ALLOWED             = SYS_MEM_ERROR_OFFSET + 0x03; // Command not allowed
const unsigned char CMD_FAIL                    = SYS_MEM_ERROR_OFFSET + 0x04; // command failed

const unsigned char PROGRAM_ERROR_OFFSET        = ERROR_OFFSET + 0x20;
const unsigned char INPUT_PARAMS_ERROR          = PROGRAM_ERROR_OFFSET + 0x01;
const unsigned char INPUT_PARAMS_MEMORY_ALLOCATION_ERROR = PROGRAM_ERROR_OFFSET + 0x02;



enum  ACKS {UNDEFINED=0x00, ST75=0x75, ST79=0x79};
enum  INTERFACE_TYPE {UART, CAN};

enum  EBaudRate { brCustom,br110, br300, br600, br1200, br2400, br4800, br9600, br14400, br19200, br38400,
                  br56000, br57600, br115200, br128000, br256000 };// Port Numbers ( custom or COM1..COM16 }
enum  EPortNumber { pnCustom,pnCOM1, pnCOM2, pnCOM3, pnCOM4, pnCOM5, pnCOM6, pnCOM7,pnCOM8, pnCOM9, pnCOM10, 
                    pnCOM11, pnCOM12, pnCOM13,pnCOM14, pnCOM15, pnCOM16 };// Data bits ( 5, 6, 7, 8 }
enum  EDataBits { db5BITS, db6BITS, db7BITS, db8BITS };
// Stop bits ( 1, 1.5, 2 }
enum  EStopBits { sb1BITS, sb1HALFBITS, sb2BITS };
// Parity ( None, odd, even, mark, space }
enum  EParity { ptNONE, ptODD, ptEVEN, ptMARK, ptSPACE };
// Hardware Flow Control ( None, None + RTS always on, RTS/CTS }
enum  EHwFlowControl { hfNONE, hfNONERTSON, hfRTSCTS };
// Software Flow Control ( None, XON/XOFF }
enum  ESwFlowControl { sfNONE, sfXONXOFF };
// What to do with incomplete (incoming} packets ( Discard, Pass }
enum  EPacketMode { pmDiscard, pmPass };

enum  OPERATION {NONE, ERASE, UPLOAD, DNLOAD, DIS_R_PROT, DIS_W_PROT, ENA_R_PROT, ENA_W_PROT};

typedef struct RESULT
{
	OPERATION operation;
	char*     filename;
    HANDLE    Image;
}* LPRESULT;

typedef struct Commands
{
	int GET_CMD                    ; //Get the version and the allowed commands supported by the current version of the boot loader
    int GET_VER_ROPS_CMD           ; //Get the BL version and the Read Protection status of the NVM
    int GET_ID_CMD                 ; //Get the chip ID
	int SET_SPEED_CMD                   ; //Change the CAN baudrate
    int READ_CMD                   ; //Read up to 256 unsigned chars of memory starting from an address specified by the user
    int GO_CMD                     ; //Jump to an address specified by the user to execute (a loaded) code
    int WRITE_CMD                  ; //Write maximum 256 unsigned chars to the RAM or the NVM starting from an address specified by the user
    int ERASE_CMD                  ; //Erase from one to all the NVM sectors
	int ERASE_EXT_CMD              ; //Erase from one to all the NVM sectors
    int WRITE_PROTECT_CMD          ; //Enable the write protection in a permanent way for some sectors
    int WRITE_TEMP_UNPROTECT_CMD   ; //Disable the write protection in a temporary way for all NVM sectors
    int WRITE_PERM_UNPROTECT_CMD   ; //Disable the write protection in a permanent way for all NVM sectors
    int READOUT_PROTECT_CMD   ; //Enable the readout protection in a permanent way
    int READOUT_TEMP_UNPROTECT_CMD ; //Disable the readout protection in a temporary way
    int READOUT_PERM_UNPROTECT_CMD ; //Disable the readout protection in a permanent way
}* LPCommands;

typedef struct TARGET_DESCRIPTOR
{
	unsigned char Version                    ;
	unsigned char CmdCount                   ;
	unsigned char PIDLen                     ;
	unsigned char* PID                       ;

	unsigned char ROPE                       ;
	unsigned char ROPD                       ;

	int GET_CMD                    ; //Get the version and the allowed commands supported by the current version of the boot loader
    int GET_VER_ROPS_CMD           ; //Get the BL version and the Read Protection status of the NVM
    int GET_ID_CMD                 ; //Get the chip ID
    int SET_SPEED_CMD              ;
	int READ_CMD                   ; //Read up to 256 unsigned chars of memory starting from an address specified by the user
    int GO_CMD                     ; //Jump to an address specified by the user to execute (a loaded) code
    int WRITE_CMD                  ; //Write maximum 256 unsigned chars to the RAM or the NVM starting from an address specified by the user
    int ERASE_CMD                  ; //Erase from one to all the NVM sectors
	int ERASE_EXT_CMD              ; //Erase from one to all the NVM sectors
    int WRITE_PROTECT_CMD          ; //Enable the write protection in a permanent way for some sectors
    int WRITE_TEMP_UNPROTECT_CMD   ; //Disable the write protection in a temporary way for all NVM sectors
    int WRITE_PERM_UNPROTECT_CMD   ; //Disable the write protection in a permanent way for all NVM sectors
    int READOUT_PERM_PROTECT_CMD   ; //Enable the readout protection in a permanent way
    int READOUT_TEMP_UNPROTECT_CMD ; //Disable the readout protection in a temporary way
    int READOUT_PERM_UNPROTECT_CMD ; //Disable the readout protection in a permanent way
}* LPTARGET_DESCRIPTOR;

typedef struct STBL_Request
{
	unsigned char   _cmd;
	DWORD  _address;
	WORD   _length;
	unsigned char   _nbSectors;
    LPTARGET_DESCRIPTOR _target;
	unsigned char* _data;
	WORD   _wbSectors;
}* LPSTBL_Request; 


extern "C"
{

/************************************************************************************/
/* SET COMMUNICATION INTERFACE TYPE
/* UART - ...
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_SetComIntType(unsigned char com_int_type);

/************************************************************************************/
/* GET PROGRESS STATE
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char GetProgress(unsigned char* progress);  

/************************************************************************************/
/* GET ACTIVITY TIME
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char GetActivityTime(LPDWORD time);  

/************************************************************************************/
/* SET ACTIVITY TIME
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char SetActivityTime(DWORD time); 

/************************************************************************************/
/* GetFlashSize
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_GetFlashSize(DWORD Addr, LPWORD val);
/************************************************************************************/
/* GetRAMSize
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_GetMemoryAddress(DWORD Addr, unsigned char* val);
/************************************************************************************/
/* GetRDPOptionunsigned char
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_GetRDPOption unsigned char(unsigned char* RDP);
/************************************************************************************/
/* GetWRPOptionunsigned chars
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_GetWRPOption unsigned char(unsigned char* WRP0, unsigned char* WRP1, unsigned char* WRP2, unsigned char* WRP3);
/************************************************************************************/
/* Basic function to send a request
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char Send_RQ(LPSTBL_Request pRQ);

/************************************************************************************/
/*
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char SetCOMSettings(int numPort, long speedInBaud, int nbBit,
	                            int parity, float nbStopBit);
/************************************************************************************/
/*
/*
/*
/************************************************************************************/
/*STUARTBLLIB_API unsigned char ESetCOMSettings(EPortNumber numPort, EBaudRate speedInBaud, EDataBits nbBit,
	                            EParity parity, EStopBits nbStopBit);*/
/************************************************************************************/
/*
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char COM_Open();
/************************************************************************************/
/*
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char COM_Close();
/************************************************************************************/
/*
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_SetSpeed(DWORD speed);
/************************************************************************************/
/*
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_Init_BL();
/************************************************************************************/
/* 0x00; //Get the version and the allowed commands supported by the current version of the boot loader
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_GET(LPunsigned char Version, LPCommands pCmds);                      
/************************************************************************************/
/* 0x01; //Get the BL version and the Read Protection status of the NVM
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_GET_VER_ROPS(LPunsigned char Version, LPunsigned char ROPEnabled, LPunsigned char ROPDisabled);             
/************************************************************************************/
/* 0x02; //Get the chip ID
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_GET_ID(LPunsigned char size, LPunsigned char pID);                   
/************************************************************************************/
/* 0x11; //Read up to 256 unsigned chars of memory starting from an address specified by the user
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_READ(DWORD Address, unsigned char Size, LPunsigned char pData);                     
/************************************************************************************/
/* 0x21; //Jump to an address specified by the user to execute (a loaded) code
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_GO(DWORD Address);                       
/************************************************************************************/
/* 0x31; //Write maximum 256 unsigned chars to the RAM or the NVM starting from an address specified by the user
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_WRITE(DWORD address, unsigned char size, LPunsigned char pData);                    
/************************************************************************************/
/* 0x43; //Erase from one to all the NVM sectors
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_ERASE(WORD NbSectors, LPunsigned char pSectors);                     
/************************************************************************************/
/* 0x63; //Enable the write protection in a permanent way for some sectors
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_WRITE_PROTECT(unsigned char NbSectors, LPunsigned char pSectors);            
/************************************************************************************/
/* 0x71; //Disable the write protection in a temporary way for all NVM sectors
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_WRITE_TEMP_UNPROTECT();     
/************************************************************************************/
/* 0x73; //Disable the write protection in a permanent way for all NVM sectors
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_WRITE_PERM_UNPROTECT();     
/************************************************************************************/
/* 0x82; //Enable the readout protection in a permanent way
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_READOUT_PROTECT();     
/************************************************************************************/
/* 0x91; //Disable the readout protection in a temporary way
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_READOUT_TEMP_UNPROTECT();   
/************************************************************************************/
/* 0x92; //Disable the readout protection in a permanent way
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_READOUT_PERM_UNPROTECT();   
/************************************************************************************/
/* UPLOAD
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_UPLOAD(DWORD Address, LPunsigned char pData, DWORD Length);  
/************************************************************************************/
/* VERIFY
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_VERIFY(DWORD Address, LPunsigned char pData, DWORD Length,int bTruncateLeadFFForDnLoad); 
/************************************************************************************/
/* DNLOAD
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_DNLOAD(DWORD Address, LPunsigned char pData, DWORD Length,int bTruncateLeadFFForDnLoad);   

/************************************************************************************/
/* SET PACKET SIZE
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char SetPaketSize(unsigned char size);
 
/************************************************************************************/
/* GET PACKET SIZE
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char GetPaketSize(LPunsigned char size);

/************************************************************************************/
/* GetAckValue
/*
/*
/************************************************************************************/
STUARTBLLIB_API ACKS GetAckValue();

/************************************************************************************/
/* IsConnected
/*
/*
/************************************************************************************/
STUARTBLLIB_API int COM_is_Open();

/************************************************************************************/
/* SetTimeOut
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char SetTimeOut(DWORD vms);
/************************************************************************************/
/* GetUserOptionunsigned char
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_GetUserOptionunsigned char(LPunsigned char User);
/************************************************************************************/
/* GetDataOptionunsigned char
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_GetDataOptionunsigned char(LPunsigned char Data0, LPunsigned char Data1);

/************************************************************************************/
/* SetSIFData
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_SetOptionunsigned chars(DWORD Addr,unsigned char User, unsigned char RDP, unsigned char Data0, unsigned char Data1, 
								   unsigned char WRP0, unsigned char WRP1, unsigned char WRP2, unsigned char WRP3);

/************************************************************************************/
/* GetSIFData
/*
/*
/************************************************************************************/
STUARTBLLIB_API unsigned char TARGET_GetOptionunsigned chars(DWORD Addr,LPunsigned char User, LPunsigned char RDP, LPunsigned char Data0, LPunsigned char Data1, 
								   LPunsigned char WRP0, LPunsigned char WRP1, LPunsigned char WRP2, LPunsigned char WRP3);

/************************************************************************************/
/* Set Rts Line State
/*   High = TRUE
/*   Low  = FALSE
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_SetRts(int Val);

/************************************************************************************/
/* Set Dtr Line State 
/*   High = TRUE
/*   Low  = FALSE
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_SetDtr(int Val);

/************************************************************************************/
/*   Set the state of TXD. Return: true if success.
/*   High = TRUE
/*   Low  = FALSE
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_setTxd(int val);	
		
/************************************************************************************/
/*   Return: The state of CTS.
/*   High = TRUE
/*   Low  = FALSE
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_getCts(int* pval);					
			
/************************************************************************************/
/*   Return: The state of DTR.
/*   High = TRUE
/*   Low  = FALSE
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_getDtr(int* pval);	
					
/************************************************************************************/
/*   Return: The state of RI.
/*   High = TRUE
/*   Low  = FALSE
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_getRi(int* pval);					
	
/************************************************************************************/
/*   Return: The state of DTR.
/*   High = TRUE
/*   Low  = FALSE
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_getCd(int* pval);	


/************************************************************************************/
/* Set Echo Mode
/*   0   =  Echo Disabled
/*   1   =  Echo Back Enabled
/*   2   =  Listen Echo Enabled
/************************************************************************************/
STUARTBLLIB_API unsigned char STBL_SetEcho(int val);	


/************************************************************************************/
/* SetFlowControl : Enable/Disable Flow Control of DTR and RTS
/* FALSE   =   Disabled
/* TRUE    =   Enabled
/************************************************************************************/
STUARTBLLIB_API  unsigned char STBL_SetFlowControl(int val);
		

}

#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE******/
