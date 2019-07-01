#ifndef _RDM_CONFIG_H
#define _RDM_CONFIG_H


#define SUPPORT_RDM	 1

/*******************************************************************************
 * Types and defines
 ******************************************************************************/

//#define VALIDATE_PERFORMANCE_ON_MAX_DMX_BUS_LOAD
//#define MAN_ID    ((32*32*('N'-'@')) + (32*('X'-'@')) + ('P'-'@'))
#define MAN_ID    (0x5880)

#define DEVICE_MODEL_ID1             0x00  /* MSB */
#define DEVICE_MODEL_ID0             0x01  /* LSB */

#define SW_VERSION_ID3               0x01  /* MSB */
#define SW_VERSION_ID2               0x00
#define SW_VERSION_ID1               0x00
#define SW_VERSION_ID0               0x03  /* LSB */

#define FIRMWARE_VERSION    0x0201

#define DMX512_FOOTPRINT             4     /* nr of slots this device uses */

#define SC_NULL                      0x00  /* NULL Start Code Packet */
#define SC_TEXT                      0x17  /* Start Code ASCII Text Packet */
#define SC_RDM                       0xCC  /* Start Code RDM Packet */

#define SC_SUB_MESSAGE               0x01

// RDM command classes (Slot 20)
#define DISCOVERY_COMMAND            0x10
#define DISCOVERY_COMMAND_RESPONSE   0x11
#define GET_COMMAND                  0x20
#define GET_COMMAND_RESPONSE         0x21
#define SET_COMMAND                  0x30
#define SET_COMMAND_RESPONSE         0x31

#define RESPONSE_TYPE_ACK            0x00
#define RESPONSE_TYPE_ACK_TIMER      0x01
#define RESPONSE_TYPE_NACK_REASON    0x02
#define RESPONSE_TYPE_ACK_OVERFLOW   0x03

#define DISC_UNIQUE_BRANCH           0x0001
#define DISC_MUTE                    0x0002
#define DISC_UN_MUTE                 0x0003
#define DEVICE_INFO                  0x0060
#define SOFTWARE_VERSION_LABEL       0x00C0
#define DMX_START_ADDRESS            0x00F0
#define IDENTIFY_DEVICE              0x1000

typedef enum 
{
        TIMER_DISCOVERY_START=(unsigned char)0,
        TIMER_DISCOVERY_CONTINU,
        TIMER_DISCOVERY_MUTE,
		TIMER_DISCOVERY_UNMUTE,
		TIMER_SEARCH_RANGE,
		TIMER_GET_DEVICE_INFO,
		TIMER_NULL,
}Timer_Type;

typedef struct 	
{ 
	void(*Master)(void);
	void(*Update_Address)(void);
	void(*Update_PanInverseYes)(void);
	void(*Update_PanInverseNo)(void);
	void(*Update_TiltInverseYes)(void);
	void(*Update_TiltInverseNo)(void);
	void(*Update_RotaR)(void);
	void(*Update_RotaN)(void);
	void(*Update_RotaA)(void);
	void(*Update_ResetYes)(void);
	void(*Update_Channel1)(void);
	void(*Update_Channel2)(void);
	unsigned char *rdm_msg;
	__int64 
	my_unique_id,
	my_uid;
	unsigned short  
	break_time,
	mab_time,
	CRC_SUM,
	count;
	unsigned char   
	rdm_message_count,
	RDMTxCounter,
	RDMTransmitFinish,
	RDMFinish,
	dmx_xmit_data_len,
	identify_state,        // off=0, on=1
	rdm_response_msg[255],
	rdm_Recv_msg[255];
	char buf[3];
}RDM_ST;

typedef struct 	
{ 
	__int64 
	upper_bound,
	lower_bound,
	To_uid;
	unsigned short  
	CRC_SUM;	
	int
	rdm_device_cnt,	   //0
	discovery_retry_cnt,
	selected_device,//-1 <0 means no device selected, >=0 means device selected
	timer_Count;
	Timer_Type
	timer_Mode;
	unsigned char
	Rdm_Updatetab,
	rdm_msg_len,
	timer_Enabled,
	data_count,
	device_count,
	Received_state,
	Received_type,
	rdm_discovery_busy,	 //0
	rdm_find_new_devices,//0
	line_activity,
	single_device_found,//0
	transaction_number,
	my_unique_id[4],
	rdm_msg[255],
	rdm_packet_req[513],
	footprint_buf[20],
	softwareid_buf[20][4],
	Personality_buf[20][2];
	__int64 
	uid_buf[20];
	unsigned short
	address_buf[20];
}SRDM_ST;


extern SRDM_ST srdm;
extern void buttonDiscover_Click(SRDM_ST *SRDM);
extern void buttonUnmute_Click(SRDM_ST *SRDM);
extern void srdm_set_new_range(unsigned char retry_same,SRDM_ST *SRDM);
extern void srdm_search_range(SRDM_ST *SRDM);
extern void srdm_start_search(SRDM_ST *SRDM);
extern void srdm_send_disc_mute_cmd(SRDM_ST *SRDM);
extern void srdm_send_disc_un_mute_cmd(SRDM_ST *SRDM);
extern void srdm_send_identify_cmd(SRDM_ST *SRDM,unsigned char value);
extern void srdm_Received_Packet(SRDM_ST *SRDM,unsigned char ch);

extern void srdm_send_device_info_cmd(SRDM_ST *SRDM);

#endif /* _RDM_CONFIG_H */


