/*
 * This code is property of Ostfalia University of Applied Sciences, Germany and Universidad Politécnica de Cartagena, Spain. 
 * Relations with QARTIA Smart Technologies, any inquiries please contact author. 
 * Based on ETSI TS 103 636-4 V1.5.1
 * Author: Iván Tomás García <ivan.tomas@edu.upct.es>
 */


#include <string.h>
#include "mac_info_elements.h"

////			PACKING METHODS

int get_mlie_1(mlie_1_t* mlie_1) {	//MAC Security Info IE (6.4.3.1)

	mlie_1->mlie_1[0] = (mlie_1->mlie_1[0] & 0x3F) | (mlie_1->Version << 3);
	mlie_1->mlie_1[0] = (mlie_1->mlie_1[0] & 0xCF) | (mlie_1->Key_Index << 2);
	mlie_1->mlie_1[0] = (mlie_1->mlie_1[0] & 0xF0) | mlie_1->Sec_IV_Type;
	mlie_1->mlie_1[1] = (uint8_t)0xFF & (mlie_1->HPC >> 24);
	mlie_1->mlie_1[2] = (uint8_t)0xFF & (mlie_1->HPC >> 16);
	mlie_1->mlie_1[3] = (uint8_t)0xFF & (mlie_1->HPC >> 8);
	mlie_1->mlie_1[4] = (uint8_t)(0xFF & mlie_1->HPC);

	return 0;
}


int get_mlie_2(mlie_2_t* mlie_2) {	//Route Info IE (6.4.3.2)

	mlie_2->mlie_2[0] = (uint8_t)0xFF & (mlie_2->Sink_Address >> 24);
	mlie_2->mlie_2[1] = (uint8_t)0xFF & (mlie_2->Sink_Address >> 16);
	mlie_2->mlie_2[2] = (uint8_t)0xFF & (mlie_2->Sink_Address >> 8);
	mlie_2->mlie_2[3] = (uint8_t)(0xFF & mlie_2->Sink_Address);
	mlie_2->mlie_2[4] = mlie_2->Sink_Address;
	mlie_2->mlie_2[5] = mlie_2->App_Sec_N;

	return 0;
}


int get_mlie_3(mlie_3_t* mlie_3) {	//Resource allocation IE (6.4.3.3)

	mlie_3->mlie_3[0] = (mlie_3->mlie_3[0] & 0x3F) | (mlie_3->Allocation_Type << 6);
	mlie_3->mlie_3[0] = (mlie_3->mlie_3[0] & 0xDF) | (mlie_3->Add << 5);
	mlie_3->mlie_3[0] = (mlie_3->mlie_3[0] & 0xEF) | (mlie_3->ID << 4);
	mlie_3->mlie_3[0] = (mlie_3->mlie_3[0] & 0xF1) | (mlie_3->Repeat << 1);
	mlie_3->mlie_3[0] = (mlie_3->mlie_3[0] & 0xFE) | mlie_3->SFN;
	mlie_3->mlie_3[1] = (mlie_3->mlie_3[1] & 0x7F) | (mlie_3->Channel << 7);
	mlie_3->mlie_3[1] = (mlie_3->mlie_3[1] & 0xBF) | (mlie_3->RLF << 6);
	mlie_3->mlie_3[1] = (mlie_3->mlie_3[1] & 0xC0) | mlie_3->Reserved;
	mlie_3->mlie_3[2] = (uint8_t) (0xFF & (mlie_3->Start_Subslot >> 8));
	mlie_3->mlie_3[3] = (uint8_t) (0xFF & mlie_3->Start_Subslot);
	mlie_3->mlie_3[4] = (mlie_3->mlie_3[4] & 0x7F) | (mlie_3->Length_Type << 7);
	mlie_3->mlie_3[4] = (mlie_3->mlie_3[4] & 0x80) | mlie_3->Length;
	mlie_3->mlie_3[5] = (uint8_t)(0xFF & (mlie_3->Start_Subslot_1 >> 8));
	mlie_3->mlie_3[6] = (uint8_t)(0xFF & mlie_3->Start_Subslot_1);
	mlie_3->mlie_3[7] = (mlie_3->mlie_3[7] & 0x7F) | (mlie_3->Length_Type_1 << 7);
	mlie_3->mlie_3[7] = (mlie_3->mlie_3[7] & 0x80) | mlie_3->Length_1;
	mlie_3->mlie_3[8] = (uint8_t)(0xFF & (mlie_3->Short_RD_ID >> 8));
	mlie_3->mlie_3[9] = (uint8_t)(0xFF & mlie_3->Short_RD_ID);
	mlie_3->mlie_3[10] = mlie_3->Repetition;
	mlie_3->mlie_3[11] = mlie_3->Validity;
	mlie_3->mlie_3[12] = mlie_3->SFN_Value;
	mlie_3->mlie_3[13] = ((mlie_3->Reserved_1 & 0x07) << 5) 
		| ((mlie_3->Channel_1 >> 8) & 0x1F);
	mlie_3->mlie_3[14] = (uint8_t) (0xFF & mlie_3->Channel_1);
	mlie_3->mlie_3[15] = ((mlie_3->Reserved_2 & 0x0F) << 4) 
		| (mlie_3->dect_Scheduled_Resource_Failure & 0x0F);

	return 0;
}


int get_mlie_4(mlie_4_t* mlie_4) {	//Random Access Resource (6.4.3.4)

	mlie_4->mlie_4[0] = (mlie_4->mlie_4[0] & 0x1F) | (mlie_4->Reserved << 5);
	mlie_4->mlie_4[0] = (mlie_4->mlie_4[0] & 0xE7) | (mlie_4->Repeat << 3);
	mlie_4->mlie_4[0] = (mlie_4->mlie_4[0] & 0xFB) | (mlie_4->SFN << 2);
	mlie_4->mlie_4[0] = (mlie_4->mlie_4[0] & 0xFD) | (mlie_4->Channel << 1);
	mlie_4->mlie_4[0] = (mlie_4->mlie_4[0] & 0xFE) | mlie_4->Channel2;
	mlie_4->mlie_4[1] = (uint8_t)(0xFF & (mlie_4->Start_Subslot >> 8));
	mlie_4->mlie_4[2] = (uint8_t)(0xFF & mlie_4->Start_Subslot);
	mlie_4->mlie_4[3] = (mlie_4->mlie_4[4] & 0x7F) | (mlie_4->Length_Type << 7);
	mlie_4->mlie_4[3] = (mlie_4->mlie_4[4] & 0x80) | mlie_4->Length;
	mlie_4->mlie_4[4] = (mlie_4->mlie_4[4] & 0x7F) | (mlie_4->MAX_Length_Type << 7);
	mlie_4->mlie_4[4] = (mlie_4->mlie_4[4] & 0x87) | (mlie_4->MAX_RACH_Length << 3);
	mlie_4->mlie_4[4] = (mlie_4->mlie_4[4] & 0xE8) | mlie_4->CW_Min_Sig;
	mlie_4->mlie_4[5] = (mlie_4->mlie_4[4] & 0x7F) | (mlie_4->DECT_Delay << 7);
	mlie_4->mlie_4[4] = (mlie_4->mlie_4[4] & 0x87) | (mlie_4->Response_Window << 3);
	mlie_4->mlie_4[5] = (mlie_4->mlie_4[4] & 0xE8) | mlie_4->CW_Max_Sig;
	mlie_4->mlie_4[6] = mlie_4->Repetition;
	mlie_4->mlie_4[7] = mlie_4->Validity;
	mlie_4->mlie_4[8] = mlie_4->SNF_Value;
	mlie_4->mlie_4[9] = (mlie_4->mlie_4[8] & 0x1F) | (mlie_4->Reserved_1 << 5);
	mlie_4->mlie_4[9] = (uint8_t)((mlie_4->mlie_4[8] & 0xE0) | (mlie_4->Channel_1 >> 8));
	mlie_4->mlie_4[10] = (uint8_t)(0xFF & mlie_4->Channel_1);
	mlie_4->mlie_4[11] = (mlie_4->mlie_4[8] & 0x0F) | (mlie_4->Reserved_2 << 4);
	mlie_4->mlie_4[11] = (uint8_t)((mlie_4->mlie_4[8] & 0xF0) | (mlie_4->Channel_2 >> 8));
	mlie_4->mlie_4[12] = (uint8_t)(0xFF & mlie_4->Channel_2);

	return 0;
}


int get_mlie_5(mlie_5_t* mlie_5) {	//RD Capability IE (6.4.3.5)

	mlie_5->mlie_5[0] = (mlie_5->mlie_5[0] & 0x1F) | (mlie_5->N_PHY_Cap << 5);
	mlie_5->mlie_5[0] = (mlie_5->mlie_5[0] & 0xE0) | (mlie_5->Release);
	mlie_5->mlie_5[1] = (mlie_5->mlie_5[1] & 0x3F) | (mlie_5->Reserved << 6);
	mlie_5->mlie_5[1] = (mlie_5->mlie_5[1] & 0xDF) | (mlie_5->Group_As << 5);
	mlie_5->mlie_5[1] = (mlie_5->mlie_5[1] & 0xEF) | (mlie_5->Paging << 4);
	mlie_5->mlie_5[1] = (mlie_5->mlie_5[1] & 0xF3) | (mlie_5->Op_Modes << 2);
	mlie_5->mlie_5[1] = (mlie_5->mlie_5[1] & 0xFD) | (mlie_5->Mesh << 1);
	mlie_5->mlie_5[1] = (mlie_5->mlie_5[1] & 0xFE) | (mlie_5->Schedul);
	mlie_5->mlie_5[2] = (mlie_5->mlie_5[2] & 0x1F) | (mlie_5->MAC_Security << 5);
	mlie_5->mlie_5[2] = (mlie_5->mlie_5[2] & 0xE3) | (mlie_5->DLC_Service_Type << 2);
	mlie_5->mlie_5[2] = (mlie_5->mlie_5[2] & 0xFC) | (mlie_5->Reserved_1 >> 1);
	mlie_5->mlie_5[3] = (mlie_5->mlie_5[3] & 0x7F) | (mlie_5->Reserved_1 << 7);
	mlie_5->mlie_5[3] = (mlie_5->mlie_5[3] & 0x8F) | (mlie_5->RD_Power_Class << 4);
	mlie_5->mlie_5[3] = (mlie_5->mlie_5[3] & 0xF3) | (mlie_5->MAX_NSS_RX << 2);
	mlie_5->mlie_5[3] = (mlie_5->mlie_5[3] & 0xFC) | (mlie_5->RX_TX_Diversity);
	mlie_5->mlie_5[4] = (mlie_5->mlie_5[4] & 0x0F) | (mlie_5->RX_Gain << 4);
	mlie_5->mlie_5[4] = (mlie_5->mlie_5[4] & 0xF0) | (mlie_5->Max_MCS);
	mlie_5->mlie_5[5] = (mlie_5->mlie_5[5] & 0x0F) | (mlie_5->Softbuffer_Size << 4);
	mlie_5->mlie_5[5] = (mlie_5->mlie_5[5] & 0xF3) | (mlie_5->N_HARQ_Process << 2);
	mlie_5->mlie_5[5] = (mlie_5->mlie_5[5] & 0xFC) | (mlie_5->Reserved_2);
	mlie_5->mlie_5[5] = (mlie_5->mlie_5[5] & 0xF3) | (mlie_5->N_HARQ_Process << 2);
	mlie_5->mlie_5[6] = (mlie_5->mlie_5[6] & 0x0F) | (mlie_5->HARQ_Feedback_delay << 4);
	mlie_5->mlie_5[6] = (mlie_5->mlie_5[6] & 0xF7) | (mlie_5->D_Delay << 3);
	mlie_5->mlie_5[6] = (mlie_5->mlie_5[6] & 0xFB) | (mlie_5->HalfDup << 2);
	mlie_5->mlie_5[6] = (mlie_5->mlie_5[6] & 0xFC) | (mlie_5->Reserved_3);
	mlie_5->mlie_5[7] = (mlie_5->mlie_5[7] & 0x1F) | (mlie_5->mu << 5);
	mlie_5->mlie_5[7] = (mlie_5->mlie_5[7] & 0xE1) | (mlie_5->beta << 1);
	mlie_5->mlie_5[7] = (mlie_5->mlie_5[7] & 0xFE) | (mlie_5->Reserved_4 >> 1);
	mlie_5->mlie_5[8] = (mlie_5->mlie_5[8] & 0x7F) | (mlie_5->Reserved_4 << 7);
	mlie_5->mlie_5[8] = (mlie_5->mlie_5[8] & 0x8F) | (mlie_5->RD_Power_Class << 4);
	mlie_5->mlie_5[8] = (mlie_5->mlie_5[8] & 0xF3) | (mlie_5->MAX_NSS_RX_1 << 2);
	mlie_5->mlie_5[8] = (mlie_5->mlie_5[8] & 0xFC) | (mlie_5->RX_TX_Diversity_1);
	mlie_5->mlie_5[8] = (mlie_5->mlie_5[9] & 0x0F) | (mlie_5->RX_Gain_1 << 4);
	mlie_5->mlie_5[8] = (mlie_5->mlie_5[9] & 0xF0) | (mlie_5->MAX_MCS_1);
	mlie_5->mlie_5[10] = (mlie_5->mlie_5[10] & 0x0F) | (mlie_5->Softbuffer_Size_1 << 4);
	mlie_5->mlie_5[10] = (mlie_5->mlie_5[10] & 0xFC) | (mlie_5->N_HARQ_Process_1 << 2);
	mlie_5->mlie_5[10] = (mlie_5->mlie_5[10] & 0xF3) | (mlie_5->Reserved_5);
	mlie_5->mlie_5[11] = (mlie_5->mlie_5[11] & 0x0F) | (mlie_5->HARQ_Feedback_delay_1 << 4);
	mlie_5->mlie_5[11] = (mlie_5->mlie_5[11] & 0x0F) | (mlie_5->Reserved_6);

	return 0;
}

int get_mlie_6(mlie_6_t* mlie_6) {	//Neighbouring IE (6.4.3.6)

	mlie_6->mlie_6[0] = (mlie_6->mlie_6[0] & 0x7F) | (mlie_6->Reserved << 7);
	mlie_6->mlie_6[0] = (mlie_6->mlie_6[0] & 0xBF) | (mlie_6->ID << 6);
	mlie_6->mlie_6[0] = (mlie_6->mlie_6[0] & 0xDF) | (mlie_6->mu << 5);
	mlie_6->mlie_6[0] = (mlie_6->mlie_6[0] & 0xEF) | (mlie_6->SNR << 4);
	mlie_6->mlie_6[0] = (mlie_6->mlie_6[0] & 0xF7) | (mlie_6->RSSI_2 << 3);
	mlie_6->mlie_6[0] = (mlie_6->mlie_6[0] & 0xFB) | (mlie_6->Power_Const << 2);
	mlie_6->mlie_6[0] = (mlie_6->mlie_6[0] & 0xFD) | (mlie_6->Next_Channel << 1);
	mlie_6->mlie_6[0] = (mlie_6->mlie_6[0] & 0xFE) | mlie_6->TTN;
	mlie_6->mlie_6[1] = (mlie_6->mlie_6[1] & 0x0F) | (mlie_6->Network_Beacon_Period << 4);
	mlie_6->mlie_6[1] = (mlie_6->mlie_6[1] & 0xF0) | mlie_6->Cluster_Beacon_Period;
	mlie_6->mlie_6[2] = (uint8_t)(0xFF & (mlie_6->Long_RD_ID << 12));
	mlie_6->mlie_6[3] = (uint8_t)(0xFF & (mlie_6->Long_RD_ID >> 8));
	mlie_6->mlie_6[4] = (uint8_t)(0xFF & (mlie_6->Long_RD_ID >> 4));
	mlie_6->mlie_6[5] = (uint8_t)(0xFF & mlie_6->Long_RD_ID);
	mlie_6->mlie_6[6] = (mlie_6->mlie_6[6] & 0x1F) | (mlie_6->Reserved_1 << 5);
	mlie_6->mlie_6[6] = (mlie_6->mlie_6[6] & 0xE0) | (mlie_6->Next_Cluster_Channel >> 4);
	mlie_6->mlie_6[7] = (mlie_6->mlie_6[7] & 0xFF) | (mlie_6->Next_Cluster_Channel);
	mlie_6->mlie_6[8] = (uint8_t)(0xFF & (mlie_6->TTN << 12));
	mlie_6->mlie_6[9] = (uint8_t)(0xFF & (mlie_6->TTN >> 8));
	mlie_6->mlie_6[10] = (uint8_t)(0xFF & (mlie_6->TTN >> 4));
	mlie_6->mlie_6[11] = (uint8_t)(0xFF & mlie_6->TTN);
	mlie_6->mlie_6[12] = (uint8_t)(0xFF & mlie_6->RSSI_2_1);
	mlie_6->mlie_6[13] = (uint8_t)(0xFF & mlie_6->SNR_1);
	mlie_6->mlie_6[14] = (uint8_t)(0x1F & mlie_6->Radio_Device_Class_mu << 5);
	mlie_6->mlie_6[14] = (mlie_6->mlie_6[14] & 0xE1) | (mlie_6->Radio_Device_Class_beta << 1);
	mlie_6->mlie_6[14] = (mlie_6->mlie_6[14] & 0x01) | mlie_6->Reserved_2;

	return 0;

}


int get_mlie_7(mlie_7_t* mlie_7) {	//Broadcast Indication IE (6.4.3.7)

	mlie_7->mlie_7[0] = (mlie_7->mlie_7[0] & 0x1F) | (mlie_7->Indication_Type << 5);
	mlie_7->mlie_7[0] = (mlie_7->mlie_7[0] & 0xEF) | (mlie_7->IDType << 4);
	mlie_7->mlie_7[0] = (mlie_7->mlie_7[0] & 0xF7) | (mlie_7->ACK_NACK << 3);
	mlie_7->mlie_7[0] = (mlie_7->mlie_7[0] & 0xF9) | (mlie_7->Feedback << 1);
	mlie_7->mlie_7[0] = (mlie_7->mlie_7[0] & 0xFE) | mlie_7->Resource_Allocation;
	mlie_7->mlie_7[1] = (uint8_t) (0xFF & mlie_7->LongShort_RDID>> 4);
	mlie_7->mlie_7[2] = (uint8_t) (0xFF & mlie_7->LongShort_RDID);
	mlie_7->mlie_7[3] = (uint8_t) (0xFF & mlie_7->MCS_MIMO_Feedback);

	return 0;

}

//// No definition for mlie_8


int get_mlie_9(mlie_9_t* mlie_9) {	//Group Assignment IE (6.4.3.9)

	mlie_9->mlie_9[0] = (mlie_9->mlie_9[0] & 0x7F) | (mlie_9->Single << 7);
	mlie_9->mlie_9[0] = (mlie_9->mlie_9[0] & 0x80) | (mlie_9->Group_ID);
	mlie_9->mlie_9[1] = (mlie_9->mlie_9[1] & 0x7F) | (mlie_9->Direct << 7);
	mlie_9->mlie_9[1] = (mlie_9->mlie_9[1] & 0x80) | (mlie_9->ResourceTag);
	mlie_9->mlie_9[2] = (mlie_9->mlie_9[2] & 0x7F) | (mlie_9->Direct1 << 7);
	mlie_9->mlie_9[2] = (mlie_9->mlie_9[2] & 0x80) | (mlie_9->ResourceTag1);
	mlie_9->mlie_9[3] = (mlie_9->mlie_9[3] & 0x7F) | (mlie_9->Direct2 << 7);
	mlie_9->mlie_9[3] = (mlie_9->mlie_9[3] & 0x80) | (mlie_9->ResourceTag2);

	return 0;

}


int get_mlie_10(mlie_10_t* mlie_10) {	//Load Info IE (6.4.3.10)

	mlie_10->mlie_10[0] = (mlie_10->mlie_10[0] & 0x0F) | (mlie_10->Reserved << 4);
	mlie_10->mlie_10[0] = (mlie_10->mlie_10[0] & 0xF7) | (mlie_10->MAX_Assoc << 3);
	mlie_10->mlie_10[0] = (mlie_10->mlie_10[0] & 0xFB) | (mlie_10->RD_PT_Load << 2);
	mlie_10->mlie_10[0] = (mlie_10->mlie_10[0] & 0xFD) | (mlie_10->RACH_Load << 1);
	mlie_10->mlie_10[0] = (mlie_10->mlie_10[0] & 0xFE) | mlie_10->Channel_Load;
	mlie_10->mlie_10[1] = (uint8_t) (0xFF & mlie_10->Traffic_Load_Percentage);
	mlie_10->mlie_10[2] = (uint8_t) (0xFF & mlie_10->MAX_n_Assoc_RDs >> 4);
	mlie_10->mlie_10[3] = (uint8_t) (0xFF & mlie_10->MAX_n_Assoc_RDs);
	mlie_10->mlie_10[4] = (uint8_t) (0xFF & mlie_10->Associated_RDs_FT);
	mlie_10->mlie_10[5] = (uint8_t) (0xFF & mlie_10->Associated_RDs_PT);
	mlie_10->mlie_10[6] = (uint8_t) (0xFF & mlie_10->RACH_Load_percentage);
	mlie_10->mlie_10[7] = (uint8_t) (0xFF & mlie_10->Free_subslots_detected);
	mlie_10->mlie_10[8] = (uint8_t) (0xFF & mlie_10->Busy_subslots_detected);


	return 0;

}

// No definition for mlie_11


int get_mlie_12(mlie_12_t* mlie_12) {	//Measurement Report IE (6.4.3.12)

	mlie_12->mlie_12[0] = (mlie_12->mlie_12[0] & 0x1F) | (mlie_12->Reserved << 5);
	mlie_12->mlie_12[0] = (mlie_12->mlie_12[0] & 0xEF) | (mlie_12->SNR << 4);
	mlie_12->mlie_12[0] = (mlie_12->mlie_12[0] & 0xF7) | (mlie_12->RSSI_2 << 3);
	mlie_12->mlie_12[0] = (mlie_12->mlie_12[0] & 0xFB) | (mlie_12->RSSI_1 << 2);
	mlie_12->mlie_12[0] = (mlie_12->mlie_12[0] & 0xFD) | (mlie_12->Tx_Count << 1);
	mlie_12->mlie_12[0] = (mlie_12->mlie_12[0] & 0xFE) | mlie_12->RACH;
	mlie_12->mlie_12[1] = (uint8_t) (0xFF & mlie_12->SNR_Result);
	mlie_12->mlie_12[2] = (uint8_t) (0xFF & mlie_12->RSSI_2_Result);
	mlie_12->mlie_12[3] = (uint8_t) (0xFF & mlie_12->RSSI_1_Result);
	mlie_12->mlie_12[4] = (uint8_t) (0xFF & mlie_12->Tx_Count_Result);

	return 0;

}


int get_mlie_13(mlie_13_t* mlie_13) {	//Radio Device Status IE (6.4.3.13)

	mlie_13->mlie_13[0] = (mlie_13->mlie_13[0] & 0x3F) | (mlie_13->Reserved << 6);
	mlie_13->mlie_13[0] = (mlie_13->mlie_13[0] & 0xCF) | (mlie_13->Status_Flag << 4);
	mlie_13->mlie_13[0] = (mlie_13->mlie_13[0] & 0xF0) | mlie_13->Duration;

	return 0;

}

//// No definition for mlie_14


////			UNPACKING METHODS

int get_mlie_1_rev(mlie_1_t* mlie_1) {	//MAC Security Info IE (6.4.3.1)

	mlie_1->Version = (mlie_1->mlie_1[0] & 0xC0) >> 6;
	mlie_1->Key_Index = (mlie_1->mlie_1[0] & 0x30) >> 4;
	mlie_1->Sec_IV_Type = (mlie_1->mlie_1[0] & 0x0F);
	mlie_1->HPC = (mlie_1->mlie_1[1] & 0xFF) << 12 | (mlie_1->mlie_1[2] & 0xFF) << 8 
		| (mlie_1->mlie_1[3] & 0xFF) << 4 | (mlie_1->mlie_1[4] & 0xFF);

	return 0;

}


int get_mlie_2_rev(mlie_2_t* mlie_2) {	//Route Info IE (6.4.3.2)

	mlie_2->Sink_Address = (mlie_2->mlie_2[0] & 0xFF) << 12 | (mlie_2->mlie_2[1] & 0xFF) << 8 
		| (mlie_2->mlie_2[2] & 0xFF) << 4 | (mlie_2->mlie_2[3] & 0xFF);
	mlie_2->Route_Cost = (mlie_2->mlie_2[4] & 0xFF);
	mlie_2->App_Sec_N = (mlie_2->mlie_2[5] & 0xFF); 

	return 0;

}


int get_mlie_3_rev(mlie_3_t* mlie_3) {	//Resource allocation IE (6.4.3.3)

	mlie_3->Allocation_Type = ((mlie_3->mlie_3[0] & 0xC0) >> 6);
	mlie_3->Add = ((mlie_3->mlie_3[0] & 0x20) >> 5);
	mlie_3->ID = ((mlie_3->mlie_3[0] & 0x10) >> 4);
	mlie_3->Repeat = ((mlie_3->mlie_3[0] & 0x02) >> 1);
	mlie_3->SFN = (mlie_3->mlie_3[0] & 0x01);
	mlie_3->Channel = ((mlie_3->mlie_3[1] & 0x80) >> 7);
	mlie_3->RLF = ((mlie_3->mlie_3[1] & 0x40) >> 6);
	mlie_3->Reserved = (mlie_3->mlie_3[1] & 0x3F) << 7 | (mlie_3->mlie_3[2] & 0xFE) >> 1;
	mlie_3->Start_Subslot = (mlie_3->mlie_3[2] & 0x1F) << 8 | (mlie_3->mlie_3[3] & 0xFF);
	mlie_3->Length_Type = ((mlie_3->mlie_3[4] & 0x80) >> 7);
	mlie_3->Length = (mlie_3->mlie_3[4] & 0x7F);
	mlie_3->Reserved_1 = ((mlie_3->mlie_3[5] & 0xFE) >> 1);
	mlie_3->Start_Subslot_1 = (mlie_3->mlie_3[5] & 0x01) << 8 | (mlie_3->mlie_3[6] & 0xFF);
	mlie_3->Length_Type_1 = ((mlie_3->mlie_3[7] & 0x80) >> 7);
	mlie_3->Length_1 = (mlie_3->mlie_3[7] & 0x7F);
	mlie_3->Short_RD_ID = (mlie_3->mlie_3[8] & 0xFF) << 8 | (mlie_3->mlie_3[9] & 0xFF);
	mlie_3->Repetition = (mlie_3->mlie_3[10] & 0xFF);
	mlie_3->Validity = (mlie_3->mlie_3[11] & 0xFF);
	mlie_3->SFN_Value = (mlie_3->mlie_3[12] & 0xFF);
	mlie_3->Reserved_2 = ((mlie_3->mlie_3[13] & 0xE0) >> 5);
	mlie_3->Channel_1 = (mlie_3->mlie_3[13] & 0x1F) << 8 | (mlie_3->mlie_3[14] & 0xFF);
	mlie_3->dect_Scheduled_Resource_Failure = (mlie_3->mlie_3[15] & 0xFF);

	return 0;

}


int get_mlie_4_rev(mlie_4_t* mlie_4) {	//Random Access Resource IE (6.4.3.4)

	mlie_4->Reserved = (mlie_4->mlie_4[0] & 0xE0) >> 5;
	mlie_4->Repeat = (mlie_4->mlie_4[0] & 0x18) >> 3;
	mlie_4->SFN = (mlie_4->mlie_4[0] & 0x04) >> 2;
	mlie_4->Channel = (mlie_4->mlie_4[0] & 0x02) >> 1;
	mlie_4->Channel2 = (mlie_4->mlie_4[0] & 0x01);
	mlie_4->Reserved = (mlie_4->mlie_4[1] & 0xFE) >> 1;
	mlie_4->Start_Subslot = (mlie_4->mlie_4[1] & 0x01) << 8 | (mlie_4->mlie_4[2] & 0xFF);
	mlie_4->Length_Type = (mlie_4->mlie_4[3] & 0x80) >> 7;
	mlie_4->Length = (mlie_4->mlie_4[3] & 0x7F);
	mlie_4->MAX_Length_Type = (mlie_4->mlie_4[4] & 0x80) >> 7;
	mlie_4->MAX_RACH_Length = (mlie_4->mlie_4[4] & 0x78) >> 3;
	mlie_4->CW_Min_Sig = (mlie_4->mlie_4[4] & 0x07);
	mlie_4->DECT_Delay = (mlie_4->mlie_4[5] & 0x80) >> 7;
	mlie_4->Response_Window = (mlie_4->mlie_4[5] & 0x78) >> 3;
	mlie_4->CW_Max_Sig = (mlie_4->mlie_4[5] & 0x07);
	mlie_4->Repetition = (mlie_4->mlie_4[6] & 0xFF);
	mlie_4->Validity = (mlie_4->mlie_4[7] & 0xFF);
	mlie_4->SNF_Value = (mlie_4->mlie_4[8] & 0xFF);
	mlie_4->Reserved_1 = (mlie_4->mlie_4[9] & 0xE0) >> 5;
	mlie_4->Channel_1 = (mlie_4->mlie_4[9] & 0x1F) << 8 | (mlie_4->mlie_4[10] & 0xFF);
	mlie_4->Reserved_2 = (mlie_4->mlie_4[11] & 0xE0) >> 5;
	mlie_4->Channel_2 = (mlie_4->mlie_4[11] & 0x1F) << 8 | (mlie_4->mlie_4[12] & 0xFF);

	return 0;

}


int get_mlie_5_rev(mlie_5_t* mlie_5) {	//RD Capability IE (6.4.3.5)

	mlie_5->N_PHY_Cap = (mlie_5->mlie_5[0] & 0xE0) >> 5;
	mlie_5->Release = (mlie_5->mlie_5[0] & 0x1C) >> 2;
	mlie_5->Reserved = (mlie_5->mlie_5[1] & 0xC0) >> 6;
	mlie_5->Group_As = (mlie_5->mlie_5[1] & 0x20) >> 5;
	mlie_5->Paging = (mlie_5->mlie_5[1] & 0x10) >> 4;
	mlie_5->Op_Modes = (mlie_5->mlie_5[1] & 0x0C) >> 2;
	mlie_5->Mesh = (mlie_5->mlie_5[1] & 0x02) >> 1;
	mlie_5->Schedul = (mlie_5->mlie_5[1] & 0x01);
	mlie_5->MAC_Security = (mlie_5->mlie_5[2] & 0x20) >> 5;
	mlie_5->DLC_Service_Type = (mlie_5->mlie_5[2] & 0x1C) >> 2;
	mlie_5->Reserved_1 = (mlie_5->mlie_5[2] & 0x03) << 1 | (mlie_5->mlie_5[3] & 0x80) >> 7;
	mlie_5->RD_Power_Class = (mlie_5->mlie_5[3] & 0x70) >> 4;
	mlie_5->MAX_NSS_RX = (mlie_5->mlie_5[3] & 0x0C) >> 2;
	mlie_5->RX_TX_Diversity = (mlie_5->mlie_5[3] & 0x03);
	mlie_5->RX_Gain = (mlie_5->mlie_5[4] & 0xF0) >> 4;
	mlie_5->Max_MCS = (mlie_5->mlie_5[4] & 0x0F);
	mlie_5->Softbuffer_Size = (mlie_5->mlie_5[5] & 0xF0) >> 4;
	mlie_5->N_HARQ_Process = (mlie_5->mlie_5[5] & 0x0C) >> 2;
	mlie_5->Reserved_2 = (mlie_5->mlie_5[5] & 0x03);
	mlie_5->HARQ_Feedback_delay = (mlie_5->mlie_5[6] & 0xF0) >> 4;
	mlie_5->D_Delay = (mlie_5->mlie_5[6] & 0x08) >> 3;
	mlie_5->HalfDup = (mlie_5->mlie_5[6] & 0x04) >> 2;	
	mlie_5->Reserved_3 = (mlie_5->mlie_5[6] & 0x03);
	mlie_5->mu = (mlie_5->mlie_5[7] & 0xE0) >> 5;
	mlie_5->beta = (mlie_5->mlie_5[7] & 0x1E) >> 1;
	mlie_5->Reserved_4 = (mlie_5->mlie_5[7] & 0x01) << 7 | (mlie_5->mlie_5[8] & 0xFE) >> 1;
	mlie_5->RD_Power_Class_1 = (mlie_5->mlie_5[8] & 0xF0) >> 4;
	mlie_5->MAX_NSS_RX_1 = (mlie_5->mlie_5[8] & 0x0C) >> 2;
	mlie_5->RX_TX_Diversity_1 = (mlie_5->mlie_5[8] & 0x03);
	mlie_5->RX_Gain_1 = (mlie_5->mlie_5[9] & 0xF0) >> 4;
	mlie_5->MAX_MCS_1 = (mlie_5->mlie_5[9] & 0x0F);
	mlie_5->Softbuffer_Size_1 = (mlie_5->mlie_5[10] & 0xF0) >> 4;
	mlie_5->N_HARQ_Process_1 = (mlie_5->mlie_5[10] & 0x0C) >> 2;
	mlie_5->Reserved_5 = (mlie_5->mlie_5[10] & 0x03);
	mlie_5->HARQ_Feedback_delay_1 = (mlie_5->mlie_5[11] & 0xF0) >> 4;
	mlie_5->Reserved_6 = (mlie_5->mlie_5[11] & 0x0F);

	return 0;

}


int get_mlie_6_rev(mlie_6_t* mlie_6) {	//Neighbouring IE (6.4.3.6)

	mlie_6->Reserved = (mlie_6->mlie_6[0] & 0x80) >> 7;
	mlie_6->ID = (mlie_6->mlie_6[0] & 0x40) >> 6;
	mlie_6->mu = (mlie_6->mlie_6[0] & 0x20) >> 5;
	mlie_6->SNR = (mlie_6->mlie_6[0] & 0x10) >> 4;
	mlie_6->RSSI_2 = (mlie_6->mlie_6[0] & 0x08) >> 3;
	mlie_6->Power_Const = (mlie_6->mlie_6[0] & 0x04) >> 2;
	mlie_6->Next_Channel = (mlie_6->mlie_6[0] & 0x02) >> 1;
	mlie_6->TTN = (mlie_6->mlie_6[0] & 0x01);
	mlie_6->Network_Beacon_Period = (mlie_6->mlie_6[1] & 0xF0) >> 4;
	mlie_6->Cluster_Beacon_Period = (mlie_6->mlie_6[1] & 0x0F);
	mlie_6->Long_RD_ID = (mlie_6->mlie_6[2] & 0xFF) << 12 | (mlie_6->mlie_6[3] & 0xFF) << 8 
		| (mlie_6->mlie_6[4] & 0xFF) << 4 | (mlie_6->mlie_6[5] & 0xFF);
	mlie_6->Reserved_1 = (mlie_6->mlie_6[6] & 0x60) >> 5;
	mlie_6->Next_Cluster_Channel = (mlie_6->mlie_6[6] & 0x1F) << 8 | (mlie_6->mlie_6[7] & 0xFF);
	mlie_6->TTN = (mlie_6->mlie_6[8] & 0xFF) << 12 | (mlie_6->mlie_6[9] & 0xFF) << 8 
		| (mlie_6->mlie_6[10] & 0xFF) << 4 | (mlie_6->mlie_6[11] & 0xFF);
	mlie_6->RSSI_2_1 = (mlie_6->mlie_6[12] & 0xFF);
	mlie_6->SNR_1 = (mlie_6->mlie_6[13] & 0xFF);
	mlie_6->Radio_Device_Class_mu = (mlie_6->mlie_6[14] & 0xF8) >> 5;
	mlie_6->Radio_Device_Class_beta = (mlie_6->mlie_6[14] & 0x06) >> 1;
	mlie_6->Reserved_2 = (mlie_6->mlie_6[14] & 0x01);

	return 0;

}


int get_mlie_7_rev(mlie_7_t* mlie_7) {	//Broadcast Indication IE (6.4.3.7)

	mlie_7->Indication_Type = (mlie_7->mlie_7[0] & 0xE0) >> 5;
	mlie_7->IDType = (mlie_7->mlie_7[0] & 0x10) >> 4;
	mlie_7->ACK_NACK = (mlie_7->mlie_7[0] & 0x08) >> 3;
	mlie_7->Feedback = (mlie_7->mlie_7[0] & 0x02) >> 1;
	mlie_7->Resource_Allocation = (mlie_7->mlie_7[0] & 0x01);
	mlie_7->LongShort_RDID = (mlie_7->mlie_7[1] & 0xFF) << 8 | (mlie_7->mlie_7[2] & 0xFF);
	mlie_7->MCS_MIMO_Feedback = (mlie_7->mlie_7[3] & 0xFF);

	return 0;

}

//// No definition for mlie_8


int get_mlie_9_rev(mlie_9_t* mlie_9) {	//Group Assignment IE (6.4.3.9)

	mlie_9->Single = (mlie_9->mlie_9[0] & 0x80) >> 7;
	mlie_9->Group_ID = (mlie_9->mlie_9[0] & 0x7F);
	mlie_9->Direct = (mlie_9->mlie_9[1] & 0x80) >> 7;
	mlie_9->ResourceTag = (mlie_9->mlie_9[1] & 0x7F);
	mlie_9->Direct1 = (mlie_9->mlie_9[2] & 0x80) >> 7;
	mlie_9->ResourceTag1 = (mlie_9->mlie_9[2] & 0x7F);
	mlie_9->Direct2 = (mlie_9->mlie_9[3] & 0x80) >> 7;
	mlie_9->ResourceTag2 = (mlie_9->mlie_9[3] & 0x7F);

	return 0;

}


int get_mlie_10_rev(mlie_10_t* mlie_10) {	//Load Info IE (6.4.3.10)

	mlie_10->Reserved = (mlie_10->mlie_10[0] & 0xF0) >> 4;
	mlie_10->MAX_Assoc = (mlie_10->mlie_10[0] & 0x08) >> 3;
	mlie_10->RD_PT_Load = (mlie_10->mlie_10[0] & 0x04) >> 2;
	mlie_10->RACH_Load = (mlie_10->mlie_10[0] & 0x02) >> 1;
	mlie_10->Channel_Load = (mlie_10->mlie_10[0] & 0x01);
	mlie_10->Traffic_Load_Percentage = (mlie_10->mlie_10[1] & 0xFF);
	mlie_10->MAX_n_Assoc_RDs = (mlie_10->mlie_10[2] & 0xFF) << 4 | (mlie_10->mlie_10[3] & 0xF0) >>	4;
	mlie_10->MAX_n_Assoc_RDs = (mlie_10->mlie_10[3] & 0x0F);
	mlie_10->Associated_RDs_FT = (mlie_10->mlie_10[4] & 0xFF);
	mlie_10->Associated_RDs_PT = (mlie_10->mlie_10[5] & 0xFF);
	mlie_10->RACH_Load_percentage = (mlie_10->mlie_10[6] & 0xFF);
	mlie_10->Free_subslots_detected = (mlie_10->mlie_10[7] & 0xFF);
	mlie_10->Busy_subslots_detected = (mlie_10->mlie_10[8] & 0xFF);

	return 0;

}

//// No definition for mlie_11


int get_mlie_12_rev(mlie_12_t* mlie_12) {	//Measurement Report IE (6.4.3.12)

	mlie_12->Reserved = (mlie_12->mlie_12[0] & 0xE0) >> 5;
	mlie_12->SNR = (mlie_12->mlie_12[0] & 0x10) >> 4;
	mlie_12->RSSI_2 = (mlie_12->mlie_12[0] & 0x08) >> 3;
	mlie_12->RSSI_1 = (mlie_12->mlie_12[0] & 0x04) >> 2;
	mlie_12->Tx_Count = (mlie_12->mlie_12[0] & 0x02) >> 1;
	mlie_12->RACH = (mlie_12->mlie_12[0] & 0x01);
	mlie_12->SNR_Result = (mlie_12->mlie_12[1] & 0xFF);
	mlie_12->RSSI_2_Result = (mlie_12->mlie_12[2] & 0xFF);
	mlie_12->RSSI_1_Result = (mlie_12->mlie_12[3] & 0xFF);
	mlie_12->Tx_Count_Result = (mlie_12->mlie_12[4] & 0xFF);

	return 0;

}


int get_mlie_13_rev(mlie_13_t* mlie_13) {	//Radio Device Status IE (6.4.3.13)

	mlie_13->Reserved = (mlie_13->mlie_13[0] & 0xC0) >> 6;
	mlie_13->Status_Flag = (mlie_13->mlie_13[0] & 0x30) >> 4;
	mlie_13->Duration = (mlie_13->mlie_13[0] & 0x0F);

	return 0;

}

//// No definition for mlie_14


	


