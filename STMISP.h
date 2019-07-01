#ifndef __STMISP_H__
#define __STMISP_H__

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef int                 BOOL;
typedef unsigned long       DWORD;
typedef BYTE               *LPBYTE;
//wifi-dmx.com
const BYTE INIT_CON                    = 0x7F;

const BYTE GET_CMD                     = 0x00; //Get the version and the allowed commands supported by the current version of the boot loader
const BYTE GET_VER_ROPS_CMD            = 0x01; //Get the BL version and the Read Protection status of the NVM
const BYTE GET_ID_CMD                  = 0x02; //Get the chip ID
const BYTE SET_SPEED_CMD               = 0x03; //set the new baudrate
const BYTE READ_CMD                    = 0x11; //Read up to 256 bytes of memory starting from an address specified by the user
const BYTE GO_CMD                      = 0x21; //Jump to an address specified by the user to execute (a loaded) code
const BYTE WRITE_CMD                   = 0x31; //Write maximum 256 bytes to the RAM or the NVM starting from an address specified by the user
const BYTE ERASE_CMD                   = 0x43; //Erase from one to all the NVM sectors
const BYTE ERASE_EXT_CMD               = 0x44; //Erase from one to all the NVM sectors
const BYTE WRITE_PROTECT_CMD           = 0x63; //Enable the write protection in a permanent way for some sectors
const BYTE WRITE_TEMP_UNPROTECT_CMD    = 0x71; //Disable the write protection in a temporary way for all NVM sectors
const BYTE WRITE_PERM_UNPROTECT_CMD    = 0x73; //Disable the write protection in a permanent way for all NVM sectors
const BYTE READOUT_PROTECT_CMD         = 0x82; //Enable the readout protection in a permanent way
const BYTE READOUT_TEMP_UNPROTECT_CMD  = 0x91; //Disable the readout protection in a temporary way
const BYTE READOUT_PERM_UNPROTECT_CMD  = 0x92; //Disable the readout protection in a permanent way


const BYTE SUCCESS_IAP                 = 0x00; // No error 
const BYTE ERROR_OFFSET                = 0x00; //error offset 

const BYTE COM_ERROR_OFFSET            = ERROR_OFFSET + 0x00;
const BYTE NO_CON_AVAILABLE            = COM_ERROR_OFFSET + 0x01;  // No serial port opened
const BYTE COM_ALREADY_OPENED          = COM_ERROR_OFFSET + 0x02;  // Serial port already opened
const BYTE CANT_OPEN_COM               = COM_ERROR_OFFSET + 0x03;  // Fail to open serial port
const BYTE SEND_FAIL                   = COM_ERROR_OFFSET + 0x04;  // send over serial port fail
const BYTE READ_FAIL                   = COM_ERROR_OFFSET + 0x05;  // Read from serial port fail

const BYTE SYS_MEM_ERROR_OFFSET        = ERROR_OFFSET + 0x10;
const BYTE CANT_INIT_BL                = SYS_MEM_ERROR_OFFSET + 0x01; // Fail to start system memory BL
const BYTE UNREOGNIZED_DEVICE          = SYS_MEM_ERROR_OFFSET + 0x02; // Unreconized device
const BYTE CMD_NOT_ALLOWED             = SYS_MEM_ERROR_OFFSET + 0x03; // Command not allowed
const BYTE CMD_FAIL                    = SYS_MEM_ERROR_OFFSET + 0x04; // command failed

const BYTE PROGRAM_ERROR_OFFSET        = ERROR_OFFSET + 0x20;
const BYTE INPUT_PARAMS_ERROR          = PROGRAM_ERROR_OFFSET + 0x01;
const BYTE INPUT_PARAMS_MEMORY_ALLOCATION_ERROR = PROGRAM_ERROR_OFFSET + 0x02;
const BYTE LIB_LOADING_ERROR           = PROGRAM_ERROR_OFFSET + 0x03;



enum  ACKS {UNDEFINED=0x00, ST75=0x75, ST79=0x79};
enum  SYSIAP {
	SYSIAP_CONNECT=0,
	SYSIAP_Getversion,
	SYSIAP_Getversion_ACK, 
	SYSIAP_Getprotect,
	SYSIAP_Getprotect_ACK, 
	SYSIAP_GetchipID, 
	SYSIAP_GetchipID_ACK,
	SYSIAP_READSize_CMD,
	SYSIAP_READSize_CMD_ACK,
	SYSIAP_READSize_ADDR,
	SYSIAP_READSize_ADDR_ACK,
	SYSIAP_READSize_Num,
	SYSIAP_READSize_Num_ACK,
	SYSIAP_READ_CMD,
	SYSIAP_READ_CMD_ACK,
	SYSIAP_READ_ADDR,
	SYSIAP_READ_ADDR_ACK,
	SYSIAP_READ_Num,
	SYSIAP_READ_Num_ACK,
	SYSIAP_SETunprotectWrite,
	SYSIAP_SETunprotectWrite_ACK,
	SYSIAP_SETunprotectREADOUT,
	SYSIAP_SETunprotectREADOUT_ACK,
	SYSIAP_ERASE_CMD,
	SYSIAP_ERASE_CMD_ACK,
	SYSIAP_ERASE_ALL,
	SYSIAP_ERASE_ALL_ACK,
	SYSIAP_WRITE_CMD,
	SYSIAP_WRITE_CMD_ACK,
	SYSIAP_WRITE_ADDR,
	SYSIAP_WRITE_ADDR_ACK,
	SYSIAP_WRITE_DATA,
	SYSIAP_WRITE_DATA_ACK,
	SYSIAP_GO_CMD,
	SYSIAP_GO_CMD_ACK,
	SYSIAP_GO_ADDR,
	SYSIAP_GO_ADDR_ACK,
    SYSIAP_WRITE_FINISH,
	SYSIAP_WRITE_STOP,
};

typedef struct 
{
	unsigned char Num;
	unsigned char SYSstate;
	unsigned char ReceiveNum;
	unsigned int  Times;
	unsigned int  Timeout;
	unsigned int  AllTime;
	unsigned int  Writelength;
}IAP_Type;

typedef struct Commands
{
	BOOL GET_CMD                    ; //Get the version and the allowed commands supported by the current version of the boot loader
    BOOL GET_VER_ROPS_CMD           ; //Get the BL version and the Read Protection status of the NVM
    BOOL GET_ID_CMD                 ; //Get the chip ID
	BOOL SET_SPEED_CMD                   ; //Change the CAN baudrate
    BOOL READ_CMD                   ; //Read up to 256 bytes of memory starting from an address specified by the user
    BOOL GO_CMD                     ; //Jump to an address specified by the user to execute (a loaded) code
    BOOL WRITE_CMD                  ; //Write maximum 256 bytes to the RAM or the NVM starting from an address specified by the user
    BOOL ERASE_CMD                  ; //Erase from one to all the NVM sectors
    BOOL ERASE_EXT_CMD              ; //Erase from one to all the NVM sectors
	BOOL WRITE_PROTECT_CMD          ; //Enable the write protection in a permanent way for some sectors
    BOOL WRITE_TEMP_UNPROTECT_CMD   ; //Disable the write protection in a temporary way for all NVM sectors
    BOOL WRITE_PERM_UNPROTECT_CMD   ; //Disable the write protection in a permanent way for all NVM sectors
    BOOL READOUT_PROTECT_CMD   ; //Enable the readout protection in a permanent way
    BOOL READOUT_TEMP_UNPROTECT_CMD ; //Disable the readout protection in a temporary way
    BOOL READOUT_PERM_UNPROTECT_CMD ; //Disable the readout protection in a permanent way
}* LPCommands;

typedef struct TARGET_DESCRIPTOR
{
	BYTE Version                    ;
	BYTE CmdCount                   ;
	BYTE PIDLen                     ;
	BYTE* PID                       ;

	BYTE ROPE                       ;
	BYTE ROPD                       ;

	BOOL GET_CMD                    ; //Get the version and the allowed commands supported by the current version of the boot loader
    BOOL GET_VER_ROPS_CMD           ; //Get the BL version and the Read Protection status of the NVM
    BOOL GET_ID_CMD                 ; //Get the chip ID
    BOOL SET_SPEED_CMD              ;
	BOOL READ_CMD                   ; //Read up to 256 bytes of memory starting from an address specified by the user
    BOOL GO_CMD                     ; //Jump to an address specified by the user to execute (a loaded) code
    BOOL WRITE_CMD                  ; //Write maximum 256 bytes to the RAM or the NVM starting from an address specified by the user
    BOOL ERASE_CMD                  ; //Erase from one to all the NVM sectors
	BOOL ERASE_EXT_CMD                  ; //Erase from one to all the NVM sectors
    BOOL WRITE_PROTECT_CMD          ; //Enable the write protection in a permanent way for some sectors
    BOOL WRITE_TEMP_UNPROTECT_CMD   ; //Disable the write protection in a temporary way for all NVM sectors
    BOOL WRITE_PERM_UNPROTECT_CMD   ; //Disable the write protection in a permanent way for all NVM sectors
    BOOL READOUT_PERM_PROTECT_CMD   ; //Enable the readout protection in a permanent way
    BOOL READOUT_TEMP_UNPROTECT_CMD ; //Disable the readout protection in a temporary way
    BOOL READOUT_PERM_UNPROTECT_CMD ; //Disable the readout protection in a permanent way
}* LPTARGET_DESCRIPTOR;

typedef struct STBL_Request
{
	BYTE     _cmd;
	DWORD    _address;
	WORD     _length;
	BYTE     _nbSectors;
    LPTARGET_DESCRIPTOR _target;
	LPBYTE   _data;
	WORD     _wbSectors;
}* LPSTBL_Request; 


BYTE STBL_Init_BL();

#endif