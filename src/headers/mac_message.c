/*
 * This code is property of Ostfalia University of Applied Sciences, Germany and Universidad Politécnica de Cartagena, Spain. 
 * Relations with QARTIA Smart Technologies, any inquiries please contact author. 
 * Based on ETSI TS 103 636-4 V1.5.1
 * Author: Iván Tomás García <ivan.tomas@edu.upct.es>
 */


#include <string.h>
#include "mac_message.h"

	//// No mlmf_1_t for notation convenience. Standard designates 6.4.2.1 to general index, to clarify programming relation to 
	//// standard we keep same order.

	////			PACKING METHODS

	int get_mlmf_2(mlmf_2_t* mlmf_2) {    //Network Beacon Message (6.4.2.2)

		mlmf_2->mlmf_2[0] = (mlmf_2->mlmf_2[0] & 0x1F) | (mlmf_2->Reserved << 5);
		mlmf_2->mlmf_2[0] = (mlmf_2->mlmf_2[0] & 0xEF) | (mlmf_2->TX_Power << 4);
		mlmf_2->mlmf_2[0] = (mlmf_2->mlmf_2[0] & 0xF7) | (mlmf_2->Power_Const << 3);
		mlmf_2->mlmf_2[0] = (mlmf_2->mlmf_2[0] & 0xFB) | (mlmf_2->Current << 2);
		mlmf_2->mlmf_2[0] = (mlmf_2->mlmf_2[0] & 0xFC) | mlmf_2->Network_Beacon_Channels;
		mlmf_2->mlmf_2[1] = (mlmf_2->mlmf_2[1] & 0x0F) | (mlmf_2->Network_Beacon_Period << 4);
		mlmf_2->mlmf_2[1] = (mlmf_2->mlmf_2[1] & 0xF0) | mlmf_2->Cluster_Beacon_Period;
		mlmf_2->mlmf_2[2] = (mlmf_2->mlmf_2[2] & 0x1F) | (mlmf_2->Reserved_1 << 5);
		mlmf_2->mlmf_2[2] = (uint8_t) (mlmf_2->mlmf_2[2] & 0xE0) | (mlmf_2->Next_Cluster_Channel >> 8);
		mlmf_2->mlmf_2[3] = (uint8_t) (mlmf_2->Next_Cluster_Channel & 0xFF);
		mlmf_2->mlmf_2[4] = (uint8_t) 0xFF & (mlmf_2->TTn >> 24);
		mlmf_2->mlmf_2[5] = (uint8_t) 0xFF & (mlmf_2->TTn >> 16);
		mlmf_2->mlmf_2[6] = (uint8_t) 0xFF & (mlmf_2->TTn >> 8);
		mlmf_2->mlmf_2[7] = (uint8_t) (0xFF & mlmf_2->TTn);
		mlmf_2->mlmf_2[8] = (mlmf_2->mlmf_2[8] & 0x0F) | (mlmf_2->Reserved_2 << 4);
		mlmf_2->mlmf_2[8] = (mlmf_2->mlmf_2[8] & 0xF0) | mlmf_2->Clusters_Max_TX_Power;
		mlmf_2->mlmf_2[9] = (mlmf_2->mlmf_2[9] & 0x1F) | (mlmf_2->Reserved_3 << 5);
		mlmf_2->mlmf_2[9] = (uint8_t) (mlmf_2->mlmf_2[9] & 0xE0) | (mlmf_2->Current_Cluster_Channel >> 8);
		mlmf_2->mlmf_2[10] = (uint8_t) (0xFF & mlmf_2->Current_Cluster_Channel);
		mlmf_2->mlmf_2[11] = (mlmf_2->mlmf_2[8] & 0x1F) | (mlmf_2->Reserved_4);
		mlmf_2->mlmf_2[11] = (mlmf_2->mlmf_2[2] & 0xE0) | (mlmf_2->ANBC << 5);
		mlmf_2->mlmf_2[12] = (uint8_t) (0xFF & mlmf_2->ANBC);

		return 0;
	}

	int get_mlmf_3(mlmf_3_t* mlmf_3) {    //Cluster Beacon Message (6.4.2.3)

		mlmf_3->mlmf_3[0] = mlmf_3->SFN;
		mlmf_3->mlmf_3[1] = (mlmf_3->mlmf_3[0] & 0x1F) | (mlmf_3->Reserved << 5);
		mlmf_3->mlmf_3[1] = (mlmf_3->mlmf_3[1] & 0xEF) | (mlmf_3->TX_Power << 4);
		mlmf_3->mlmf_3[1] = (mlmf_3->mlmf_3[1] & 0xF7) | (mlmf_3->Power_Const << 3);
		mlmf_3->mlmf_3[1] = (mlmf_3->mlmf_3[1] & 0xFB) | (mlmf_3->FO << 2);
		mlmf_3->mlmf_3[1] = (mlmf_3->mlmf_3[1] & 0xFD) | (mlmf_3->Next_Channel << 1);
		mlmf_3->mlmf_3[1] = (mlmf_3->mlmf_3[1] & 0xFE) | mlmf_3->TTN;
		mlmf_3->mlmf_3[2] = (mlmf_3->mlmf_3[2] & 0x0F) | (mlmf_3->Network_Beacon_Period << 4);
		mlmf_3->mlmf_3[2] = (mlmf_3->mlmf_3[2] & 0xF0) | mlmf_3->Cluster_Beacon_Period;
		mlmf_3->mlmf_3[3] = (mlmf_3->mlmf_3[3] & 0x0F) | (mlmf_3->countToTrigger << 4);
		mlmf_3->mlmf_3[3] = (mlmf_3->mlmf_3[3] & 0xF3) | (mlmf_3->ReQuality << 2);
		mlmf_3->mlmf_3[3] = (mlmf_3->mlmf_3[3] & 0xFC) | mlmf_3->MinQuality;
		mlmf_3->mlmf_3[4] = (mlmf_3->mlmf_3[4] & 0x0F) | (mlmf_3->Reserved_1 << 4);
		mlmf_3->mlmf_3[4] = (mlmf_3->mlmf_3[4] & 0xF0) | mlmf_3->Clusters_Max_TX_Power;
		mlmf_3->mlmf_3[5] = mlmf_3->Frame_Offset;
		mlmf_3->mlmf_3[6] = (mlmf_3->mlmf_3[6] & 0x0F) | (mlmf_3->Reserved_2 << 4);
		mlmf_3->mlmf_3[6] = (uint8_t) (mlmf_3->mlmf_3[6] & 0xF0) | (mlmf_3->Next_Cluster_Channel >> 8);
		mlmf_3->mlmf_3[7] = (uint8_t) (0xFF & mlmf_3->Next_Cluster_Channel);
		mlmf_3->mlmf_3[8] = (uint8_t)0xFF & (mlmf_3->TTn >> 24);
		mlmf_3->mlmf_3[9] = (uint8_t)0xFF & (mlmf_3->TTn >> 16);
		mlmf_3->mlmf_3[10] = (uint8_t) (0xFF & (mlmf_3->TTn >> 8));
		mlmf_3->mlmf_3[11] = (uint8_t) (0xFF & mlmf_3->TTn);

		return 0;
	}

	int get_mlmf_4(mlmf_4_t* mlmf_4) {	//Association Request Message IE (6.4.2.4)

		mlmf_4->mlmf_4[0] = (mlmf_4->mlmf_4[0] & 0x1F) | (mlmf_4->Setup_Cause << 5);
		mlmf_4->mlmf_4[0] = (mlmf_4->mlmf_4[0] & 0xE3) | (mlmf_4->N_Flows << 2);
		mlmf_4->mlmf_4[0] = (mlmf_4->mlmf_4[0] & 0xFD) | (mlmf_4->Power_Const << 1);
		mlmf_4->mlmf_4[0] = (mlmf_4->mlmf_4[0] & 0xFE) | mlmf_4->FT_Mode;
		mlmf_4->mlmf_4[1] = (mlmf_4->mlmf_4[1] & 0x7F) | (mlmf_4->Current << 7);
		mlmf_4->mlmf_4[1] = (mlmf_4->mlmf_4[1] & 0x80) | mlmf_4->Reserved;
		mlmf_4->mlmf_4[2] = (mlmf_4->mlmf_4[2] & 0x1F) | (mlmf_4->HARQ_P_TX << 5);
		mlmf_4->mlmf_4[2] = (mlmf_4->mlmf_4[2] & 0x70) | mlmf_4->MAX_HARQ_Re_TX;
		mlmf_4->mlmf_4[3] = (mlmf_4->mlmf_4[3] & 0x1F) | (mlmf_4->HARQ_P_RX << 5);
		mlmf_4->mlmf_4[3] = (mlmf_4->mlmf_4[3] & 0x70) | mlmf_4->MAX_HARQ_Re_RX;
		mlmf_4->mlmf_4[4] = (mlmf_4->mlmf_4[4] & 0x3F) | (mlmf_4->Reserved_1 << 6);
		mlmf_4->mlmf_4[4] = (mlmf_4->mlmf_4[4] & 0xC0) | mlmf_4->FlowID;
		mlmf_4->mlmf_4[5] = (mlmf_4->mlmf_4[5] & 0x0F) | (mlmf_4->Network_Beacon_Period << 4);
		mlmf_4->mlmf_4[5] = (mlmf_4->mlmf_4[5] & 0xF0) | mlmf_4->Cluster_Beacon_Period;
		mlmf_4->mlmf_4[6] = (mlmf_4->mlmf_4[6] & 0x1F) | (mlmf_4->Reserved_2 << 5);
		mlmf_4->mlmf_4[6] = (uint8_t) (mlmf_4->mlmf_4[6] & 0xE0) | (mlmf_4->Next_Cluster_Channel >> 8);
		mlmf_4->mlmf_4[7] = (uint8_t) (0xFF & mlmf_4->Next_Cluster_Channel);
		mlmf_4->mlmf_4[8] = (uint8_t) 0xFF & (mlmf_4->TTn >> 24);
		mlmf_4->mlmf_4[9] = (uint8_t) 0xFF & (mlmf_4->TTn >> 16);
		mlmf_4->mlmf_4[10] = (uint8_t) 0xFF & (mlmf_4->TTn >> 8);
		mlmf_4->mlmf_4[11] = (uint8_t) (0xFF & mlmf_4->TTn);
		mlmf_4->mlmf_4[12] = (mlmf_4->Reserved_3 << 5);
		mlmf_4->mlmf_4[12] = (uint8_t) (mlmf_4->mlmf_4[12] & 0xE0) | (mlmf_4->Current_Cluster_Channel >> 8);
		mlmf_4->mlmf_4[13] = (uint8_t) (0xFF & mlmf_4->Current_Cluster_Channel);


		return 0;
	}

	int get_mlmf_5(mlmf_5_t* mlmf_5) {	//Association Response Message (6.4.2.5)

		mlmf_5->mlmf_5[0] = (mlmf_5->mlmf_5[0] & 0x7F) | (mlmf_5->ACK_NACK << 7);
		mlmf_5->mlmf_5[0] = (mlmf_5->mlmf_5[0] & 0xBF) | (mlmf_5->Reserved << 6);
		mlmf_5->mlmf_5[0] = (mlmf_5->mlmf_5[0] & 0xDF) | (mlmf_5->HARQ_mod << 5);
		mlmf_5->mlmf_5[0] = (mlmf_5->mlmf_5[0] & 0xE3) | (mlmf_5->NFlows << 2);
		mlmf_5->mlmf_5[0] = (mlmf_5->mlmf_5[0] & 0xFD) | (mlmf_5->Group << 1);
		mlmf_5->mlmf_5[0] = (mlmf_5->mlmf_5[0] & 0xFE) | mlmf_5->TX_Power;
		mlmf_5->mlmf_5[1] = (mlmf_5->mlmf_5[1] & 0x0F) | (mlmf_5->Reject_Cause << 4);
		mlmf_5->mlmf_5[1] = (mlmf_5->mlmf_5[1] & 0xF0) | mlmf_5->Reject_Time;
		mlmf_5->mlmf_5[2] = (mlmf_5->mlmf_5[2] & 0x1F) | (mlmf_5->HARQ_P_RX << 5);
		mlmf_5->mlmf_5[2] = (mlmf_5->mlmf_5[2] & 0x70) | mlmf_5->MAX_HARQ_Re_RX;
		mlmf_5->mlmf_5[3] = (mlmf_5->mlmf_5[3] & 0x1F) | (mlmf_5->HARQ_P_TX << 5);
		mlmf_5->mlmf_5[3] = (mlmf_5->mlmf_5[3] & 0x70) | mlmf_5->MAX_HARQ_Re_TX;
		mlmf_5->mlmf_5[4] = (mlmf_5->mlmf_5[4] & 0x3F) | (mlmf_5->Reserved_1 << 6);
		mlmf_5->mlmf_5[4] = (mlmf_5->mlmf_5[4] & 0xC0) | mlmf_5->Flow_ID;
		mlmf_5->mlmf_5[5] = (mlmf_5->mlmf_5[5] & 0x7F) | (mlmf_5->Reserved_2 << 7);
		mlmf_5->mlmf_5[5] = (mlmf_5->mlmf_5[5] & 0x80) | mlmf_5->Group_ID;
		mlmf_5->mlmf_5[6] = (mlmf_5->mlmf_5[6] & 0x7F) | (mlmf_5->Reserved_3 << 7);
		mlmf_5->mlmf_5[6] = (mlmf_5->mlmf_5[6] & 0x80) | mlmf_5->Resource_TAG;


		return 0;
	}


	int get_mlmf_6(mlmf_6_t* mlmf_6) {    //Association Release Message (6.4.2.6)
		mlmf_6->mlmf_6 = (mlmf_6->mlmf_6 & 0x0F) | (mlmf_6->Release_Cause << 4);
		mlmf_6->mlmf_6 = (mlmf_6->mlmf_6 & 0xF0) | mlmf_6->Reserved;

		return 0;
	}


	int get_mlmf_7(mlmf_7_t* mlmf_7) {	//Reconfiguration Request Message (6.4.2.7)

		mlmf_7->mlmf_7[0] = (mlmf_7->mlmf_7[0] & 0x7F) | (mlmf_7->TX_HARQ << 7);
		mlmf_7->mlmf_7[0] = (mlmf_7->mlmf_7[0] & 0xBF) | (mlmf_7->RX_HARQ << 6);
		mlmf_7->mlmf_7[0] = (mlmf_7->mlmf_7[0] & 0xDF) | (mlmf_7->RD_Capability << 5);
		mlmf_7->mlmf_7[0] = (mlmf_7->mlmf_7[0] & 0xE3) | (mlmf_7->N_Flows << 2);
		mlmf_7->mlmf_7[0] = (mlmf_7->mlmf_7[0] & 0xFD) | mlmf_7->Radio_Resource;
		mlmf_7->mlmf_7[1] = (mlmf_7->mlmf_7[1] & 0x1F) | (mlmf_7->HARQ_P_TX << 5);
		mlmf_7->mlmf_7[1] = (mlmf_7->mlmf_7[1] & 0x70) | mlmf_7->MAX_HARQ_Re_TX;
		mlmf_7->mlmf_7[2] = (mlmf_7->mlmf_7[2] & 0x1F) | (mlmf_7->HARQ_P_RX << 5);
		mlmf_7->mlmf_7[2] = (mlmf_7->mlmf_7[2] & 0x70) | mlmf_7->MAX_HARQ_Re_RX;
		mlmf_7->mlmf_7[3] = (mlmf_7->mlmf_7[3] & 0x7F) | (mlmf_7->Setup_Release << 7);
		mlmf_7->mlmf_7[3] = (mlmf_7->mlmf_7[3] & 0xBF) | (mlmf_7->Reserved << 6);
		mlmf_7->mlmf_7[3] = (mlmf_7->mlmf_7[3] & 0xC0) | mlmf_7->Flow_ID;


		return 0;
	}


	int get_mlmf_8(mlmf_8_t* mlmf_8) {	//Reconfiguration Response Message (6.4.2.8)

		mlmf_8->mlmf_8[0] = (mlmf_8->mlmf_8[0] & 0x7F) | (mlmf_8->TX_HARQ << 7);
		mlmf_8->mlmf_8[0] = (mlmf_8->mlmf_8[0] & 0xBF) | (mlmf_8->RX_HARQ << 6);
		mlmf_8->mlmf_8[0] = (mlmf_8->mlmf_8[0] & 0xDF) | (mlmf_8->RD_Capability << 5);
		mlmf_8->mlmf_8[0] = (mlmf_8->mlmf_8[0] & 0xE3) | (mlmf_8->N_Flows << 2);
		mlmf_8->mlmf_8[0] = (mlmf_8->mlmf_8[0] & 0xFD) | mlmf_8->Radio_Resource;
		mlmf_8->mlmf_8[1] = (mlmf_8->mlmf_8[1] & 0x1F) | (mlmf_8->HARQ_P_TX << 5);
		mlmf_8->mlmf_8[1] = (mlmf_8->mlmf_8[1] & 0x70) | mlmf_8->MAX_HARQ_Re_TX;
		mlmf_8->mlmf_8[2] = (mlmf_8->mlmf_8[2] & 0x1F) | (mlmf_8->HARQ_P_RX << 5);
		mlmf_8->mlmf_8[2] = (mlmf_8->mlmf_8[2] & 0x70) | mlmf_8->MAX_HARQ_Re_RX;
		mlmf_8->mlmf_8[3] = (mlmf_8->mlmf_8[3] & 0x7F) | (mlmf_8->Setup_Release << 7);
		mlmf_8->mlmf_8[3] = (mlmf_8->mlmf_8[3] & 0xBF) | (mlmf_8->Reserved << 6);
		mlmf_8->mlmf_8[3] = (mlmf_8->mlmf_8[3] & 0xC0) | mlmf_8->Flow_ID;


		return 0;
	}

	
	////			UNPACKING METHODS

	int get_mlmf_2_rev(mlmf_2_t* mlmf_2) {							// Network Beacon Message (6.4.2.2)

		mlmf_2->Reserved					= (mlmf_2->mlmf_2[0] & 0xE0) >> 5;
		mlmf_2->TX_Power 				= (mlmf_2->mlmf_2[0] & 0x10) >> 4;
		mlmf_2->Power_Const 				= (mlmf_2->mlmf_2[0] & 0x08) >> 3;
		mlmf_2->Current 					= (mlmf_2->mlmf_2[0] & 0x04) >> 2;
		mlmf_2->Network_Beacon_Channels	= mlmf_2->mlmf_2[0] & 0x03;
		mlmf_2->Network_Beacon_Period 	= (mlmf_2->mlmf_2[1] & 0xF0) >> 4;
		mlmf_2->Cluster_Beacon_Period 	= mlmf_2->mlmf_2[1] & 0x0F;
		mlmf_2->Reserved_1 				= (mlmf_2->mlmf_2[2] & 0xE0) >> 5;
		mlmf_2->Next_Cluster_Channel 	= (mlmf_2->mlmf_2[2] & 0x1F) << 8 | mlmf_2->mlmf_2[3];
		mlmf_2->TTn 						= (mlmf_2->mlmf_2[4] << 24) | (mlmf_2->mlmf_2[5] << 16) | (mlmf_2->mlmf_2[6] << 8) | mlmf_2->mlmf_2[7];
		mlmf_2->Reserved_2 				= (mlmf_2->mlmf_2[8] & 0xF0) >> 4;
		mlmf_2->Clusters_Max_TX_Power 	= mlmf_2->mlmf_2[8] & 0x0F;
		mlmf_2->Reserved_3 				= (mlmf_2->mlmf_2[9] & 0xE0) >> 5;
		mlmf_2->Current_Cluster_Channel 	= (mlmf_2->mlmf_2[9] & 0x1F) << 8 | mlmf_2->mlmf_2[10];
		mlmf_2->Reserved_4 				= (mlmf_2->mlmf_2[11] & 0x1F);
		mlmf_2->ANBC 					= (mlmf_2->mlmf_2[11] & 0xE0) & mlmf_2->mlmf_2[12];

		return 0;
	}

	int get_mlmf_3_rev(mlmf_3_t* mlmf_3) {							// Cluster Beacon Message (6.4.2.3)

		mlmf_3->SFN 					= mlmf_3->mlmf_3[0];
		mlmf_3->Reserved 			= (mlmf_3->mlmf_3[1] & 0x20) >> 5;
		mlmf_3->TX_Power 			= (mlmf_3->mlmf_3[1] & 0x10) >> 4;
		mlmf_3->Power_Const 			= (mlmf_3->mlmf_3[1] & 0x08) >> 3;
		mlmf_3->FO 					= (mlmf_3->mlmf_3[1] & 0x04) >> 2;
		mlmf_3->Next_Channel 		= (mlmf_3->mlmf_3[1] & 0x02) >> 1;
		mlmf_3->TTN 					= mlmf_3->mlmf_3[1] & 0x01;
		mlmf_3->Network_Beacon_Period = (mlmf_3->mlmf_3[2] & 0xF0) >> 4;
		mlmf_3->Cluster_Beacon_Period = mlmf_3->mlmf_3[2] & 0x0F;
		mlmf_3->countToTrigger 		= (mlmf_3->mlmf_3[3] & 0xF0) >> 4;
		mlmf_3->ReQuality 			= (mlmf_3->mlmf_3[3] & 0x0C) >> 2;
		mlmf_3->MinQuality 			= mlmf_3->mlmf_3[3] & 0x03;
		mlmf_3->Reserved_1 			= (mlmf_3->mlmf_3[4] & 0x40) >> 6;
		mlmf_3->Clusters_Max_TX_Power = mlmf_3->mlmf_3[4] & 0x3F;
		mlmf_3->Frame_Offset 		= mlmf_3->mlmf_3[5];
		mlmf_3->Reserved_2 			= (mlmf_3->mlmf_3[6] & 0x10) >> 4;
		mlmf_3->Next_Cluster_Channel = (mlmf_3->mlmf_3[6] & 0x0F) << 8 | mlmf_3->mlmf_3[7];
		mlmf_3->TTn 					= (mlmf_3->mlmf_3[8] << 24) | (mlmf_3->mlmf_3[9] << 16) | (mlmf_3->mlmf_3[10] << 8) | mlmf_3->mlmf_3[11];

		return 0;
	}

	int get_mlmf_4_rev(mlmf_4_t* mlmf_4) {							// Association Request Message (6.4.2.4)
		
		mlmf_4->Setup_Cause 			= (mlmf_4->mlmf_4[0] & 0xE0) >> 5;
		mlmf_4->N_Flows 				= (mlmf_4->mlmf_4[0] & 0x1C) >> 2;
		mlmf_4->Power_Const 			= (mlmf_4->mlmf_4[0] & 0x02) >> 1;
		mlmf_4->FT_Mode 				= mlmf_4->mlmf_4[0] & 0x01;
		mlmf_4->Current 				= (mlmf_4->mlmf_4[1] & 0x80) >> 7;
		mlmf_4->Reserved 			= (mlmf_4->mlmf_4[1] & 0x40) >> 6;
		mlmf_4->HARQ_P_TX 			= (mlmf_4->mlmf_4[2] & 0xE0) >> 5;
		mlmf_4->MAX_HARQ_Re_TX 		= (mlmf_4->mlmf_4[2] & 0x70) >> 4;
		mlmf_4->HARQ_P_RX 			= (mlmf_4->mlmf_4[3] & 0xE0) >> 5;
		mlmf_4->MAX_HARQ_Re_RX 		= (mlmf_4->mlmf_4[3] & 0x70) >> 4;
		mlmf_4->Reserved_1 			= (mlmf_4->mlmf_4[4] & 0xC0) >> 6;
		mlmf_4->FlowID 				= mlmf_4->mlmf_4[4] & 0x3F;
		mlmf_4->Network_Beacon_Period = (mlmf_4->mlmf_4[5] & 0xF0) >> 4;
		mlmf_4->Cluster_Beacon_Period = mlmf_4->mlmf_4[5] & 0x0F;
		mlmf_4->Reserved_2 			= (mlmf_4->mlmf_4[6] & 0xE0) >> 5;
		mlmf_4->Next_Cluster_Channel = (mlmf_4->mlmf_4[6] & 0x1F) << 8 | mlmf_4->mlmf_4[7];
		mlmf_4->TTn 					= (mlmf_4->mlmf_4[8] << 24) | (mlmf_4->mlmf_4[9] << 16) | (mlmf_4->mlmf_4[10] << 8) | mlmf_4->mlmf_4[11];
		mlmf_4->Reserved_3 			= (mlmf_4->mlmf_4[12] & 0xE0) >> 5;
		mlmf_4->Current_Cluster_Channel = (mlmf_4->mlmf_4[12] & 0x1F) << 8 | mlmf_4->mlmf_4[13];

		return 0;
	}

	int get_mlmf_5_rev(mlmf_5_t* mlmf_5) {							// Association Response Message (6.4.2.5)

		mlmf_5->ACK_NACK = (mlmf_5->mlmf_5[0] & 0x80) >> 7;
		mlmf_5->Reserved = (mlmf_5->mlmf_5[0] & 0x40) >> 6;
		mlmf_5->HARQ_mod = (mlmf_5->mlmf_5[0] & 0x20) >> 5;
		mlmf_5->NFlows = (mlmf_5->mlmf_5[0] & 0x0C) >> 2;
		mlmf_5->Group = (mlmf_5->mlmf_5[0] & 0x02) >> 1;
		mlmf_5->TX_Power = mlmf_5->mlmf_5[0] & 0x01;
		mlmf_5->Reject_Cause = (mlmf_5->mlmf_5[1] & 0xF0) >> 4;
		mlmf_5->Reject_Time = mlmf_5->mlmf_5[1] & 0x0F;
		mlmf_5->HARQ_P_RX = (mlmf_5->mlmf_5[2] & 0xE0) >> 5;
		mlmf_5->MAX_HARQ_Re_RX = (mlmf_5->mlmf_5[2] & 0x70) >> 4;
		mlmf_5->HARQ_P_TX = (mlmf_5->mlmf_5[3] & 0xE0) >> 5;
		mlmf_5->MAX_HARQ_Re_TX = (mlmf_5->mlmf_5[3] & 0x70) >> 4;
		mlmf_5->Reserved_1 = (mlmf_5->mlmf_5[4] & 0xC0) >> 6;
		mlmf_5->Flow_ID = mlmf_5->mlmf_5[4] & 0x3F;
		mlmf_5->Reserved_2 = (mlmf_5->mlmf_5[5] & 0x80) >> 7;
		mlmf_5->Group_ID = mlmf_5->mlmf_5[5] & 0x7F;
		mlmf_5->Reserved_3 = (mlmf_5->mlmf_5[6] & 0x80) >> 7;
		mlmf_5->Resource_TAG = mlmf_5->mlmf_5[6] & 0x7F;

		return 0;
	}

	int get_mlmf_6_rev(mlmf_6_t* mlmf_6) {							// Association Release Message (6.4.2.6)

		mlmf_6->Release_Cause = (mlmf_6->mlmf_6 & 0xF0) >> 4;
		mlmf_6->Reserved = mlmf_6->mlmf_6 & 0x0F;

		return 0;
	}


	int get_mlmf_7_rev(mlmf_7_t* mlmf_7) {							// Reconfiguration Request Message (6.4.2.7)
		mlmf_7->TX_HARQ = (mlmf_7->mlmf_7[0] & 0x80) >> 7;
		mlmf_7->RX_HARQ = (mlmf_7->mlmf_7[0] & 0x40) >> 6;
		mlmf_7->RD_Capability = (mlmf_7->mlmf_7[0] & 0x20) >> 5;
		mlmf_7->N_Flows = (mlmf_7->mlmf_7[0] & 0x1C) >> 2;
		mlmf_7->Radio_Resource = mlmf_7->mlmf_7[0] & 0x03;
		mlmf_7->HARQ_P_TX = (mlmf_7->mlmf_7[1] & 0xE0) >> 5;
		mlmf_7->MAX_HARQ_Re_TX = (mlmf_7->mlmf_7[1] & 0x70) >> 4;
		mlmf_7->HARQ_P_RX = (mlmf_7->mlmf_7[2] & 0xE0) >> 5;
		mlmf_7->MAX_HARQ_Re_RX = (mlmf_7->mlmf_7[2] & 0x70) >> 4;
		mlmf_7->Setup_Release = (mlmf_7->mlmf_7[3] & 0x80) >> 7;
		mlmf_7->Reserved = (mlmf_7->mlmf_7[3] & 0x40) >> 6;
		mlmf_7->Flow_ID = mlmf_7->mlmf_7[3] & 0x3F;

		return 0;
	}

	int get_mlmf_8_rev(mlmf_8_t* mlmf_8) {							// Reconfiguration Response Message (6.4.2.8)

		mlmf_8->TX_HARQ = (mlmf_8->mlmf_8[0] & 0x80) >> 7;
		mlmf_8->RX_HARQ = (mlmf_8->mlmf_8[0] & 0x40) >> 6;
		mlmf_8->RD_Capability = (mlmf_8->mlmf_8[0] & 0x20) >> 5;
		mlmf_8->N_Flows = (mlmf_8->mlmf_8[0] & 0x1C) >> 2;
		mlmf_8->Radio_Resource = mlmf_8->mlmf_8[0] & 0x03;
		mlmf_8->HARQ_P_TX = (mlmf_8->mlmf_8[1] & 0xE0) >> 5;
		mlmf_8->MAX_HARQ_Re_TX = (mlmf_8->mlmf_8[1] & 0x70) >> 4;
		mlmf_8->HARQ_P_RX = (mlmf_8->mlmf_8[2] & 0xE0) >> 5;
		mlmf_8->MAX_HARQ_Re_RX = (mlmf_8->mlmf_8[2] & 0x70) >> 4;
		mlmf_8->Setup_Release = (mlmf_8->mlmf_8[3] & 0x80) >> 7;
		mlmf_8->Reserved = (mlmf_8->mlmf_8[3] & 0x40) >> 6;
		mlmf_8->Flow_ID = mlmf_8->mlmf_8[3] & 0x3F;

		return 0;
	}





