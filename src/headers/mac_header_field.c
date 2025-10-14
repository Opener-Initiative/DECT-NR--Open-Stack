/*
 * This code is property of Ostfalia University of Applied Sciences, Germany and Universidad Politécnica de Cartagena, Spain. 
 * Relations with QARTIA Smart Technologies, any inquiries please contact author. 
 * Based on ETSI TS 103 636-4 V1.5.1
 * Author: Iván Tomás García <ivan.tomas@edu.upct.es>
 */

#include <string.h>
#include "mac_header_field.h"



////			PACKING METHODS

	int get_mlcf_a(mlcf_a_t* mlcf_a) {    //MAC Header type (6.3.2)

		mlcf_a->mlcf_a = (mlcf_a->mlcf_a & 0x3F) | (mlcf_a->Version << 6);
		mlcf_a->mlcf_a = (mlcf_a->mlcf_a & 0xCF) | (mlcf_a->Security << 4);
		mlcf_a->mlcf_a = (mlcf_a->mlcf_a & 0xF0) | mlcf_a->HeaderType;

		return 0;
	}

	int get_mlcf_b_1(mlcf_b_1t* mlcf_b_1) {    //Common header - DATA MAC PDU Header (6.3.3.1)
		
		mlcf_b_1->mlcf_b_1[0] = (mlcf_b_1->mlcf_b_1[0] & 0x1F) | (mlcf_b_1->Reserved << 5);
		mlcf_b_1->mlcf_b_1[0] = (mlcf_b_1->mlcf_b_1[0] & 0xEF) | (mlcf_b_1->Reset << 4);
		mlcf_b_1->mlcf_b_1[0] = (mlcf_b_1->mlcf_b_1[0] & 0xF0) | (mlcf_b_1->SequenceNumber >> 8);
		mlcf_b_1->mlcf_b_1[1] = (uint8_t)(mlcf_b_1->SequenceNumber & 0xFF);

		return 0;
	}

	int get_mlcf_b_2(mlcf_b_2t* mlcf_b_2) {    //Common header - Beacon Header (6.3.3.2)

		mlcf_b_2->mlcf_b_2[0] = (uint8_t)((mlcf_b_2->NetworkID & 0xFF0000) >> 16);
		mlcf_b_2->mlcf_b_2[1] = (uint8_t)((mlcf_b_2->NetworkID & 0xFF00) >> 8);
		mlcf_b_2->mlcf_b_2[2] = (uint8_t)(mlcf_b_2->NetworkID & 0xFF);
		mlcf_b_2->mlcf_b_2[3] = (uint8_t)((mlcf_b_2->TransmitterAddress & 0xFF000000) >> 24);
		mlcf_b_2->mlcf_b_2[4] = (uint8_t)((mlcf_b_2->TransmitterAddress & 0xFF0000) >> 16);
		mlcf_b_2->mlcf_b_2[5] = (uint8_t)((mlcf_b_2->TransmitterAddress & 0xFF00) >> 8);
		mlcf_b_2->mlcf_b_2[6] = (uint8_t)(mlcf_b_2->TransmitterAddress & 0xFF);


		return 0;
	}

		int get_mlcf_b_3(mlcf_b_3t* mlcf_b_3) {    // Common header - Unicast Header (6.3.3.3)
	
		mlcf_b_3->mlcf_b_3[0] = (mlcf_b_3->mlcf_b_3[0] & 0x1F) | (mlcf_b_3->Reserved << 5);
		mlcf_b_3->mlcf_b_3[0] = (mlcf_b_3->mlcf_b_3[0] & 0xEF) | (mlcf_b_3->Reset << 4);
		mlcf_b_3->mlcf_b_3[0] = (mlcf_b_3->mlcf_b_3[0] & 0xF0) | (mlcf_b_3->MACSequence);
	
		mlcf_b_3->mlcf_b_3[1] = (uint8_t)(mlcf_b_3->SequenceNumber & 0xFF);
	
		mlcf_b_3->mlcf_b_3[2] = (uint8_t)((mlcf_b_3->ReceiverAddress & 0xFF000000) >> 24);
		mlcf_b_3->mlcf_b_3[3] = (uint8_t)((mlcf_b_3->ReceiverAddress & 0xFF0000) >> 16);
		mlcf_b_3->mlcf_b_3[4] = (uint8_t)((mlcf_b_3->ReceiverAddress & 0xFF00) >> 8);
		mlcf_b_3->mlcf_b_3[5] = (uint8_t)(mlcf_b_3->ReceiverAddress & 0xFF);
	
		mlcf_b_3->mlcf_b_3[6] = (uint8_t)((mlcf_b_3->TransmitterAddress & 0xFF000000) >> 24);
		mlcf_b_3->mlcf_b_3[7] = (uint8_t)((mlcf_b_3->TransmitterAddress & 0xFF0000) >> 16);
		mlcf_b_3->mlcf_b_3[8] = (uint8_t)((mlcf_b_3->TransmitterAddress & 0xFF00) >> 8);
		mlcf_b_3->mlcf_b_3[9] = (uint8_t)(mlcf_b_3->TransmitterAddress & 0xFF);
	
		return 0;
	}

	int get_mlcf_b_4(mlcf_b_4t* mlcf_b_4) {    //Common header - RD Broadcasting Header (6.3.3.4)

		mlcf_b_4->mlcf_b_4[0] = (mlcf_b_4->mlcf_b_4[0] & 0x1F) | (mlcf_b_4->Reserved << 5);
		mlcf_b_4->mlcf_b_4[0] = (mlcf_b_4->mlcf_b_4[0] & 0xEF) | (mlcf_b_4->Reset << 4);
		mlcf_b_4->mlcf_b_4[0] = (mlcf_b_4->mlcf_b_4[0] & 0xF0) | (mlcf_b_4->SequenceNumber >> 8);
		mlcf_b_4->mlcf_b_4[1] = (uint8_t)(mlcf_b_4->SequenceNumber & 0xFF);
		mlcf_b_4->mlcf_b_4[2] = (uint8_t)((mlcf_b_4->TransmitterAddress & 0xFF000000)>> 24);
		mlcf_b_4->mlcf_b_4[3] = (uint8_t)((mlcf_b_4->TransmitterAddress & 0xFF0000)>> 16);
		mlcf_b_4->mlcf_b_4[4] = (uint8_t)((mlcf_b_4->TransmitterAddress & 0xFF00)>> 8);
		mlcf_b_4->mlcf_b_4[5] = (uint8_t)((mlcf_b_4->TransmitterAddress& 0xFF));

		return 0;
	}

	int get_mlcf_c_1(mlcf_c_1t* mlcf_c_1) {    //Multiplexing Header - Type 1 (a & b)

		mlcf_c_1->mlcf_c_1 = ((mlcf_c_1->MAC_Ext << 6) & 0xC0) | ((mlcf_c_1->Length << 5) & 0x20) | ((mlcf_c_1->IE_Type) & 0x1F);

		return 0;
	}

	int get_mlcf_c_2(mlcf_c_2t* mlcf_c_2) {    //Multiplexing Header - Type 2 (c)

		mlcf_c_2->mlcf_c_2 = ((mlcf_c_2->MAC_Ext << 6) & 0xC0) | ((mlcf_c_2->IE_Type) & 0x3F);

		return 0;
	}

	int get_mlcf_c_3(mlcf_c_3t* mlcf_c_3) {    //Multiplexing Header - Type 3 (d)

		mlcf_c_3->mlcf_c_3[0] = ((mlcf_c_3->MAC_Ext << 6) & 0xC0) | ((mlcf_c_3->IE_Type) & 0x3F);
		mlcf_c_3->mlcf_c_3[1] = mlcf_c_3->Length;

		return 0;
	}

	int get_mlcf_c_4(mlcf_c_4t* mlcf_c_4) {    //Multiplexing Header - Type 4 (e & f)

		mlcf_c_4->mlcf_c_4[0] = ((mlcf_c_4->MAC_Ext << 6) & 0xC0) | ((mlcf_c_4->IE_Type) & 0x3F);
		mlcf_c_4->mlcf_c_4[1] = (uint8_t)((mlcf_c_4->Length >> 8) & 0xFF);
		mlcf_c_4->mlcf_c_4[2] = (uint8_t)(mlcf_c_4->Length & 0xFF);

		return 0;
	}

	int get_mlcf_c_5(mlcf_c_5t* mlcf_c_5) {    //Multiplexing Header - Type 5

		mlcf_c_5->mlcf_c_5[0] = ((mlcf_c_5->MAC_Ext << 6) & 0xC0) | ((mlcf_c_5->IE_Type) & 0x3F);
		mlcf_c_5->mlcf_c_5[1] = (uint8_t) (mlcf_c_5->Length & 0xFF)>> 8;
		mlcf_c_5->mlcf_c_5[2] = (uint8_t) mlcf_c_5->Length & 0xFF;
		mlcf_c_5->mlcf_c_5[3] = (uint8_t)mlcf_c_5->IE_Type_Extension & 0xFF;

		return 0;
	}



	////			UNPACKING METHODS


	int get_mlcf_a_rev(mlcf_a_t* mlcf_a) {			//Header type (6.3.2)

		mlcf_a->Version = (mlcf_a->mlcf_a & 0xC0) >> 6;
		mlcf_a->Security = (mlcf_a->mlcf_a & 0x30) >> 4;
		mlcf_a->HeaderType = mlcf_a->mlcf_a & 0x0F;

		return 0;
	}

	int get_mlcf_b_1_rev(mlcf_b_1t* mlcf_b_1) {			//Common header - DATA MAC PDU Header (6.3.3.1)

		mlcf_b_1->Reserved = (mlcf_b_1->mlcf_b_1[0] & 0xE0) >> 5;
		mlcf_b_1->Reset = (mlcf_b_1->mlcf_b_1[0] & 0x10) >> 4;
		mlcf_b_1->SequenceNumber = ((mlcf_b_1->mlcf_b_1[0] & 0x0F) << 8) | mlcf_b_1->mlcf_b_1[1];

		return 0;
	}

	int get_mlcf_b_2_rev(mlcf_b_2t* mlcf_b_2) {			//Common header - Beacon Header (6.3.3.2)

		mlcf_b_2->NetworkID = (uint32_t)(mlcf_b_2->mlcf_b_2[0]) << 16;
		mlcf_b_2->NetworkID = (mlcf_b_2->NetworkID & 0xFF0000) | (uint32_t)(mlcf_b_2->mlcf_b_2[1]) << 8;
		mlcf_b_2->NetworkID = (mlcf_b_2->NetworkID & 0xFFFF00) | (uint32_t)(mlcf_b_2->mlcf_b_2[2]);
		mlcf_b_2->TransmitterAddress = (uint32_t)(mlcf_b_2->mlcf_b_2[3]) << 24;
		mlcf_b_2->TransmitterAddress = (mlcf_b_2->TransmitterAddress & 0xFF000000) | (uint32_t)(mlcf_b_2->mlcf_b_2[4]) << 16;
		mlcf_b_2->TransmitterAddress = (mlcf_b_2->TransmitterAddress & 0xFFFF0000) | (uint32_t)(mlcf_b_2->mlcf_b_2[5]) << 8;
		mlcf_b_2->TransmitterAddress = (mlcf_b_2->TransmitterAddress & 0xFFFFFF00) | (uint32_t)(mlcf_b_2->mlcf_b_2[6]);
		
		return 0;
	}

	int get_mlcf_b_3_rev(mlcf_b_3t* mlcf_b_3) {			// Common header - Unicast Header (6.3.3.3)
	
		mlcf_b_3->Reserved = mlcf_b_3->mlcf_b_3[0] >> 5;
		mlcf_b_3->Reset = (mlcf_b_3->mlcf_b_3[0] >> 4) & 0x01;
		mlcf_b_3->MACSequence = mlcf_b_3->mlcf_b_3[0] & 0x0F;
		mlcf_b_3->SequenceNumber = mlcf_b_3->mlcf_b_3[1];
	
		mlcf_b_3->ReceiverAddress = (uint32_t)(mlcf_b_3->mlcf_b_3[2]) << 24;
		mlcf_b_3->ReceiverAddress |= (uint32_t)(mlcf_b_3->mlcf_b_3[3]) << 16;
		mlcf_b_3->ReceiverAddress |= (uint32_t)(mlcf_b_3->mlcf_b_3[4]) << 8;
		mlcf_b_3->ReceiverAddress |= (uint32_t)(mlcf_b_3->mlcf_b_3[5]);
	
		mlcf_b_3->TransmitterAddress = (uint32_t)(mlcf_b_3->mlcf_b_3[6]) << 24;
		mlcf_b_3->TransmitterAddress |= (uint32_t)(mlcf_b_3->mlcf_b_3[7]) << 16;
		mlcf_b_3->TransmitterAddress |= (uint32_t)(mlcf_b_3->mlcf_b_3[8]) << 8;
		mlcf_b_3->TransmitterAddress |= (uint32_t)(mlcf_b_3->mlcf_b_3[9]);
	
		return 0;
	}

	int get_mlcf_b_4_rev(mlcf_b_4t* mlcf_b_4) {				//Common header - RD Broadcasting Header (6.3.3.4)

		mlcf_b_4->Reserved = mlcf_b_4->mlcf_b_4[0] >> 5;
		mlcf_b_4->Reset = (mlcf_b_4->mlcf_b_4[0] >> 4) & 1;
		mlcf_b_4->SequenceNumber = (mlcf_b_4->mlcf_b_4[0] & 0xF) | (uint16_t)mlcf_b_4->mlcf_b_4[1];
		mlcf_b_4->TransmitterAddress = (uint32_t)(mlcf_b_4->mlcf_b_4[2]) << 24;
		mlcf_b_4->TransmitterAddress = (mlcf_b_4->TransmitterAddress & 0xFF000000) | (uint32_t)(mlcf_b_4->mlcf_b_4[3]) << 16;
		mlcf_b_4->TransmitterAddress = (mlcf_b_4->TransmitterAddress & 0xFFFF0000) | (uint32_t)(mlcf_b_4->mlcf_b_4[4]) << 8;
		mlcf_b_4->TransmitterAddress = (mlcf_b_4->TransmitterAddress & 0xFFFFFF00) | (uint32_t)(mlcf_b_4->mlcf_b_4[5]);

		return 0;
	}


	int get_mlcf_c_1_rev(mlcf_c_1t* mlcf_c_1) {    //Multiplexing Header - Type 1

		mlcf_c_1->MAC_Ext = mlcf_c_1->mlcf_c_1 >> 6;
		mlcf_c_1->Length = (mlcf_c_1->mlcf_c_1 >> 5) & 0x01;
		mlcf_c_1->IE_Type = mlcf_c_1->mlcf_c_1 & 0x1F;

		return 0;
	}

	int get_mlcf_c_2_rev(mlcf_c_2t* mlcf_c_2) {    //Multiplexing Header - Type 2

		mlcf_c_2->MAC_Ext = mlcf_c_2->mlcf_c_2 >> 6;
		mlcf_c_2->IE_Type = mlcf_c_2->mlcf_c_2 & 0x3F;

		return 0;
	}

	int get_mlcf_c_3_rev(mlcf_c_3t* mlcf_c_3) {    //Multiplexing Header - Type 3

		mlcf_c_3->MAC_Ext = mlcf_c_3->mlcf_c_3[0] >> 6;
		mlcf_c_3->IE_Type = mlcf_c_3->mlcf_c_3[0] & 0x3F;
		mlcf_c_3->Length = mlcf_c_3->mlcf_c_3[1];

		return 0;
	}

	int get_mlcf_c_4_rev(mlcf_c_4t* mlcf_c_4) {    //Multiplexing Header - Type 4

		mlcf_c_4->MAC_Ext = mlcf_c_4->mlcf_c_4[0] >> 6;
		mlcf_c_4->IE_Type = mlcf_c_4->mlcf_c_4[0] & 0x3F;
		mlcf_c_4->Length = ((uint16_t)mlcf_c_4->mlcf_c_4[1] << 8) | (uint16_t)mlcf_c_4->mlcf_c_4[2];

		return 0;
	}

	int get_mlcf_c_5_rev(mlcf_c_5t* mlcf_c_5) {    //Multiplexing Header - Type 5

		mlcf_c_5->MAC_Ext = mlcf_c_5->mlcf_c_5[0] >> 6;
		mlcf_c_5->IE_Type = mlcf_c_5->mlcf_c_5[0] & 0x3F;
		mlcf_c_5->Length = (uint16_t)mlcf_c_5->mlcf_c_5[1] << 8;
		mlcf_c_5->Length = (mlcf_c_5->Length & 0xFFFF0000) | (uint16_t)mlcf_c_5->mlcf_c_5[2];
		mlcf_c_5->IE_Type_Extension = mlcf_c_5->mlcf_c_5[3];

		return 0;
	}
