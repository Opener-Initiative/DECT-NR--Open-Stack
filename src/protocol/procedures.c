#include <stdint.h>
#include <string.h>
#include "../headers/physical_header_field.h"
#include "../headers/mac_header_field.h"
#include "../headers/mac_message.h"
#include "procedures.h"



int txAssocBeacon(struct AssocBeaconMessage *abm, struct TXParams *tp){
    
    // uint8_t phyheader[5];
    // uint8_t message[22];
    // plcf_10_t plcf_10;      //Physical Layer Common Field - 5 bytes  
    // mlcf_a_t mlcf_a;        //MAC Header Type - 1 byte
    // mlcf_b_2t mlcf_b_2;     //MAC Beacon Header - 7 bytes
    // mlcf_c_2t mlcf_c_2;     //MAC MUX Header - Type c - 1 byte
    // mlmf_1_t mlmf_1;        //MAC Beacon Message - 13 bytes


    //Montamos la cabecera física
    abm -> plcf_10.HeaderFormat = 0;
    abm->plcf_10.PacketLengthType = 0;
    abm->plcf_10.PacketLength = 7;
    abm->plcf_10.ShortNetworkID = tp->SnetworkID;
    abm->plcf_10.TransmitterIdentity = (uint16_t)tp->S_SRDID;
    abm->plcf_10.TransmitPower = (uint8_t)0xb;
    abm->plcf_10.Reserved = 0;
    abm->plcf_10.DFMCS = 4;

    //Montamos la cabecera MAC
    abm->mlcf_a.Version = 0;
    abm->mlcf_a.Security = 0;
    abm->mlcf_a.HeaderType = 1;

    //Montamos la cabecera MAC del tipo elegido
    abm->mlcf_b_2.NetworkID = (tp->networkID >> 8) & 0xFFFFFF;
    abm->mlcf_b_2.TransmitterAddress = tp->S_LRDID;

    //Montamos la cabecera MAC MUX correspondiente
    abm->mlcf_c_2.MAC_Ext = 3;
    abm->mlcf_c_2.IE_Type = 0b001000;

    //Montamos el mensaje del beacon
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

    //Obtenemos la cabecera física montada
    get_plcf_1(&abm->plcf_10);
    //Obtenemos la cabecera MAC montada
    get_mlcf_a(&abm->mlcf_a);
    //Obtenemos la cabecera MAC del tipo elegido
    get_mlcf_b_2(&abm->mlcf_b_2);
    //Obtenemos la cabecera MAC MUX correspondiente
    get_mlcf_c_2(&abm->mlcf_c_2);
    //Obtenemos el mensaje del beacon montado
    get_mlmf_2(&abm->mlmf_2);
    //Copiamos cabecera en buffer
    memcpy(abm->phyheader, &abm->plcf_10.plcf, 5);
    //Copiamos en el contenido la cabecera MAC
    memcpy(abm->message, &abm->mlcf_a.mlcf_a, 1);
    //Copiamos en el contenido la cabecera MAC del tipo elegido
    memcpy(abm->message + 1, &abm->mlcf_b_2.mlcf_b_2, 7);
    //Copiamos en el contenido la cabecera MAC MUX correspondiente
    memcpy(abm->message + 8, &abm->mlcf_c_2.mlcf_c_2, 1);
    //Copiamos en el contenido del mensaje beacon
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

  //Montamos la cabecera física
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

  //Montamos la cabecera MAC
  aRm->mlcf_a.Version = 0;
  aRm->mlcf_a.Security = 0;
  aRm->mlcf_a.HeaderType = 2;

  //Montamos la cabecera MAC del tipo elegido
  aRm->mlcf_b_3.Reserved = 0;
  aRm->mlcf_b_3.Reset = 1;
  aRm->mlcf_b_3.MACSequence = 1;
  aRm->mlcf_b_3.SequenceNumber = 0;
  aRm->mlcf_b_3.ReceiverAddress = tp->D_LRDID;
  aRm->mlcf_b_3.TransmitterAddress = tp->S_LRDID;

  // printk("In sendAssocReq Receiver LRDID: %x\n", aRm->mlcf_b_3.ReceiverAddress);
  // printk("In sendAssocReq Transmitter LRDID: %x\n", aRm->mlcf_b_3.TransmitterAddress);

  //Montamos la cabecera MAC MUX correspondiente
  aRm->mlcf_c_2.MAC_Ext = 3;
  aRm->mlcf_c_2.IE_Type = 0b001010;

  //Montamos el mensaje de asociación
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


  //Obtenemos la cabecera física montada
  get_plcf_2(&aRm->plcf_20);
  //Obtenemos la cabecera MAC montada
  get_mlcf_a(&aRm->mlcf_a);
  //Obtenemos la cabecera MAC del tipo elegido
  get_mlcf_b_3(&aRm->mlcf_b_3);
  //Obtenemos la cabecera MAC MUX correspondiente
  get_mlcf_c_2(&aRm->mlcf_c_2);
  //Obtenemos el mensaje del beacon montado
  get_mlmf_4(&aRm->mlmf_4);
  //Copiamos cabecera en buffer
  memcpy(aRm->phyheader, &aRm->plcf_20.plcf, 10);
  //Copiamos en el contenido la cabecera MAC
  memcpy(aRm->message, &aRm->mlcf_a.mlcf_a, 1);
  //Copiamos en el contenido la cabecera MAC del tipo elegido
  memcpy(aRm->message + 1, &aRm->mlcf_b_3.mlcf_b_3, 10);
  //Copiamos en el contenido la cabecera MAC MUX correspondiente
  memcpy(aRm->message + 11, &aRm->mlcf_c_2.mlcf_c_2, 1);
  //Copiamos en el contenido del mensaje beacon
  memcpy(aRm->message + 12, &aRm->mlmf_4.mlmf_4, 14);

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

  

  //Montamos la cabecera física
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

  //Montamos la cabecera MAC
  arm->mlcf_a.Version = 0;
  arm->mlcf_a.Security = 0;
  arm->mlcf_a.HeaderType = 2;

  //Montamos la cabecera MAC del tipo elegido
  arm->mlcf_b_3.Reserved = 0;
  arm->mlcf_b_3.Reset = 0;
  arm->mlcf_b_3.MACSequence = 0;
  arm->mlcf_b_3.SequenceNumber = 0;
  arm->mlcf_b_3.ReceiverAddress = tp->D_LRDID;
  arm->mlcf_b_3.TransmitterAddress = tp->S_LRDID;

  // printk("In sendAssocResp Receiver LRDID: %x\n", arm->mlcf_b_3.ReceiverAddress);
  // printk("In sendAssocResp Transmitter LRDID: %x\n", arm->mlcf_b_3.TransmitterAddress);

  //Montamos la cabecera MAC MUX correspondiente
  arm->mlcf_c_2.MAC_Ext = 0;
  arm->mlcf_c_2.IE_Type = 0b001011;

  //Montamos el mensaje de respuesta a la asociación
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

  // Montamos otra cabecera MAC MUX para coordinar el canal con Scheduled Access
  arm->mlcf_c_2_2.MAC_Ext = 0;
  arm->mlcf_c_2_2.IE_Type = 0b010010;

  // Montamos la IE de asignación de recursos
  arm->mlie_3.Allocation_Type = 0b10;           // Uplink only, configured responding 
  arm->mlie_3.Add = 0;                          // New configuration
  arm->mlie_3.ID = 1;                           // ID present
  arm->mlie_3.SFN_Value = tp->SFN;              // SFN value


  //Obtenemos la cabecera física montada
  get_plcf_2(&arm->plcf_20);
  //Obtenemos la cabecera MAC montada
  get_mlcf_a(&arm->mlcf_a);
  //Obtenemos la cabecera MAC del tipo elegido
  get_mlcf_b_3(&arm->mlcf_b_3);
  //Obtenemos la cabecera MAC MUX correspondiente
  get_mlcf_c_2(&arm->mlcf_c_2);
  //Obtenemos el mensaje montado de la respuesta a la asociación
  get_mlmf_5(&arm->mlmf_5);
  //Obtenemos la cabecera MAC MUX para coordinar el canal con Scheduled Access
  get_mlcf_c_2(&arm->mlcf_c_2_2);
  //Obtenemos la IE de asignación de recursos
  get_mlie_3(&arm->mlie_3);
  

  //Copiamos cabecera en buffer
  memcpy(arm->phyheader, &arm->plcf_20.plcf, 10);
  //Copiamos en el contenido la cabecera MAC
  memcpy(arm->message, &arm->mlcf_a.mlcf_a, 1);
  //Copiamos en el contenido la cabecera MAC del tipo elegido
  memcpy(arm->message + 1, &arm->mlcf_b_3.mlcf_b_3, 10);
  //Copiamos en el contenido la cabecera MAC MUX correspondiente
  memcpy(arm->message + 11, &arm->mlcf_c_2.mlcf_c_2, 1);
  //Copiamos en el contenido del mensaje beacon
  memcpy(arm->message + 12, &arm->mlmf_5.mlmf_5, 7);
  //Copiamos el contenido de la cabecera MAC MUX para coordinar el canal con Scheduled Access
  memcpy(arm->message + 19, &arm->mlcf_c_2_2.mlcf_c_2, 1);
  //Copiamos el contenido de la IE de asignación de recursos
  memcpy(arm->message + 20, &arm->mlie_3.mlie_3, 16);

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

  //Montamos la cabecera física
    
  dm->plcf_20.HeaderFormat = 0;
  dm->plcf_20.PacketLengthType = 0;
  dm->plcf_20.PacketLength = 7;
  dm->plcf_20.ShortNetworkID = (uint8_t)0x0a;
  dm->plcf_20.TransmitterIdentity = (uint16_t)tp->S_SRDID;
  dm->plcf_20.TransmitPower = (uint8_t)0xb;
  dm->plcf_20.ReceiverIdentity = tp->D_SRDID;
  dm->plcf_20.DFMCS = 4;

  //Montamos la cabecera MAC
  dm->mlcf_a.Version = 0;
  dm->mlcf_a.Security = 0;
  dm->mlcf_a.HeaderType = 0;

  //Montamos la cabecera MAC del tipo elegido
  dm->mlcf_b_1.Reserved = 0;
  dm->mlcf_b_1.Reset = tp->reset;
  dm->mlcf_b_1.SequenceNumber = tp->sequenceNumber;

  //Montamos la cabecera MAC MUX correspondiente
  dm->mlcf_c_4.MAC_Ext = 2;
  dm->mlcf_c_4.IE_Type = 0b000011;
  dm->mlcf_c_4.Length = 50;

  //// Application data already copied in dm->data externally

  //Obtenemos la cabecera física montada
  get_plcf_2(&dm->plcf_20);
  //Obtenemos la cabecera MAC montada
  get_mlcf_a(&dm->mlcf_a);
  //Obtenemos la cabecera MAC del tipo elegido
  get_mlcf_b_1(&dm->mlcf_b_1);
  //Obtenemos la cabecera MAC MUX correspondiente
  get_mlcf_c_4(&dm->mlcf_c_4);
  //Copiamos cabecera en buffer
  memcpy(dm->phyheader, &dm->plcf_20.plcf, 10);
  //Copiamos en el contenido la cabecera MAC
  memcpy(dm->mac_header, &dm->mlcf_a.mlcf_a, 1);
  //Copiamos en el contenido la cabecera MAC del tipo elegido
  memcpy(dm->mac_header + 1, &dm->mlcf_b_1.mlcf_b_1, 2);
  //Copiamos en el contenido la cabecera MAC MUX correspondiente
  memcpy(dm->mac_header + 3, &dm->mlcf_c_4.mlcf_c_4, 4);
  //Copiamos la cabecera MAC en el buffer
  memcpy(dm->payload, &dm->mac_header, 7);
  //Copiamos los datos en el buffer
  memcpy(dm->payload + 7, &dm->data, 700-7);

  return 1;

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

