#include <stdint.h>
#include <string.h>
#include "../headers/physical_header_field.h"
#include "../headers/mac_header_field.h"
#include "../headers/mac_message.h"
#include "../config/device_config.h"
#include "procedures.h"



int txAssocBeacon(struct AssocBeaconMessage *abm, struct TXParams *tp){
    
    // uint8_t phyheader[5];
    // uint8_t message[22];
    // plcf_10_t plcf_10;      //Physical Layer Common Field - 5 bytes  
    // mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
    // mlcf_b_2t mlcf_b_2;     //MAC Beacon Header - 7 bytes
    // mlcf_c_2t mlcf_c_2;     //MAC MUX Header - Type c - 1 byte
    // mlmf_1_t mlmf_1;        //MAC Beacon Message - 13 bytes


    // Assemble physical header
    abm -> plcf_10.HeaderFormat = 0;
    abm->plcf_10.PacketLengthType = 0;
    abm->plcf_10.PacketLength = 7;
    abm->plcf_10.ShortNetworkID = tp->SnetworkID;
    // abm->plcf_10.ShortNetworkID = 5;
    // abm->plcf_10.ShortNetworkID = (uint8_t)0x0a;
    abm->plcf_10.TransmitterIdentity = (uint16_t)tp->S_SRDID;
    abm->plcf_10.TransmitPower = (uint8_t)0xb;
    abm->plcf_10.Reserved = 0;
    abm->plcf_10.DFMCS = 4;

    // Assemble MAC header
    abm->mlcf_a.Version = 0;
    abm->mlcf_a.Security = 0;
    abm->mlcf_a.HeaderType = 1;

    // Assemble MAC header for chosen type
    abm->mlcf_b_2.NetworkID = (tp->networkID >> 8) & 0xFFFFFF;
    abm->mlcf_b_2.TransmitterAddress = tp->S_LRDID;

    // Assemble corresponding MAC MUX header
    abm->mlcf_c_2.MAC_Ext = 0b00;
    abm->mlcf_c_2.IE_Type = 0b001000;

    // Assemble the beacon message
    abm->mlmf_2.Reserved = tp->isGW;
    abm->mlmf_2.TX_Power = 1;
    abm->mlmf_2.Power_Const = 0;
    abm->mlmf_2.Current = 1;
    abm->mlmf_2.Network_Beacon_Channels = 1;
    abm->mlmf_2.Network_Beacon_Period = 7;
    abm->mlmf_2.Cluster_Beacon_Period = 0;
    abm->mlmf_2.Reserved_1 = tp->Ring_Level;
    abm->mlmf_2.Next_Cluster_Channel = 1;
    abm->mlmf_2.TTn = 0;
    abm->mlmf_2.Reserved_2 = 0;
    abm->mlmf_2.Clusters_Max_TX_Power = 1;
    abm->mlmf_2.Reserved_3 = 0;
    abm->mlmf_2.Current_Cluster_Channel = 0;
    abm->mlmf_2.Reserved_4 = 0;
    abm->mlmf_2.ANBC = 0;


    // printk("Reserved (Si es o no Gateway): %x\n", abm->mlmf_1.Reserved);

    // Get assembled physical header
    get_plcf_1(&abm->plcf_10);
    // Get assembled MAC header
    get_mlcf_a(&abm->mlcf_a);
    // Get assembled MAC header for chosen type
    get_mlcf_b_2(&abm->mlcf_b_2);
    // Get assembled MAC MUX header
    get_mlcf_c_2(&abm->mlcf_c_2);
    // Get assembled beacon message
    get_mlmf_2(&abm->mlmf_2);
    // Copy physical header into buffer
    memcpy(abm->phyheader, &abm->plcf_10.plcf, 5);
    // Copy MAC header into payload
    memcpy(abm->message, &abm->mlcf_a.mlcf_a, 1);
    // Copy MAC header for chosen type into payload
    memcpy(abm->message + 1, &abm->mlcf_b_2.mlcf_b_2, 7);
    // Copy MAC MUX header into payload
    memcpy(abm->message + 8, &abm->mlcf_c_2.mlcf_c_2, 1);
    // Copy beacon message into payload
    memcpy(abm->message + 9, &abm->mlmf_2.mlmf_2, 13);

    // printk("Transmitted ShortNetwork ID: %x\n", abm->phyheader[1]);
    // printk("Transmitted Network ID: %x\n", abm->mlcf_b_2.NetworkID);
    // printk("Transmitted LRDID: %x\n", abm->mlcf_b_2.TransmitterAddress);

    return 1;
}

int txAssocReq(struct AssocReqMessage *aRm, struct TXParams *tp){

  // uint8_t phyheader[10];
  // uint8_t message[26];
  // plcf_20_t plcf_20;      //Physical Layer Common Field - 10 bytes  
  // mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
  // mlcf_b_3t mlcf_b_3;     //MAC Unicast Header - 10 bytes
  // mlcf_c_2t mlcf_c_2;     //MAC MUX Header - Type c - 1 byte
  // mlmf_3_t mlmf_3;        //MAC Association Request Message - 14 bytes

  // Assemble physical header
  aRm->plcf_20.HeaderFormat = 0;
  aRm->plcf_20.PacketLengthType = 0;
  aRm->plcf_20.PacketLength = 7;
  aRm->plcf_20.ShortNetworkID = tp->SnetworkID;
  aRm->plcf_20.TransmitterIdentity = tp->S_SRDID;
  aRm->plcf_20.TransmitPower = (uint8_t)0xb;
  aRm->plcf_20.DFMCS = 4;
  aRm->plcf_20.ReceiverIdentity = tp->D_SRDID;
  aRm->plcf_20.NumberOfSpatialStreams = 0;
  aRm->plcf_20.DFRedundancyVersion = 0;
  aRm->plcf_20.DFNewDataIndication = 0;
  aRm->plcf_20.DFHARQProcessNumber = 0;
  aRm->plcf_20.FeedbackFormat = 0;
  aRm->plcf_20.FeedbackInfo = 0;

  // printk("Transmitting Assoc Req to LRDID: %x\n", tp->D_SRDID);
  // printk("Transmitting Assoc Req from LRDID: %x\n", tp->S_SRDID);
  // printk("Transmitting Assoc Req to ShortNetworkID: %x\n", tp->SnetworkID);
  // printk("Transmitting Assoc Req from NetworkID: %x\n", tp->networkID);

  // Assemble MAC header
  aRm->mlcf_a.Version = 0;
  aRm->mlcf_a.Security = 0;
  aRm->mlcf_a.HeaderType = 2;

  // Assemble MAC header for chosen type
  aRm->mlcf_b_3.Reserved = 0;
  aRm->mlcf_b_3.Reset = 1;
  aRm->mlcf_b_3.MACSequence = 1;
  aRm->mlcf_b_3.SequenceNumber = 0;
  aRm->mlcf_b_3.ReceiverAddress = tp->D_LRDID;
  aRm->mlcf_b_3.TransmitterAddress = tp->S_LRDID;

  // Assemble corresponding MAC MUX header
  aRm->mlcf_c_2.MAC_Ext = 0b00;
  aRm->mlcf_c_2.IE_Type = 0b001010;

  // Assemble association message
  aRm->mlmf_4.Setup_Cause = 0;
  aRm->mlmf_4.N_Flows = 1;
  aRm->mlmf_4.Power_Const = 1;
  aRm->mlmf_4.FT_Mode = 0;
  if(tp->isFT) aRm->mlmf_4.FT_Mode = 1;
  aRm->mlmf_4.Current = 0;
  aRm->mlmf_4.Reserved = 0;
  aRm->mlmf_4.HARQ_P_TX = 0;
  aRm->mlmf_4.MAX_HARQ_Re_TX = 0;
  aRm->mlmf_4.HARQ_P_RX = 0;
  aRm->mlmf_4.MAX_HARQ_Re_RX = 0;
  aRm->mlmf_4.Reserved_1 = 0;
  aRm->mlmf_4.FlowID = 0;
  aRm->mlmf_4.Network_Beacon_Period = 0;
  aRm->mlmf_4.Cluster_Beacon_Period = 0;
  aRm->mlmf_4.Reserved_2 = 0;
  aRm->mlmf_4.Next_Cluster_Channel = 0;
  aRm->mlmf_4.TTn = 0;
  aRm->mlmf_4.Reserved_3 = 0;
  aRm->mlmf_4.Current_Cluster_Channel = 0;


  // Get assembled physical header
  get_plcf_2(&aRm->plcf_20);
  // Get assembled MAC header
  get_mlcf_a(&aRm->mlcf_a);
  // Get assembled MAC header for chosen type
  get_mlcf_b_3(&aRm->mlcf_b_3);
  // Get assembled MAC MUX header
  get_mlcf_c_2(&aRm->mlcf_c_2);
  // Get assembled association message
  get_mlmf_4(&aRm->mlmf_4);
  // Copy physical header into buffer
  memcpy(aRm->phyheader, &aRm->plcf_20.plcf, 10);
  // Copy MAC header into payload
  memcpy(aRm->message, &aRm->mlcf_a.mlcf_a, 1);
  // Copy MAC header for chosen type into payload
  memcpy(aRm->message + 1, &aRm->mlcf_b_3.mlcf_b_3, 10);
  // Copy MAC MUX header into payload
  memcpy(aRm->message + 11, &aRm->mlcf_c_2.mlcf_c_2, 1);
  // Copy association message into payload
  memcpy(aRm->message + 12, &aRm->mlmf_4.mlmf_4, 14);
  // //Send everything (commented out)
  // modem_tx(aRm->message, 25, &aRm->phyheader, 5);

  return 1;

}

int txAssocResp(struct AssocRespMessage *arm, struct TXParams *tp){

  // uint8_t phyheader[5];
  // uint8_t message[36];
  // plcf_20_t plcf_20;      //Physical Layer Common Field - 10 bytes  
  // mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
  // mlcf_b_3t mlcf_b_3;     //MAC Unicast Header - 10 bytes
  // mlcf_c_2t mlcf_c_2;     //MAC MUX Header - Type c - 1 byte
  // mlmf_4_t mlmf_4;        //MAC Association Response Message - 7 bytes
  // mlcf_c_2t mlcf_c_2_2;   //MAC MUX Header - Type c - 1 byte
  // mlie_3_t mlie_3;        //MAC Resource Allocation IE - 16 bytes

  

  // Assemble physical header
  arm->plcf_20.HeaderFormat = 0;
  arm->plcf_20.PacketLengthType = 0;
  arm->plcf_20.PacketLength = 7;
  arm->plcf_20.ShortNetworkID = tp->SnetworkID;
  arm->plcf_20.TransmitterIdentity = (uint16_t)tp->S_SRDID;
  arm->plcf_20.TransmitPower = (uint8_t)0xb;
  arm->plcf_20.DFMCS = 4;
  arm->plcf_20.ReceiverIdentity = tp->D_SRDID;
  arm->plcf_20.NumberOfSpatialStreams = 0;
  arm->plcf_20.DFRedundancyVersion = 0;
  arm->plcf_20.DFNewDataIndication = 0;
  arm->plcf_20.DFHARQProcessNumber = 0;
  arm->plcf_20.FeedbackFormat = 0;
  arm->plcf_20.FeedbackInfo = 0;

  // printk("Transmitting Assoc Resp to LRDID: %x\n", tp->D_SRDID);
  // printk("Transmitting Assoc Resp from LRDID: %x\n", tp->S_SRDID);

  // Assemble MAC header
  arm->mlcf_a.Version = 0;
  arm->mlcf_a.Security = 0;
  arm->mlcf_a.HeaderType = 2;

  // Assemble MAC header for chosen type
  arm->mlcf_b_3.Reserved = 0;
  arm->mlcf_b_3.Reset = 0;
  arm->mlcf_b_3.MACSequence = 0;
  arm->mlcf_b_3.SequenceNumber = 0;
  arm->mlcf_b_3.ReceiverAddress = tp->D_LRDID;
  arm->mlcf_b_3.TransmitterAddress = tp->S_LRDID;

  // printk("In sendAssocResp Receiver LRDID: %x\n", arm->mlcf_b_3.ReceiverAddress);
  // printk("In sendAssocResp Transmitter LRDID: %x\n", arm->mlcf_b_3.TransmitterAddress);

  // Assemble corresponding MAC MUX header
  arm->mlcf_c_2.MAC_Ext = 0;
  arm->mlcf_c_2.IE_Type = 0b001011;

  // Assemble association response message
  arm->mlmf_5.ACK_NACK = 1;
  arm->mlmf_5.Reserved = 0;
  arm->mlmf_5.HARQ_mod = 0;
  arm->mlmf_5.NFlows = 0;
  arm->mlmf_5.Group = 0;
  arm->mlmf_5.TX_Power = 0;
  arm->mlmf_5.Reject_Cause = 0;
  arm->mlmf_5.Reject_Time = 0;
  arm->mlmf_5.HARQ_P_RX = 0;
  arm->mlmf_5.MAX_HARQ_Re_RX = 0;
  arm->mlmf_5.HARQ_P_TX = 0;
  arm->mlmf_5.MAX_HARQ_Re_TX = 0;
  arm->mlmf_5.Reserved_1 = 0;
  arm->mlmf_5.Flow_ID = 8;
  arm->mlmf_5.Reserved_2 = 0;
  arm->mlmf_5.Group_ID = 0;
  arm->mlmf_5.Reserved_3 = 0;
  arm->mlmf_5.Resource_TAG = 0;

  // Assemble another MAC MUX header for coordinating channel with Scheduled Access
  arm->mlcf_c_2_2.MAC_Ext = 0;
  arm->mlcf_c_2_2.IE_Type = 0b010010;

  // Assemble the Resource Allocation IE
  arm->mlie_3.Allocation_Type = 0b10;           // Uplink only, configured responding 
  arm->mlie_3.Add = 0;                          // New configuration
  arm->mlie_3.ID = 1;                           // ID present
  arm->mlie_3.SFN_Value = tp->SFN;              // SFN value


  // Get assembled physical header
  get_plcf_2(&arm->plcf_20);
  // Get assembled MAC header
  get_mlcf_a(&arm->mlcf_a);
  // Get assembled MAC header for chosen type
  get_mlcf_b_3(&arm->mlcf_b_3);
  // Get assembled MAC MUX header
  get_mlcf_c_2(&arm->mlcf_c_2);
  // Get assembled association response message
  get_mlmf_5(&arm->mlmf_5);
  // Get MAC MUX header for coordinating channel with Scheduled Access
  get_mlcf_c_2(&arm->mlcf_c_2_2);
  // Get Resource Allocation IE
  get_mlie_3(&arm->mlie_3);
  

  // Copy physical header into buffer
  memcpy(arm->phyheader, &arm->plcf_20.plcf, 10);
  // Copy MAC header into payload
  memcpy(arm->message, &arm->mlcf_a.mlcf_a, 1);
  // Copy MAC header for chosen type into payload
  memcpy(arm->message + 1, &arm->mlcf_b_3.mlcf_b_3, 10);
  // Copy MAC MUX header into payload
  memcpy(arm->message + 11, &arm->mlcf_c_2.mlcf_c_2, 1);
  // Copy message into payload
  memcpy(arm->message + 12, &arm->mlmf_5.mlmf_5, 7);
  // Copy MAC MUX header content for coordinating channel with Scheduled Access
  memcpy(arm->message + 19, &arm->mlcf_c_2_2.mlcf_c_2, 1);
  // Copy Resource Allocation IE content
  memcpy(arm->message + 20, &arm->mlie_3.mlie_3, 16);
  // Send everything (commented out)
  // modem_tx(arm->message, 18, &arm->phyheader, 5);

  return 0;

}

int txData(struct DataMessage *dm, struct TXParams *tp){
    
  // uint8_t phyheader[5];
  // uint8_t message[19];
  // plcf_10_t plcf_10;      //Physical Layer Common Field - 5 bytes  
  // mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
  // mlcf_b_1t mlcf_b_1;     //MAC DATA PDU Header - 2 bytes
  // mlcf_c_t mlcf_c_4;     //MAC MUX Header - Type e - 4 bytes
                            //MAC Data Message - XXXXX bytes

  // Assemble physical header
    
  dm->plcf_10.HeaderFormat = 0;
  dm->plcf_10.PacketLengthType = 0;
  dm->plcf_10.PacketLength = 7;
  dm->plcf_10.ShortNetworkID = (uint8_t)tp->SnetworkID;
  dm->plcf_10.TransmitterIdentity = (uint16_t)tp->S_SRDID;
  dm->plcf_10.TransmitPower = (uint8_t)0xb;
  // dm->plcf_10.ReceiverIdentity = tp->D_SRDID;
  dm->plcf_10.DFMCS = 4;

  // Assemble MAC header
  dm->mlcf_a.Version = 0;
  dm->mlcf_a.Security = 0;
  dm->mlcf_a.HeaderType = 0;  

  // Assemble MAC header for chosen type
  if(IS_GATEWAY)    dm->mlcf_b_1.Reserved = 0;
  else              dm->mlcf_b_1.Reserved = tp->SFN;
  dm->mlcf_b_1.Reset = tp->reset;
  dm->mlcf_b_1.SequenceNumber = tp->sequenceNumber;

  // Assemble corresponding MAC MUX header
  dm->mlcf_c_4.MAC_Ext = 0b10;
  dm->mlcf_c_4.IE_Type = 0b000011;
  dm->mlcf_c_4.Length = 693;

  // Application data
  // dm->payload;




  // Get assembled physical header
  get_plcf_1(&dm->plcf_10);
  // Get assembled MAC header
  get_mlcf_a(&dm->mlcf_a);
  // Get assembled MAC header for chosen type
  get_mlcf_b_1(&dm->mlcf_b_1);
  // Get assembled MAC MUX header
  get_mlcf_c_4(&dm->mlcf_c_4);
  // Copy physical header into buffer
  memcpy(dm->phyheader, &dm->plcf_10.plcf, 5);
  // Copy MAC header into payload
  memcpy(dm->mac_header, &dm->mlcf_a.mlcf_a, 1);
  // Copy MAC header for chosen type into payload
  memcpy(dm->mac_header + 1, &dm->mlcf_b_1.mlcf_b_1, 2);
  // Copy MAC MUX header into payload
  memcpy(dm->mac_header + 3, &dm->mlcf_c_4.mlcf_c_4, 4);
  // Copy MAC header into buffer
  memcpy(dm->payload, &dm->mac_header, 7);
  // Copy data into buffer
  memcpy(dm->payload + 7, &dm->data, 700-7);

  return 1;



}


int txBroIndIE(struct BroIndIE *bim, struct TXParams *tp){
    
  // plcf_10_t plcf_10;
  // mlcf_a_t mlcf_a;        // MAC Header Type - 1 byte
  // mlcf_b_1t mlcf_b_1;     // DATA MAC PDU Header - 2 bytes
  // mlcf_c_2t mlcf_c_2;      // MAC MUX Header - Type c - 1 bytes
  // mlie_7_t mlie_7;        // MAC Broadcast Indication IE - 4 bytes



  // Assemble physical header
  bim->plcf_10.HeaderFormat = 0;
  bim->plcf_10.PacketLengthType = 0;
  bim->plcf_10.PacketLength = 7;
  bim->plcf_10.ShortNetworkID = tp->SnetworkID;
  bim->plcf_10.TransmitterIdentity = (uint16_t)tp->S_SRDID;
  bim->plcf_10.TransmitPower = (uint8_t)0xb;
  bim->plcf_10.DFMCS = 4;


  // Assemble MAC header
  bim->mlcf_a.Version = 0;
  bim->mlcf_a.Security = 0;
  bim->mlcf_a.HeaderType = 0;

  // Assemble MAC header for chosen type
  bim->mlcf_b_1.Reserved = 0;
  bim->mlcf_b_1.Reset = tp->reset;
  bim->mlcf_b_1.SequenceNumber = 0;

  // Assemble corresponding MAC MUX header
  bim->mlcf_c_2.MAC_Ext = 0b00;
  bim->mlcf_c_2.IE_Type = 0b010110;

  // Assemble Broadcast Indication message
  bim->mlie_7.Indication_Type = 0;      // Paging
  bim->mlie_7.IDType = 0;               // Short
  bim->mlie_7.ACK_NACK = 0;             
  bim->mlie_7.Feedback = 0;
  bim->mlie_7.Resource_Allocation = 0;
  bim->mlie_7.LongShort_RDID = tp->D_SRDID; // Depende de si se quiere largo o corto
  bim->mlie_7.MCS_MIMO_Feedback = 0;

  // Get assembled physical header
  get_plcf_1(&bim->plcf_10);
  // Get assembled MAC header
  get_mlcf_a(&bim->mlcf_a);
  // Get assembled MAC header for chosen type
  get_mlcf_b_1(&bim->mlcf_b_1);
  // Get assembled MAC MUX header
  get_mlcf_c_2(&bim->mlcf_c_2);
  // Copy physical header into buffer
  memcpy(bim->phyheader, &bim->plcf_10.plcf, 5);
  // Copy MAC header into payload
  memcpy(bim->mac_header, &bim->mlcf_a.mlcf_a, 1);
  // Copy MAC header for chosen type into payload
  memcpy(bim->mac_header + 1, &bim->mlcf_b_1.mlcf_b_1, 2);
  // Copy MAC MUX header into payload
  memcpy(bim->mac_header + 3, &bim->mlcf_c_2.mlcf_c_2, 1);
  // Copy MAC MUX content into payload
  memcpy(bim->mac_header + 4, &bim->mlie_7.mlie_7, 4);
  // Copy MAC header into buffer
  memcpy(bim->payload, &bim->mac_header, 9);
  // Copy network info into payload if present
  // memcpy(bim->payload + 14, &bim->message, 700-14);

  // printk("Payload: %s\n", bim->payload);

  return 1;
}


int txKAm_IE(struct KAm *kam, struct TXParams *tp){
    
  // uint8_t phyheader[10];
  // uint8_t message[4];
  // plcf_20_t plcf_20;      //Physical Layer Common Field - 10 bytes  
  // mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
  // mlcf_b_1t mlcf_b_1;     //DATA MAC PDU Header - 2 bytes
  // mlcf_c_1t mlcf_c_1;     //MAC MUX Header - Type c - 1 byte


  // Assemble physical header
  kam->plcf_20.HeaderFormat = 1;
  kam->plcf_20.PacketLengthType = 0;
  kam->plcf_20.PacketLength = 7;
  kam->plcf_20.ShortNetworkID = tp->SnetworkID;
  kam->plcf_20.TransmitterIdentity = (uint16_t)tp->S_SRDID;
  kam->plcf_20.TransmitPower = (uint8_t)0xb;
  kam->plcf_20.DFMCS = 4;
  kam->plcf_20.ReceiverIdentity = (uint16_t)tp->D_SRDID;
  kam->plcf_20.NumberOfSpatialStreams = 0;
  kam->plcf_20.DFRedundancyVersion = 0;
  kam->plcf_20.DFNewDataIndication = 0;
  kam->plcf_20.DFHARQProcessNumber = 0;
  kam->plcf_20.FeedbackFormat = 0;
  kam->plcf_20.FeedbackInfo = 0;


  // Assemble MAC header
  kam->mlcf_a.Version = 0;
  kam->mlcf_a.Security = 0;
  kam->mlcf_a.HeaderType = 0;

  // Assemble MAC header for chosen type
  kam->mlcf_b_1.Reserved = 0;
  kam->mlcf_b_1.Reset = tp->reset;
  kam->mlcf_b_1.SequenceNumber = 0;

  // Assemble corresponding MAC MUX header
  kam->mlcf_c_1.MAC_Ext = 0b11;
  kam->mlcf_c_1.Length = 0;
  kam->mlcf_c_1.IE_Type = 0b00010;

  // Get assembled physical header
  get_plcf_2(&kam->plcf_20);
  // Get assembled MAC header
  get_mlcf_a(&kam->mlcf_a);
  // Get assembled MAC header for chosen type
  get_mlcf_b_1(&kam->mlcf_b_1);
  // Get assembled MAC MUX header
  get_mlcf_c_1(&kam->mlcf_c_1);
  // Copy physical header into buffer
  memcpy(kam->phyheader, &kam->plcf_20.plcf, 10);
  // Copy MAC header into payload
  memcpy(kam->mac_header, &kam->mlcf_a.mlcf_a, 1);
  // Copy MAC header for chosen type into payload
  memcpy(kam->mac_header + 1, &kam->mlcf_b_1.mlcf_b_1, 2);
  // Copy MAC MUX header into payload
  memcpy(kam->mac_header + 3, &kam->mlcf_c_1.mlcf_c_1, 1);

  
  // Copy MAC header into buffer
  memcpy(kam->payload, &kam->mac_header, 4);
  // Copy network info into payload if present
  memcpy(kam->payload + 4, &kam->message, 700-4);
  

  return 1;
}

int txMetricReq(struct MetricMessage *mr, struct TXParams *tp){

  // uint8_t phyheader[5];
  // uint8_t message[3];
  // plcf_10_t plcf_10;      //Physical Layer Common Field - 10 bytes  
  // mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
  // mlcf_b_1t mlcf_b_1;     //DATA MAC PDU Header - 2 bytes


  // Assemble physical header
  mr->plcf_10.HeaderFormat = 0;
  mr->plcf_10.PacketLengthType = 0;
  mr->plcf_10.PacketLength = 7;
  mr->plcf_10.ShortNetworkID = tp->SnetworkID;
  mr->plcf_10.TransmitterIdentity = (uint16_t)tp->S_SRDID;
  mr->plcf_10.TransmitPower = (uint8_t)0xb;
  mr->plcf_10.DFMCS = 4;


  // Assemble MAC header
  mr->mlcf_a.Version = 0;
  mr->mlcf_a.Security = 0;
  mr->mlcf_a.HeaderType = 0;

  // Assemble MAC header for chosen type
  // mr->mlcf_b_1.Reserved = 0;               // Modified in main.c
  // printk("Reserved in sendMetricReq: %x\n", mr->mlcf_b_1.Reserved);
  mr->mlcf_b_1.Reset = tp->reset;
  mr->mlcf_b_1.SequenceNumber = tp->sequenceNumber;

  // Get assembled physical header
  get_plcf_1(&mr->plcf_10);
  // Get assembled MAC header
  get_mlcf_a(&mr->mlcf_a);
  // Get assembled MAC header for chosen type
  get_mlcf_b_1(&mr->mlcf_b_1);

  // Copy physical header into buffer
  memcpy(mr->phyheader, &mr->plcf_10.plcf, 5);
  // Copy MAC header into payload
  memcpy(mr->mac_header, &mr->mlcf_a.mlcf_a, 1);
  // Copy MAC header for chosen type into payload
  memcpy(mr->mac_header + 1, &mr->mlcf_b_1.mlcf_b_1, 2);
  
  // Copy MAC header into buffer
  memcpy(mr->payload, &mr->mac_header, 3);
  // Copy network info into payload if present
  memcpy(mr->payload + 3, &mr->message, 700-3);


}

int getStatusDevNet(struct StatusDevNet *sdn)
{
  // printk("getStatusDevNet\n");
  // printk("isFT: %d\n", sdn->IsFT);
  // printk("isGW: %d\n", sdn->IsGW);
  // printk("DataLength: %d\n", sdn->DataLength);
  sdn->sdn[0] = (sdn->sdn[0] & 0x7F) | (sdn->IsFT << 7);
  sdn->sdn[0] = (sdn->sdn[0] & 0xBF) | (sdn->IsGW << 6);
  sdn->sdn[0] = (sdn->sdn[0] & 0xC0) | (sdn->DataLength);
  sdn->sdn[1] = (sdn->NetworkID >> 24) & 0xFF;
  sdn->sdn[2] = (sdn->NetworkID >> 16) & 0xFF;
  sdn->sdn[3] = (sdn->NetworkID >> 8) & 0xFF;
  sdn->sdn[4] = sdn->NetworkID & 0xFF;
    
  for (int i = 0; i < sdn->n_devices+1; i++)
  {
    sdn->sdn[5 + i * 4] = (sdn->LRDID[i] >> 24) & 0xFF;
    sdn->sdn[6 + i * 4] = (sdn->LRDID[i] >> 16) & 0xFF;
    sdn->sdn[7 + i * 4] = (sdn->LRDID[i] >> 8) & 0xFF;
    sdn->sdn[8 + i * 4] = sdn->LRDID[i] & 0xFF;
  }

  // printk("StatusDevNet: %x\n", sdn->sdn[0]);
  // printk("NetworkID (Unpacked): %x\n", sdn->NetworkID);
  // printk("NetworkID (Packed): %x\n", sdn->sdn[1]);
  // printk("NetworkID: %x\n", sdn->sdn[2]);
  // printk("NetworkID: %x\n", sdn->sdn[3]);
  // printk("NetworkID: %x\n", sdn->sdn[4]);

  // for(int i = 5; i < 5 + (sdn->n_devices+1) * 4; i++)
  // {
  //   printk("LRDID (%d): %x\n", i-5, sdn->sdn[i]);
  // }

  return 1;
}


int setStatusDevNet(struct StatusDevNet *sdn)
{
    // Set StatusDevNet fields from the data buffer

    sdn->IsFT = (sdn->sdn[0] >> 7) & 0x01;
    sdn->IsGW = (sdn->sdn[0] >> 6) & 0x01;
    sdn->DataLength = sdn->sdn[0] & 0x3F;
    sdn->NetworkID = (sdn->sdn[1] << 24) | (sdn->sdn[2] << 16) | (sdn->sdn[3] << 8) | sdn->sdn[4];

    // Calculate the number of devices
    sdn->n_devices = sdn->DataLength;

    // printk("DataLength: %d\n", sdn->DataLength);

    // Set LRDID values from the data buffer
    for (int i = 0; i < sdn->n_devices+1; i++) {
        sdn->LRDID[i] = (sdn->sdn[5 + i * 4] << 24) | (sdn->sdn[6 + i * 4] << 16)
         | (sdn->sdn[7 + i * 4] << 8) | sdn->sdn[8 + i * 4];
    }

    // // Print extracted data
    // printk("IsFT: %x\n", sdn->IsFT);
    // printk("IsGW: %x\n", sdn->IsGW);
    // printk("DataLength: %x\n", sdn->DataLength);
    // printk("NetworkID: %x\n", sdn->NetworkID);

    // for (int i = 0; i < sdn->n_devices+1; i++) {
    //   printk("LRDID[%d]: %x\n", i, sdn->LRDID[i]);
    // }

    return 0;
}



int nSubslots(int bytes, int MCS){
  
  int nSubslots = 0;

  int data[5][15] = {
    {0, 136, 264, 400, 536, 664, 792, 920, 1064, 1192, 1320, 1448, 1576, 1704, 1864, 1992},
    {32, 296, 552, 824, 1096, 1352, 1608, 1864, 2104, 2360, 2616, 2872, 3128, 3384, 3704, 3960},
    {56, 456, 856, 1256, 1640, 2024, 2360, 2744, 3192, 3576, 3960, 4320, 4768, 5152, 5536, -1},
    {88, 616, 1128, 1672, 2168, 2680, 3192, 3704, 4256, 4768, 5280, -1, -1, -1, -1, -1},
    {144, 936, 1736, 2488, 3256, 4024, 4832, 5600, -1, -1, -1, -1, -1, -1, -1, -1}
  };
  
  for (int i = 0; i < 15; i++) {
    if (data[MCS][i] == bytes * 8) {
      nSubslots = i+1;
      break;
    }
  }


  nSubslots = data[MCS][bytes*8];
  

  return nSubslots;
}

