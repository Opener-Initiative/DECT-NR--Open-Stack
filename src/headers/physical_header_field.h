/*
 * This code is property of Ostfalia University of Applied Sciences, Germany and Universidad Politécnica de Cartagena, Spain. 
 * Relations with QARTIA Smart Technologies, any inquiries please contact last author. 
 * Original Author: Maxim Penner <maxim.penner@ikt.uni-hannover.de>
 * Adapted and modified by Iván Tomás García <ivan.tomas@edu.upct.es>
 */

#include <stdint.h>

#ifndef DECT2020_PHYSICAL_HEADER_FIELD_H
#define DECT2020_PHYSICAL_HEADER_FIELD_H


/**
 * @brief Physical Layer Control Field - Type 1 structure
 *
 * @param HeaderFormat (3 bits)
 * @param PacketLengthType (1 bit)
 * @param PacketLength (4 bits)
 * @param ShortNetworkID (8 bits)
 * @param TransmitterIdentity (16 bits)
 * @param TransmitPower (4 bits)
 * @param Reserved (1 bit)
 * @param DFMCS (3 bits)
 * 
 * @param plcf (5 bytes)
 */
typedef struct{
    uint8_t HeaderFormat            : 3;
    uint8_t PacketLengthType        : 1;
    uint8_t PacketLength            : 4;
    uint8_t ShortNetworkID          : 8;
    uint16_t TransmitterIdentity    : 16;
    uint8_t TransmitPower           : 4;
    uint8_t Reserved                : 1;
    uint8_t DFMCS                   : 3;

    uint8_t plcf[5];
} plcf_10_t;


/**
 * @brief Physical Layer Control Field - Type 2 structure (Header Format 000)
 *
 * @param HeaderFormat (3 bits)
 * @param PacketLengthType (1 bit)
 * @param PacketLength (4 bits)
 * @param ShortNetworkID (8 bits)
 * @param TransmitterIdentity (16 bits)
 * @param TransmitPower (4 bits)
 * @param DFMCS (4 bits)
 * @param ReceiverIdentity (16 bits)
 * @param NumberOfSpatialStreams (2 bits)
 * @param DFRedundancyVersion (2 bits)
 * @param DFNewDataIndication (1 bit)
 * @param DFHARQProcessNumber (3 bits)
 * @param FeedbackFormat (4 bits)
 * @param FeedbackInfo (12 bits)
 * 
 * @param plcf (10 bytes)
 */
typedef struct{
    uint8_t HeaderFormat            : 3;
    uint8_t PacketLengthType        : 1;
    uint8_t PacketLength            : 4;
    uint8_t ShortNetworkID          : 8;
    uint16_t TransmitterIdentity    : 16;
    uint8_t TransmitPower           : 4;
    uint8_t DFMCS                   : 4;
    uint16_t ReceiverIdentity       : 16;
    uint8_t NumberOfSpatialStreams  : 2;
    uint8_t DFRedundancyVersion     : 2;
    uint8_t DFNewDataIndication     : 1;
    uint8_t DFHARQProcessNumber     : 3;
    uint8_t FeedbackFormat          : 4;
    uint16_t FeedbackInfo           : 12;

    uint8_t plcf[10];
} plcf_20_t;


/**
 * @brief Physical Layer Control Field - Type 2 structure (Header Format 001)
 *
 * @param HeaderFormat (3 bits)
 * @param PacketLengthType (1 bit)
 * @param PacketLength (4 bits)
 * @param ShortNetworkID (8 bits)
 * @param TransmitterIdentity (16 bits)
 * @param TransmitPower (4 bits)
 * @param DFMCS (4 bits)
 * @param ReceiverIdentity (16 bits)
 * @param NumberOfSpatialStreams (2 bits)
 * @param Reserved (6 bits)
 * @param FeedbackFormat (4 bits)
 * @param FeedbackInfo (12 bits)
 * 
 * @param plcf (10 bytes)
 */
typedef struct{
    uint8_t HeaderFormat            : 3;
    uint8_t PacketLengthType        : 1;
    uint8_t PacketLength            : 4;
    uint8_t ShortNetworkID          : 8;
    uint16_t TransmitterIdentity    : 16;
    uint8_t TransmitPower           : 4;
    uint8_t DFMCS                   : 4;
    uint16_t ReceiverIdentity       : 16;
    uint8_t NumberOfSpatialStreams  : 2;
    uint8_t Reserved                : 6;
    uint8_t FeedbackFormat          : 4;
    uint16_t FeedbackInfo           : 12;

    uint8_t plcf[10];
} plcf_21_t;


int get_plcf_1(plcf_10_t* plcf_10);
int get_plcf_2(plcf_20_t* plcf_20);
int get_plcf_3(plcf_21_t* plcf_21);

int get_plcf_1_rev(plcf_10_t* plcf_10);
int get_plcf_2_rev(plcf_20_t* plcf_20);
int get_plcf_3_rev(plcf_21_t* plcf_21);

uint8_t get_transmit_power(int32_t tx_power_dBm);

uint8_t get_number_of_spatial_streams(int32_t N_SS);



#endif