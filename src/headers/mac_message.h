/*
 * This code is property of Ostfalia University of Applied Sciences, Germany and Universidad Politécnica de Cartagena, Spain. 
 * Relations with QARTIA Smart Technologies, any inquiries please contact author. 
 * Based on ETSI TS 103 636-4 V1.5.1
 * Author: Iván Tomás García <ivan.tomas@edu.upct.es>
 */

#include <stdint.h>
#ifndef MAC_MESSAGE_H
#define MAC_MESSAGE_H


	//// No mlmf_1_t for notation convenience. Standard designates 6.4.2.1 to general index, to clarify programming relation to 
	//// standard we keep same order.

	/**
	 * @brief Network Beacon Message (6.4.2.2)
	 *
	 * @param Reserved (3 bits)
	 * @param TX_Power (1 bit)
	 * @param Power_Const (1 bit)
	 * @param Current (1 bit)
	 * @param Network_Beacon_Channels (2 bits)
	 * @param Network_Beacon_Period (4 bits)
	 * @param Cluster_Beacon_Period (4 bits)
	 * @param Reserved_1 (3 bits)
	 * @param Next_Cluster_Channel (13 bits)
	 * @param TTn (32 bits) //Time To next
	 * @param Reserved_2 (4 bits)
	 * @param Clusters_Max_TX_Power (4 bits)
	 * @param Reserved_3 (3 bits)
	 * @param Current_Cluster_Channel (13 bits)
	 * @param Reserved_4 (3 bits)
	 * @param ANBC (13 bits) 
	 * 
	 * @param mlmf_2 (13 Bytes)
	 */
	 typedef struct {

		uint8_t	Reserved					: 3;
		uint8_t	TX_Power					: 1;
		uint8_t	Power_Const					: 1;
		uint8_t	Current						: 1;
		uint8_t	Network_Beacon_Channels		: 2;
		uint8_t	Network_Beacon_Period		: 4;
		uint8_t	Cluster_Beacon_Period		: 4;
		uint8_t	Reserved_1					: 3;
		uint16_t	Next_Cluster_Channel	: 13;
		uint32_t	TTn						: 32;	//Time To next
		uint8_t	Reserved_2					: 4;
		uint8_t	Clusters_Max_TX_Power		: 4;
		uint8_t	Reserved_3					: 3;
		uint16_t	Current_Cluster_Channel	: 13;
		uint8_t	Reserved_4					: 3;
		uint16_t	ANBC					: 13;	//Additional_Network_Beacon_Channels
		
		uint8_t	mlmf_2[13];
	}mlmf_2_t;

	/**
	 * @brief Cluster Beacon Message (6.4.2.3)
	 * 
	 * @param SFN (8 bits)
	 * @param Reserved (3 bits)
	 * @param TX_Power (1 bit)
	 * @param Power_Const (1 bit)
	 * @param FO (1 bit)
	 * @param Next_Channel (1 bit)
	 * @param TTN (1 bit)
	 * @param Network_Beacon_Period (4 bits)
	 * @param Cluster_Beacon_Period (4 bits)
	 * @param countToTrigger (4 bits)
	 * @param ReQuality (2 bits)
	 * @param MinQuality (2 bits)
	 * @param Reserved_1 (4 bits)
	 * @param Clusters_Max_TX_Power (4 bits)
	 * @param Frame_Offset (8 bits)
	 * @param Reserved_2 (3 bits)
	 * @param Next_Cluster_Channel (13 bits)
	 * @param TTn (32 bits)
	 * 
	 * @param mlmf_3 (12 Bytes)
	 */
	typedef struct {

		uint8_t	SFN							: 8;
		uint8_t	Reserved					: 3;
		uint8_t	TX_Power					: 1;
		uint8_t	Power_Const					: 1;
		uint8_t	FO							: 1;
		uint8_t Next_Channel				: 1;
		uint8_t	TTN							: 1;
		uint8_t	Network_Beacon_Period		: 4;
		uint8_t	Cluster_Beacon_Period		: 4;
		uint8_t	countToTrigger				: 4;
		uint8_t	ReQuality					: 2;
		uint8_t	MinQuality					: 2;
		uint8_t	Reserved_1					: 4;
		uint8_t	Clusters_Max_TX_Power		: 4;
		uint8_t	Frame_Offset				: 8;
		uint8_t	Reserved_2					: 3;
		uint16_t	Next_Cluster_Channel	: 13;
		uint32_t	TTn						: 32;	//Time To next
		
		uint8_t	mlmf_3[12];
	}mlmf_3_t;


	/**
	 * @brief Association Request Message (6.4.2.4)
	 * 
	 * @param Setup_Cause (3 bits)
	 * @param N_Flows (3 bits)
	 * @param Power_Const (1 bit)
	 * @param FT_Mode (1 bit)
	 * @param Current (1 bit)
	 * @param Reserved (7 bits)
	 * @param HARQ_P_TX (3 bits)
	 * @param MAX_HARQ_Re_TX (5 bits)
	 * @param HARQ_P_RX (3 bits)
	 * @param MAX_HARQ_Re_RX (5 bits)
	 * @param Reserved_1 (2 bits)
	 * @param FlowID (6 bits)
	 * @param Network_Beacon_Period (4 bits)
	 * @param Cluster_Beacon_Period (4 bits)
	 * @param Reserved_2 (3 bits)
	 * @param Next_Cluster_Channel (13 bits)
	 * @param TTn (32 bits) //Time To next
	 * @param Reserved_3 (3 bits)
	 * @param Current_Cluster_Channel (13 bits)
	 * 
	 * @param mlmf_4 (14 Bytes)
	 */
	typedef struct {

		uint8_t	Setup_Cause					: 3;
		uint8_t	N_Flows						: 3;
		uint8_t	Power_Const					: 1;
		uint8_t	FT_Mode						: 1;
		uint8_t Current						: 1;
		uint8_t	Reserved					: 7;
		uint8_t	HARQ_P_TX					: 3;
		uint8_t	MAX_HARQ_Re_TX				: 5;
		uint8_t	HARQ_P_RX					: 3;
		uint8_t	MAX_HARQ_Re_RX				: 5;
		uint8_t	Reserved_1					: 2;
		uint8_t	FlowID						: 6;
		uint8_t	Network_Beacon_Period		: 4;
		uint8_t	Cluster_Beacon_Period		: 4;
		uint8_t	Reserved_2					: 3;
		uint16_t	Next_Cluster_Channel	: 13;
		uint32_t	TTn						: 32;	//Time To next
		uint8_t	Reserved_3					: 3;
		uint16_t	Current_Cluster_Channel	: 13;

		uint8_t	mlmf_4[14];
	}mlmf_4_t;


	/**
	 * @brief Association Response Message (6.4.2.5)
	 * 
	 * @param ACK_NACK (1 bit)
	 * @param Reserved (1 bit)
	 * @param HARQ_mod (1 bit)
	 * @param NFlows (3 bits)
	 * @param Group (1 bit)
	 * @param TX_Power (1 bit)
	 * @param Reject_Cause (4 bits)
	 * @param Reject_Time (4 bits)
	 * @param HARQ_P_RX (3 bits)
	 * @param MAX_HARQ_Re_RX (5 bits)
	 * @param HARQ_P_TX (3 bits)
	 * @param MAX_HARQ_Re_TX (5 bits)
	 * @param Reserved_1 (2 bits)
	 * @param Flow_ID (6 bits)
	 * @param Reserved_2 (1 bit)
	 * @param Group_ID (7 bits)
	 * @param Reserved_3 (1 bit)
	 * @param Resource_TAG (7 bits)
	 * 
	 * @param mlmf_5 (7 Bytes)
	 */
	typedef struct {

		uint8_t	ACK_NACK		: 1;
		uint8_t	Reserved		: 1;
		uint8_t	HARQ_mod		: 1;
		uint8_t	NFlows			: 3;
		uint8_t Group			: 1;
		uint8_t	TX_Power		: 1;
		uint8_t	Reject_Cause	: 4;
		uint8_t	Reject_Time		: 4;
		uint8_t	HARQ_P_RX		: 3;
		uint8_t	MAX_HARQ_Re_RX	: 5;
		uint8_t	HARQ_P_TX		: 3;
		uint8_t	MAX_HARQ_Re_TX	: 5;
		uint8_t	Reserved_1		: 2;
		uint8_t	Flow_ID			: 6;
		uint8_t	Reserved_2		: 1;
		uint8_t	Group_ID		: 7;
		uint8_t	Reserved_3		: 1;	
		uint8_t	Resource_TAG	: 7;

		uint8_t	mlmf_5[7];
	}mlmf_5_t;


	/**
	 * @brief Association Release Message (6.4.2.6)
	 * 
	 * @param Release_Cause (4 bits)
	 * @param Reserved (4 bits)
	 * 
	 * @param mlmf_6 (1 Byte)
	 */
	typedef struct {

		uint8_t	Release_Cause	: 4;
		uint8_t	Reserved		: 4;

		uint8_t	mlmf_6;
	}mlmf_6_t;


	/**
	 * @brief Reconfiguration Request Message (6.4.2.7)
	 * 
	 * @param TX_HARQ (1 bit)
	 * @param RX_HARQ (1 bit)
	 * @param RD_Capability (1 bit)
	 * @param N_Flows (3 bits)
	 * @param Radio_Resource (2 bits)
	 * @param HARQ_P_TX (3 bits)
	 * @param MAX_HARQ_Re_TX (5 bits)
	 * @param HARQ_P_RX (3 bits)
	 * @param MAX_HARQ_Re_RX (5 bits)
	 * @param Setup_Release (1 bit)
	 * @param Reserved (1 bit)
	 * @param Flow_ID (6 bits)
	 * 
	 * @param mlmf_7 (4 Bytes)
	 */
	typedef struct {

		uint8_t	TX_HARQ			: 1;
		uint8_t	RX_HARQ			: 1;
		uint8_t	RD_Capability	: 1;
		uint8_t	N_Flows			: 3;
		uint8_t Radio_Resource	: 2;
		uint8_t HARQ_P_TX		: 3;
		uint8_t	MAX_HARQ_Re_TX	: 5;
		uint8_t	HARQ_P_RX		: 3;
		uint8_t	MAX_HARQ_Re_RX	: 5;
		uint8_t	Setup_Release	: 1;
		uint8_t	Reserved		: 1;
		uint8_t	Flow_ID			: 6;

		uint8_t	mlmf_7[4];
	}mlmf_7_t;


	/**
	 * @brief Reconfiguration Response Message (6.4.2.8)
	 * 
	 * @param TX_HARQ (1 bit)
	 * @param RX_HARQ (1 bit)
	 * @param RD_Capability (1 bit)
	 * @param N_Flows (3 bits)
	 * @param Radio_Resource (2 bits)
	 * @param HARQ_P_TX (3 bits)
	 * @param MAX_HARQ_Re_TX (5 bits)
	 * @param HARQ_P_RX (3 bits)
	 * @param MAX_HARQ_Re_RX (5 bits)
	 * @param Setup_Release (1 bit)
	 * @param Reserved (1 bit)
	 * @param Flow_ID (6 bits)
	 * 
	 * @param mlmf_8 (4 Bytes)
	 */
	typedef struct {

		uint8_t	TX_HARQ			: 1;
		uint8_t	RX_HARQ			: 1;
		uint8_t	RD_Capability	: 1;
		uint8_t	N_Flows			: 3;
		uint8_t Radio_Resource	: 2;
		uint8_t HARQ_P_TX		: 3;
		uint8_t	MAX_HARQ_Re_TX	: 5;
		uint8_t	HARQ_P_RX		: 3;
		uint8_t	MAX_HARQ_Re_RX	: 5;
		uint8_t	Setup_Release	: 1;
		uint8_t	Reserved		: 1;
		uint8_t	Flow_ID			: 6;

		uint8_t	mlmf_8[4];
	}mlmf_8_t;



	int get_mlmf_2(mlmf_2_t* mlmf_2);
	int get_mlmf_3(mlmf_3_t* mlmf_3);
	int get_mlmf_4(mlmf_4_t* mlmf_4);
	int get_mlmf_5(mlmf_5_t* mlmf_5);
	int get_mlmf_6(mlmf_6_t* mlmf_6);
	int get_mlmf_7(mlmf_7_t* mlmf_7);
	int get_mlmf_8(mlmf_8_t* mlmf_8);

	int get_mlmf_2_rev(mlmf_2_t* mlmf_2);
	int get_mlmf_3_rev(mlmf_3_t* mlmf_3);
	int get_mlmf_4_rev(mlmf_4_t* mlmf_4);
	int get_mlmf_5_rev(mlmf_5_t* mlmf_5);
	int get_mlmf_6_rev(mlmf_6_t* mlmf_6);
	int get_mlmf_7_rev(mlmf_7_t* mlmf_7);
	int get_mlmf_8_rev(mlmf_8_t* mlmf_8);



#endif 