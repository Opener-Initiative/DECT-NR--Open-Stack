/*
 * This code is property of Ostfalia University of Applied Sciences, Germany and Universidad Politécnica de Cartagena, Spain. 
 * Relations with QARTIA Smart Technologies, any inquiries please contact author. 
 * Based on ETSI TS 103 636-4 V1.5.1
 * Author: Iván Tomás García <ivan.tomas@edu.upct.es>
 */

#include <stdint.h>

#ifndef MAC_INFO_ELEMENTS_H
#define MAC_INFO_ELEMENTS_H


/**
 * @brief MAC Security Info IE (6.4.3.1)
 * 
 * @param Version (2 bits)
 * @param Key_Index (2 bits)
 * @param Sec_IV_Type (4 bits)
 * @param HPC (32 bits)
 * 
 * @param mlie_1 (5 Bytes)
 */
typedef struct {

	uint8_t	Version  		: 2;
	uint8_t	Key_Index		: 2;
	uint8_t	Sec_IV_Type		: 4;
	uint32_t	HPC			: 32;

	uint8_t	mlie_1[5];
}mlie_1_t;


/**
 * @brief Route Info IE (6.4.3.2)
 * 
 * @param Sink_Address (32 bits)
 * @param Route_Cost (8 bits)
 * @param App_Sec_N (8 bits)
 * 
 * @param mlie_2 (6 Bytes)
 */
typedef struct {

	uint32_t	Sink_Address	: 32;
	uint8_t	Route_Cost			: 8;
	uint8_t	App_Sec_N			: 8;

	uint8_t	mlie_2[6];
}mlie_2_t;


/**
 * @brief Resource Allocation IE (6.4.3.3)
 * 
 * @param Allocation_Type (2 bits)
 * @param Add (1 bit)
 * @param ID (1 bit)
 * @param Repeat (3 bits)
 * @param SFN (1 bit)
 * @param Channel (1 bit)
 * @param RLF (1 bit)
 * @param Reserved (7 bits)
 * @param Start_Subslot (9 bits)
 * @param Length_Type (1 bit)
 * @param Length (7 bits)
 * @param Reserved_1 (7 bits)
 * @param Start_Subslot_1 (9 bits)
 * @param Length_Type_1 (1 bit)
 * @param Length_1 (7 bits)
 * @param Short_RD_ID (16 bits)
 * @param Repetition (8 bits)
 * @param Validity (8 bits)
 * @param SFN_Value (8 bits)
 * @param Reserved_2 (3 bits)
 * @param Channel_1 (13 bits)
 * @param Reserved_3 (4 bits)
 * @param dect_Scheduled_Resource_Failure (4 bits)
 * 
 * @param mlie_3 (16 Bytes)
 */
typedef struct {

	uint8_t	Allocation_Type		: 2;
	uint8_t	Add					: 1;
	uint8_t	ID					: 1;
	uint8_t	Repeat				: 3;
	uint8_t SFN					: 1;
	uint8_t Channel				: 1;
	uint8_t RLF					: 1;
	uint8_t Reserved			: 7;
	uint16_t Start_Subslot		: 9;
	uint8_t Length_Type			: 1;
	uint8_t Length				: 7;
	uint8_t Reserved_1			: 7;	
	uint16_t Start_Subslot_1	: 9;
	uint8_t Length_Type_1		: 1;
	uint8_t Length_1			: 7;
	uint16_t Short_RD_ID		: 16;
	uint8_t Repetition			: 8;
	uint8_t Validity			: 8;
	uint8_t SFN_Value			: 8;
	uint8_t Reserved_2			: 3;
	uint16_t Channel_1			: 13;
	uint8_t Reserved_3			: 4;
	uint8_t dect_Scheduled_Resource_Failure	: 4;

	uint8_t	mlie_3[16];

}mlie_3_t;

/**
 * @brief Random Access Resource IE (6.4.3.4)
 * 
 * @param Reserved (3 bits)
 * @param Repeat (2 bits)
 * @param SFN (1 bit)
 * @param Channel (1 bit)
 * @param Channel2 (1 bit)
 * @param Reserved_1 (7 bits)
 * @param Start_Subslot (1 bit)
 * @param Length_Type (1 bit)
 * @param Length (7 bits)
 * @param MAX_Length_Type (1 bit)
 * @param MAX_RACH_Length (4 bits)
 * @param CW_Min_Sig (3 bits)
 * @param DECT_Delay (1 bit)
 * @param Response_Window (4 bits)
 * @param CW_Max_Sig (3 bits)
 * @param Repetition (8 bits)
 * @param Validity (8 bits)
 * @param SNF_Value (8 bits)
 * @param Reserved_2 (3 bits)
 * @param Channel_1 (13 bits)
 * @param Reserved_3 (3 bits)
 * @param Channel_2 (13 bits)
 * 
 * @param mlie_4 (13 Bytes)
 */
typedef struct {

	uint8_t	Reserved		: 3;
	uint8_t	Repeat			: 2;
	uint8_t	SFN				: 1;
	uint8_t	Channel			: 1;
	uint8_t Channel2		: 1;
	uint8_t Reserved_1		: 7;
	uint16_t Start_Subslot	: 1;
	uint8_t Length_Type		: 1;
	uint8_t Length			: 7;
	uint8_t MAX_Length_Type	: 1;
	uint8_t MAX_RACH_Length	: 4;
	uint8_t CW_Min_Sig		: 3;
	uint8_t DECT_Delay		: 1;
	uint8_t Response_Window	: 4;
	uint8_t CW_Max_Sig		: 3;
	uint8_t Repetition		: 8;
	uint8_t Validity		: 8;
	uint8_t SNF_Value		: 8;
	uint8_t Reserved_2		: 3;
	uint16_t Channel_1		: 13;
	uint8_t Reserved_3		: 3;
	uint16_t Channel_2		: 13;

	uint8_t	mlie_4[13];
}mlie_4_t;

/**
 * @brief RD Capability IE (6.4.3.5)
 * 
 * @param N_PHY_Cap (3 bits)
 * @param Release (5 bits)
 * @param Reserved (2 bits)
 * @param Group_As (1 bit)
 * @param Paging (1 bit)
 * @param Op_Modes (2 bits)
 * @param Mesh (1 bit)
 * @param Schedul (1 bit)
 * @param MAC_Security (3 bits)
 * @param DLC_Service_Type (3 bits)
 * @param Reserved_1 (2 bits)
 * @param RD_Power_Class (3 bits)
 * @param MAX_NSS_RX (2 bits)
 * @param RX_TX_Diversity (2 bits)
 * @param RX_Gain (4 bits)
 * @param Max_MCS (4 bits)
 * @param Softbuffer_Size (4 bits)
 * @param N_HARQ_Process (2 bits)
 * @param Reserved_2 (2 bits)
 * @param HARQ_Feedback_delay (4 bits)
 * @param D_Delay (1 bit)
 * @param HalfDup (1 bit)
 * @param Reserved_3 (2 bits)
 * @param mu (3 bits)
 * @param beta (4 bits)
 * @param Reserved_4 (2 bits)
 * @param RD_Power_Class_1 (3 bits)
 * @param MAX_NSS_RX_1 (2 bits)
 * @param RX_TX_Diversity_1 (2 bits)
 * @param RX_Gain_1 (4 bits)
 * @param MAX_MCS_1 (4 bits)
 * @param Softbuffer_Size_1 (4 bits)
 * @param N_HARQ_Process_1 (2 bits)
 * @param Reserved_5 (2 bits)
 * @param HARQ_Feedback_delay_1 (4 bits)
 * @param Reserved_6 (4 bits)
 * 
 * @param mlie_5 (12 Bytes)
 */
typedef struct {

	uint8_t	N_PHY_Cap			: 3;
	uint8_t	Release				: 5;
	uint8_t	Reserved			: 2;
	uint8_t Group_As			: 1;
	uint8_t Paging				: 1;
	uint8_t	Op_Modes			: 2;
	uint8_t Mesh				: 1;	
	uint8_t Schedul				: 1;
	uint8_t MAC_Security		: 3;
	uint8_t DLC_Service_Type	: 3;
	uint8_t Reserved_1			: 2;
	uint8_t RD_Power_Class		: 3;
	uint8_t MAX_NSS_RX			: 2;
	uint8_t RX_TX_Diversity		: 2;
	uint8_t RX_Gain				: 4;
	uint8_t Max_MCS				: 4;
	uint8_t Softbuffer_Size		: 4;
	uint8_t N_HARQ_Process		: 2;
	uint8_t Reserved_2			: 2;
	uint8_t HARQ_Feedback_delay	: 4;
	uint8_t D_Delay				: 1;
	uint8_t HalfDup				: 1;
	uint8_t Reserved_3			: 2;
	uint8_t mu					: 3;
	uint8_t beta				: 4;
	uint8_t Reserved_4			: 2;
	uint8_t RD_Power_Class_1	: 3;
	uint8_t MAX_NSS_RX_1		: 2;
	uint8_t RX_TX_Diversity_1	: 2;
	uint8_t RX_Gain_1			: 4;
	uint8_t MAX_MCS_1			: 4;
	uint8_t Softbuffer_Size_1	: 4;
	uint8_t N_HARQ_Process_1	: 2;
	uint8_t Reserved_5			: 2;
	uint8_t HARQ_Feedback_delay_1	: 4;
	uint8_t Reserved_6			:4;

	uint8_t	mlie_5[12];
}mlie_5_t;

/**
 * @brief Neighbouring IE (6.4.3.6)
 * 
 * @param Reserved (1 bit)
 * @param ID (1 bit)
 * @param mu (1 bit)
 * @param SNR (1 bit)
 * @param RSSI_2 (1 bit)
 * @param Power_Const (1 bit)
 * @param Next_Channel (1 bit)
 * @param TTN (1 bit)
 * @param Network_Beacon_Period (4 bits)
 * @param Cluster_Beacon_Period (4 bits)
 * @param Long_RD_ID (32 bits)
 * @param Reserved_1 (4 bits)
 * @param Next_Cluster_Channel (12 bits)
 * @param TTN_1 (32 bits)
 * @param RSSI_2_1 (8 bits)
 * @param SNR_1 (8 bits)
 * @param Radio_Device_Class_mu (3 bits)
 * @param Radio_Device_Class_beta (4 bits)
 * @param Reserved_2 (1 bit)
 * 
 * @param mlie_6 (18 Bytes)
 */
typedef struct {

	uint8_t	Reserved				: 1;
	uint8_t	ID						: 1;
	uint8_t	mu						: 1;
	uint8_t	SNR						: 1;
	uint8_t RSSI_2					: 1;
	uint8_t Power_Const				: 1;
	uint8_t Next_Channel			: 1;
	uint8_t TTN						: 1;
	uint8_t	Network_Beacon_Period	: 4;
	uint8_t	Cluster_Beacon_Period	: 4;
	uint32_t Long_RD_ID				: 32;
	uint8_t Reserved_1				: 4;
	uint16_t	Next_Cluster_Channel	: 12;
	uint32_t	TTN_1				: 32;
	uint8_t RSSI_2_1				: 8;
	uint8_t SNR_1					: 8;
	uint8_t Radio_Device_Class_mu	: 3;
	uint8_t	Radio_Device_Class_beta	: 4;
	uint8_t	Reserved_2				: 1;

	uint8_t	mlie_6[18];
}mlie_6_t;

/**
 * @brief Broadcast Indication IE (6.4.3.7)
 * 
 * @param Indication_Type (3 bits)
 * @param IDType (1 bit)
 * @param ACK_NACK (1 bit)
 * @param Feedback (2 bits)
 * @param Resource_Allocation (1 bit)
 * @param LongShort_RDID (16 bits)
 * @param MCS_MIMO_Feedback (8 bits)
 * 
 * @param mlie_7 (4 Bytes)
 */
typedef struct {

	uint8_t	Indication_Type		: 3;
	uint8_t	IDType				: 1;
	uint8_t	ACK_NACK			: 1;
	uint8_t	Feedback			: 2;
	uint8_t Resource_Allocation	: 1;
	uint16_t LongShort_RDID		: 16;
	uint8_t MCS_MIMO_Feedback	: 8;

	uint8_t	mlie_7[4];
}mlie_7_t;


/**
 * @brief Group Assignment IE (6.4.3.9)
 * 
 * @param Single (1 bit)
 * @param Group_ID (7 bits)
 * @param Direct (1 bit)
 * @param ResourceTag (7 bits)
 * @param Direct1 (1 bit)
 * @param ResourceTag1 (7 bits)
 * @param Direct2 (1 bit)
 * @param ResourceTag2 (7 bits)
 * 
 * @param mlie_9 (4 Bytes)
 */
typedef struct {

	uint8_t Single			: 1;
	uint8_t Group_ID		: 7;
	uint8_t Direct			: 1;
	uint8_t ResourceTag		: 7;
	uint8_t Direct1			: 1;
	uint8_t ResourceTag1	: 7;
	uint8_t Direct2			: 1;
	uint8_t ResourceTag2	: 7;

	uint8_t	mlie_9[4];


}mlie_9_t;

/**
 * @brief Load Info IE (6.4.3.10)
 * 
 * @param Reserved (4 bits)
 * @param MAX_Assoc (1 bit)
 * @param RD_PT_Load (1 bit)
 * @param RACH_Load (1 bit)
 * @param Channel_Load (1 bit)
 * @param Traffic_Load_Percentage (8 bits)
 * @param MAX_n_Assoc_RDs (16 bits)
 * @param Associated_RDs_FT (8 bits)
 * @param Associated_RDs_PT (8 bits)
 * @param RACH_Load_percentage (8 bits)
 * @param Free_subslots_detected (8 bits)
 * @param Busy_subslots_detected (8 bits)
 * 
 * @param mlie_10 (9 Bytes)
 */
typedef struct {

	uint8_t Reserved				: 4;
	uint8_t MAX_Assoc				: 1;
	uint8_t RD_PT_Load				: 1;
	uint8_t RACH_Load				: 1;
	uint8_t Channel_Load			: 1;
	uint8_t Traffic_Load_Percentage	: 8;	
	uint16_t MAX_n_Assoc_RDs		: 16;
	uint8_t Associated_RDs_FT		: 8;
	uint8_t Associated_RDs_PT		: 8;
	uint8_t RACH_Load_percentage	: 8;
	uint8_t Free_subslots_detected	: 8;
	uint8_t Busy_subslots_detected	: 8;

	uint8_t	mlie_10[9];

}mlie_10_t;


/**
 * @brief Measurement Report IE (6.4.3.12)
 * 
 * @param Reserved (3 bits)
 * @param SNR (1 bit)
 * @param RSSI_2 (1 bit)
 * @param RSSI_1 (1 bit)
 * @param Tx_Count (1 bit)
 * @param RACH (1 bit)
 * @param SNR_Result (8 bits)
 * @param RSSI_2_Result (8 bits)
 * @param RSSI_1_Result (8 bits)
 * @param Tx_Count_Result (8 bits)
 * 
 * @param mlie_12 (5 Bytes)
 */
typedef struct {

	uint8_t	Reserved			: 3;
	uint8_t	SNR					: 1;
	uint8_t RSSI_2				: 1;
	uint8_t RSSI_1				: 1;
	uint8_t Tx_Count			: 1;
	uint8_t RACH				: 1;
	uint8_t SNR_Result			: 8;
	uint8_t RSSI_2_Result		: 8;
	uint8_t RSSI_1_Result		: 8;
	uint8_t Tx_Count_Result		: 8;

	uint8_t	mlie_12[5];

}mlie_12_t;


/**
 * @brief Radio Device Status (6.4.3.13)
 * 
 * @param Reserved (2 bits)
 * @param Status_Flag (2 bits)
 * @param Duration (4 bits)
 * 
 * @param mlie_13 (1 Byte)
 */
typedef struct {

	uint8_t	Reserved	: 2;
	uint8_t	Status_Flag	: 2;
	uint8_t	Duration	: 4;

	uint8_t	mlie_13[1];

}mlie_13_t;



int get_mlie_1(mlie_1_t* mlie_1);
int get_mlie_2(mlie_2_t* mlie_2);
int get_mlie_3(mlie_3_t* mlie_3);
int get_mlie_4(mlie_4_t* mlie_4);
int get_mlie_5(mlie_5_t* mlie_5);
int get_mlie_6(mlie_6_t* mlie_6);
int get_mlie_7(mlie_7_t* mlie_7);
// int get_mlie_8(mlie_8_t* mlie_8);
int get_mlie_9(mlie_9_t* mlie_9);
int get_mlie_10(mlie_10_t* mlie_10);
// int get_mlie_11(mlie_11_t* mlie_11);
int get_mlie_12(mlie_12_t* mlie_12);
int get_mlie_13(mlie_13_t* mlie_13);

int get_mlie_1_rev(mlie_1_t* mlie_1);
int get_mlie_2_rev(mlie_2_t* mlie_2);
int get_mlie_3_rev(mlie_3_t* mlie_3);
int get_mlie_4_rev(mlie_4_t* mlie_4);
int get_mlie_5_rev(mlie_5_t* mlie_5);
int get_mlie_6_rev(mlie_6_t* mlie_6);
int get_mlie_7_rev(mlie_7_t* mlie_7);
// int get_mlie_8_rev(mlie_8_t* mlie_8);
int get_mlie_9_rev(mlie_9_t* mlie_9);
int get_mlie_10_rev(mlie_10_t* mlie_10);
// int get_mlie_11_rev(mlie_11_t* mlie_11);
int get_mlie_12_rev(mlie_12_t* mlie_12);
int get_mlie_13_rev(mlie_13_t* mlie_13);

#endif