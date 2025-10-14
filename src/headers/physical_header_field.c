/*
 * This code is property of Ostfalia University of Applied Sciences, Germany and Universidad Politécnica de Cartagena, Spain. 
 * Relations with QARTIA Smart Technologies, any inquiries please contact last author. 
 * Original Author: Maxim Penner <maxim.penner@ikt.uni-hannover.de>
 * Adapted and modified by Iván Tomás García <ivan.tomas@edu.upct.es>
 */

#include <string.h>
#include "physical_header_field.h"


////            PACKING METHODS 

int get_plcf_1(plcf_10_t* plcf_10){                                                       //Physical Layer Control Field Type 1 (6.2.1-1)

    plcf_10->plcf[0] = (plcf_10->plcf[0] & 0x1F) | (plcf_10->HeaderFormat << 5);
    plcf_10->plcf[0] = (plcf_10->plcf[0] & 0xEF) | (plcf_10->PacketLengthType << 4);
    plcf_10->plcf[0] = (plcf_10->plcf[0] & 0xF0) | plcf_10->PacketLength;

    plcf_10->plcf[1] = plcf_10->ShortNetworkID;

    plcf_10->plcf[2] = (uint8_t) (plcf_10->TransmitterIdentity >> 8);
    plcf_10->plcf[3] = (uint8_t) (plcf_10->TransmitterIdentity & 0xFF);

    plcf_10->plcf[4] = (plcf_10->plcf[4] & 0xF) | (plcf_10->TransmitPower << 4);
    plcf_10->plcf[4] = (plcf_10->plcf[4] & 0xF7) | (plcf_10->Reserved << 3);
    plcf_10->plcf[4] = (plcf_10->plcf[4] & 0xF8) | plcf_10->DFMCS;

    return 1;
}

int get_plcf_2(plcf_20_t* plcf_20){                                                       //Physical Layer Control Field Type 2 Header Format 000 (6.2.1-2)

    plcf_20->plcf[0] = (plcf_20->plcf[0] & 0x1F) | (plcf_20->HeaderFormat << 5);
    plcf_20->plcf[0] = (plcf_20->plcf[0] & 0xEF) | (plcf_20->PacketLengthType << 4);
    plcf_20->plcf[0] = (plcf_20->plcf[0] & 0xF0) | plcf_20->PacketLength;

    plcf_20->plcf[1] = plcf_20->ShortNetworkID;

    plcf_20->plcf[2] = (uint8_t) (plcf_20->TransmitterIdentity >> 8);
    plcf_20->plcf[3] = (uint8_t) (plcf_20->TransmitterIdentity & 0xFF);

    plcf_20->plcf[4] = (plcf_20->plcf[4] & 0xF) | (plcf_20->TransmitPower << 4);
    plcf_20->plcf[4] = (plcf_20->plcf[4] & 0xF0) | plcf_20->DFMCS;

    plcf_20->plcf[5] = (uint8_t) (plcf_20->ReceiverIdentity >> 8);
    plcf_20->plcf[6] = (uint8_t) (plcf_20->ReceiverIdentity & 0xFF);

    plcf_20->plcf[7] = (uint8_t) (plcf_20->plcf[7] & 0x3F) | (plcf_20->NumberOfSpatialStreams << 6);
    plcf_20->plcf[7] = (plcf_20->plcf[7] & 0xCF) | (plcf_20->DFRedundancyVersion << 4);
    plcf_20->plcf[7] = (plcf_20->plcf[7] & 0xF7) | (plcf_20->DFNewDataIndication << 3);
    plcf_20->plcf[7] = (plcf_20->plcf[7] & 0xF8) | plcf_20->DFHARQProcessNumber;

    plcf_20->plcf[8] = (uint8_t) (plcf_20->plcf[8] & 0xF) | (plcf_20->FeedbackFormat << 4);
    plcf_20->plcf[8] = (uint8_t) (plcf_20->plcf[8] & 0xF0) | (plcf_20->FeedbackInfo >> 8);

    plcf_20->plcf[9] = (uint8_t) (plcf_20->FeedbackInfo & 0xFF);

    return 1;
}

int get_plcf_3(plcf_21_t* plcf_21){                                                   //Physical Layer Control Field Type 2 Header Format 001 (6.2.1-2a)

    plcf_21->plcf[0] = (plcf_21->plcf[0] & 0x1F) | (plcf_21->HeaderFormat << 5);
    plcf_21->plcf[0] = (plcf_21->plcf[0] & 0xEF) | (plcf_21->PacketLengthType << 4);
    plcf_21->plcf[0] = (plcf_21->plcf[0] & 0xF0) | plcf_21->PacketLength;

    plcf_21->plcf[1] = plcf_21->ShortNetworkID;

    plcf_21->plcf[2] = (uint8_t) (plcf_21->TransmitterIdentity >> 8);
    plcf_21->plcf[3] = (uint8_t) (plcf_21->TransmitterIdentity & 0xFF);

    plcf_21->plcf[4] = (plcf_21->plcf[4] & 0xF) | (plcf_21->TransmitPower << 4);
    plcf_21->plcf[4] = (plcf_21->plcf[4] & 0xF0) | plcf_21->DFMCS;

    plcf_21->plcf[5] = (uint8_t) (plcf_21->ReceiverIdentity >> 8);
    plcf_21->plcf[6] = (uint8_t) (plcf_21->ReceiverIdentity & 0xFF);

    plcf_21->plcf[7] = (uint8_t) (plcf_21->plcf[7] & 0x3F) | (plcf_21->NumberOfSpatialStreams << 6);
    plcf_21->plcf[7] = (plcf_21->plcf[7] & 0xC0) | plcf_21->Reserved;

    plcf_21->plcf[8] = (uint8_t) (plcf_21->plcf[8] & 0xF) | (plcf_21->FeedbackFormat << 4);
    plcf_21->plcf[8] = (uint8_t) (plcf_21->plcf[8] & 0xF0) | (plcf_21->FeedbackInfo >> 8);

    plcf_21->plcf[9] = (uint8_t) (plcf_21->FeedbackInfo & 0xFF);

    return 1;
}


////            UNPACKING STRUCTURES   

int get_plcf_1_rev(plcf_10_t* plcf_10){                           //Unpacking methods for PLCF 6.2.1-1

    plcf_10->HeaderFormat        = plcf_10->plcf[0] >> 5;
    plcf_10->PacketLengthType    = (plcf_10->plcf[0] >> 4) & 1;
    plcf_10->PacketLength        = plcf_10->plcf[0] & 0xF;
    plcf_10->ShortNetworkID      = plcf_10->plcf[1];
    plcf_10->TransmitterIdentity = ((uint16_t) plcf_10->plcf[2]) << 8;
    plcf_10->TransmitterIdentity = (plcf_10->TransmitterIdentity & 0xFF00) | (uint16_t) plcf_10->plcf[3];
    plcf_10->TransmitPower       = plcf_10->plcf[4] >> 4;
    plcf_10->Reserved            = (plcf_10->plcf[4] >> 3) & 1;
    plcf_10->DFMCS               = plcf_10->plcf[4] & 7;

    return 1;
}

int get_plcf_2_rev(plcf_20_t* plcf_20){                           //Unpacking methods for PLCF 6.2.1-2

    plcf_20->HeaderFormat            = plcf_20->plcf[0] >> 5;
    plcf_20->PacketLengthType        = (plcf_20->plcf[0] >> 4) & 1;
    plcf_20->PacketLength            = plcf_20->plcf[0] & 0xF;
    plcf_20->ShortNetworkID          = plcf_20->plcf[1];
    plcf_20->TransmitterIdentity     = ((uint16_t) plcf_20->plcf[2]) << 8;
    plcf_20->TransmitterIdentity     = (plcf_20->TransmitterIdentity & 0xFF00) | (uint16_t) plcf_20->plcf[3];
    plcf_20->TransmitPower           = plcf_20->plcf[4] >> 4;
    plcf_20->DFMCS                   = plcf_20->plcf[4] & 0xF;
    plcf_20->ReceiverIdentity        = ((uint16_t) plcf_20->plcf[5]) << 8;
    plcf_20->ReceiverIdentity        = (plcf_20->ReceiverIdentity & 0xFF00) | (uint16_t) plcf_20->plcf[6];
    plcf_20->NumberOfSpatialStreams  = plcf_20->plcf[7] >> 6;
    plcf_20->DFRedundancyVersion     = (plcf_20->plcf[7] >> 4) & 3;
    plcf_20->DFNewDataIndication     = (plcf_20->plcf[7] >> 3) & 1;
    plcf_20->DFHARQProcessNumber     = plcf_20->plcf[7] & 7;
    plcf_20->FeedbackFormat          = (plcf_20->plcf[8] >> 4) & 0xF;
    plcf_20->FeedbackInfo            = (uint16_t) (plcf_20->plcf[8] & 0xF) << 8;
    plcf_20->FeedbackInfo            = (plcf_20->FeedbackInfo & 0xFF00) | (uint16_t) plcf_20->plcf[9];

    return 1;
}

int get_plcf_3_rev(plcf_21_t* plcf_21){                       //Unpacking methods for PLCF 6.2.1-2a

    plcf_21->HeaderFormat            = plcf_21->plcf[0] >> 5;
    plcf_21->PacketLengthType        = (plcf_21->plcf[0] >> 4) & 1;
    plcf_21->PacketLength            = plcf_21->plcf[0] & 0xF;
    plcf_21->ShortNetworkID          = plcf_21->plcf[1];
    plcf_21->TransmitterIdentity     = ((uint16_t) plcf_21->plcf[2]) << 8;
    plcf_21->TransmitterIdentity     = (plcf_21->TransmitterIdentity & 0xFF00) | (uint16_t) plcf_21->plcf[3];
    plcf_21->TransmitPower           = plcf_21->plcf[4] >> 4;
    plcf_21->DFMCS                   = plcf_21->plcf[4] & 0xF;
    plcf_21->ReceiverIdentity        = ((uint16_t) plcf_21->plcf[5]) << 8;
    plcf_21->ReceiverIdentity        = (plcf_21->ReceiverIdentity & 0xFF00) | (uint16_t) plcf_21->plcf[6];
    plcf_21->NumberOfSpatialStreams  = plcf_21->plcf[7] >> 6;
    plcf_21->Reserved                = plcf_21->plcf[7] & 0x3F;
    plcf_21->FeedbackFormat          = (plcf_21->plcf[8] >> 4) & 0xF;
    plcf_21->FeedbackInfo            = (uint16_t) (plcf_21->plcf[8] & 0xF) << 8;
    plcf_21->FeedbackInfo            = (plcf_21->FeedbackInfo & 0xFF00) | (uint16_t) plcf_21->plcf[9];

    return 1;
}

uint8_t get_transmit_power(int32_t tx_power_dBm){           //Method to get the transmit power index from a tx power value in dBm

    uint32_t ret = 15;

    int32_t tx_power_table[16] = {-40, -30, -20, -10, -6, -3, 0, 3, 6, 10, 14, 19, 23, 26, 29, 32};

    // find closest value
    int32_t dist_min = 1000000;
    int32_t dist;
    for(uint32_t i=0; i<16; i++){
        dist = (tx_power_dBm - tx_power_table[i]);
        dist *= dist;

        if(dist <= dist_min){
            dist_min = dist;
            ret = (uint8_t) i;
        }
    }

    return ret;
}

uint8_t get_number_of_spatial_streams(int32_t N_SS){       //Method to get the number of spatial streams index from a N_SS value            
    if(N_SS == 1)
        return 0;
    else if(N_SS == 2)
        return 1;
    else if(N_SS == 4)
        return 2;
    else if(N_SS == 8)
        return 3;

    return 0;
}
