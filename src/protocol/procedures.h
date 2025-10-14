#include "../headers/mac_header_field.h"
#include "../headers/physical_header_field.h"
#include "../headers/mac_info_elements.h"
#include "../headers/mac_message.h"
#include "stdbool.h"
#ifndef PROCEDURES_H
#define PROCEDURES_H
 
/**
 * @brief Assoc Beacon Message structure
 *
 * @param phyheader (5 Bytes)
 * @param message (22 Bytes)
 * @param plcf_10 (Physical Layer Common Field - 5 Bytes)
 * @param mlcf_a (MAC Header Type - 1 Byte)
 * @param mlcf_b_2 (MAC Beacon Header - 7 Bytes)
 * @param mlcf_c_2 (MAC MUX Header - Type c - 1 Byte)
 * 
 * @param mlmf_2 (MAC Beacon Message - 13 Bytes)
 */
struct AssocBeaconMessage
{
    uint8_t phyheader[5];
    uint8_t message[22];

    plcf_10_t plcf_10;      //Physical Layer Common Field - 5 bytes  

    mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
    mlcf_b_2t mlcf_b_2;     //MAC Beacon Header - 7 bytes
    mlcf_c_2t mlcf_c_2;     //MAC MUX Header - Type c - 1 byte

    mlmf_2_t mlmf_2;        //MAC Beacon Message - 13 bytes
};

struct AssocReqMessage
{
    uint8_t phyheader[10];
    uint8_t message[26];

    plcf_20_t plcf_20;      //Physical Layer Common Field - 10 bytes 

    mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
    mlcf_b_3t mlcf_b_3;     //MAC Unicast Header - 10 bytes
    mlcf_c_2t mlcf_c_2;     //MAC MUX Header - Type c - 1 byte

    mlmf_4_t mlmf_4;        //MAC Association Request Message - 14 bytes
};

struct AssocRespMessage
{
    uint8_t phyheader[10];
    uint8_t message[36];

    plcf_20_t plcf_20;      //Physical Layer Common Field - 10 bytes  

    mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
    mlcf_b_3t mlcf_b_3;     //MAC Unicast Header - 10 bytes

    // Association Response Message
    mlcf_c_2t mlcf_c_2;     //MAC MUX Header - Type c - 1 byte
    mlmf_5_t mlmf_5;        //MAC Association Response Message - 7 bytes

    // Resource Allocation IE
    mlcf_c_2t mlcf_c_2_2;   //MAC MUX Header - Type c - 1 byte
    mlie_3_t mlie_3;        //MAC Resource Allocation IE - 16 bytes


};

struct DataMessage
{
    uint8_t phyheader[5];
    uint8_t mac_header[7];
    uint8_t data[700-7];    //Data aplicacion to transmit
    uint8_t payload[700]; 

    plcf_20_t plcf_20;      //Physical Layer Common Field - 10 bytes  

    mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
    mlcf_b_1t mlcf_b_1;     //MAC DATA PDU Header - 2 bytes
    mlcf_c_4t mlcf_c_4;     //MAC MUX Header - Type e - 4 bytes

    //Payload               //MAC Data Message - XXXXX bytes
};





struct TXParams
{
    bool reset;
    uint16_t sequenceNumber;
    uint16_t S_SRDID;
    uint32_t S_LRDID;
    uint16_t D_SRDID;
    uint32_t D_LRDID;
    uint8_t SnetworkID;
    uint32_t networkID;
    int SFN;
    bool isFT;
    bool isGW;
    int Ring_Level;
    int assignated_SFN;

    
};

struct Network
{
    uint32_t networkID;         //Full ID of the network
    uint32_t own_LRDID;         //LRDID of the RD in the network
    uint32_t network_FT;        //LRDID of the owner of the network
    uint32_t list_LRDID[10][3];    //Devices connected to that network
                                    // [i] Device 
                                    // [i][X]   X -> 0: LRDID, 1: Timer, 2: SFN
    int SFN;                    //System Frame Number
    int n_devices;              //Number of devices connected to the network
    bool isGW;                  //Is the owner a gateway
};

int txAssocBeacon(struct AssocBeaconMessage *abm, struct TXParams *tp);
int txAssocReq(struct AssocReqMessage *aRm, struct TXParams *tp);
int txAssocResp(struct AssocRespMessage *arm, struct TXParams *tp);
int txData(struct DataMessage *dm, struct TXParams *tp);
int nSubslots(int bytes, int MCS);


#endif