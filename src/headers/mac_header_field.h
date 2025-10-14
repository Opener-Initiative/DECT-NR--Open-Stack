/*
 * This code is property of Ostfalia University of Applied Sciences, Germany and Universidad Politécnica de Cartagena, Spain. 
 * Relations with QARTIA Smart Technologies, any inquiries please contact author. 
 * Based on ETSI TS 103 636-4 V1.5.1
 * Author: Iván Tomás García <ivan.tomas@edu.upct.es>
 */

#include <stdint.h>

#ifndef MAC_HEADER_FIELD_H
#define MAC_HEADER_FIELD_H


/**
 * @brief MAC Header Type (6.3.2) structure
 *
 * @param Version (2 bits)
 * @param Security (2 bits)
 * @param HeaderType Type of header (4 bits)
 * 
 * @param mlcf_a (1 byte)
 */
typedef struct{
	uint8_t Version		: 2;
	uint8_t Security	: 2;
	uint8_t HeaderType	: 4;

	uint8_t mlcf_a;
} mlcf_a_t;


/**
 * @brief MAC Common header - DATA MAC PDU (6.3.3.1) structure
 *
 * @param Reserved (3 bits)
 * @param Reset (1 bit)
 * @param SequenceNumber (12 bits)
 * 
 * @param mlcf_b_1 (2 Bytes)
 */
typedef struct {
	uint8_t Reserved		: 3;
	uint8_t Reset			: 1;
	uint16_t SequenceNumber	: 12;

	uint8_t mlcf_b_1[2];
} mlcf_b_1t;


/**
 * @brief MAC Common header - MAC Beacon Header (6.3.3.2) structure
 *
 * @param NetworkID (24 bits)
 * @param TransmitterAddress (32 bits)
 * 
 * @param mlcf_b_2 (7 Bytes)
 */
typedef struct {
	uint32_t NetworkID				: 24;
	uint32_t TransmitterAddress		: 32;

	uint8_t mlcf_b_2[7];
} mlcf_b_2t;


/**
 * @brief MAC Common header - MAC Unicast Header (6.3.3.3) structure
 *
 * @param Reserved (3 bits)
 * @param Reset (1 bit)
 * @param MACSequence (4 bits)
 * @param SequenceNumber (8 bits)
 * @param ReceiverAddress (32 bits)
 * @param TransmitterAddress (32 bits)
 * 
 * @param mlcf_b_3 (10 Bytes).
 */
typedef struct {
	uint8_t Reserved				: 3;
	uint8_t Reset					: 1;
	uint8_t MACSequence				: 4;
	uint8_t SequenceNumber			: 8;
	uint32_t ReceiverAddress		: 32;
	uint32_t TransmitterAddress		: 32;

	uint8_t mlcf_b_3[10];
} mlcf_b_3t;


/**
 * @brief MAC Common header - RD Broadcasting Header (6.3.3.4) structure
 *
 * @param Reserved (3 bits)
 * @param Reset (1 bit)
 * @param SequenceNumber (12 bits)
 * @param TransmitterAddress (32 bits)
 *
 * @param mlcf_b_4 (6 Bytes).
 */
typedef struct {
	uint8_t Reserved			: 3;
	uint8_t Reset				: 1;
	uint16_t SequenceNumber		: 12;
	uint32_t TransmitterAddress	: 32;

	uint8_t mlcf_b_4[6];
} mlcf_b_4t;


/**
 * @brief MAC Multiplexing header Type 1 (6.3.4) (Option a & b) (MAC_EXT = 11)
 *
 * @param MAC_Ext Extensión MAC (2 bits)
 * @param Length Longitud (1 bit)
 * @param IE_Type Tipo de IE (5 bits)
 * 
 * @param mlcf_c_1 (1 Byte)
 */
typedef struct {
	uint8_t MAC_Ext		: 2;
	uint8_t Length		: 1;
	uint8_t IE_Type		: 5;

	uint8_t mlcf_c_1;
} mlcf_c_1t;


/**
 * @brief MAC Multiplexing header Type 2 (6.3.4) (Option c) (MAC_EXT = 00)
 *
 * @param MAC_Ext (2 bits)
 * @param IE_Type (6 bits)
 * 
 * @param mlcf_c_2 (1 Byte)
 */
typedef struct {
	uint8_t MAC_Ext		: 2;
	uint8_t IE_Type		: 6;

	uint8_t mlcf_c_2;
} mlcf_c_2t;


/**
 * @brief MAC Multiplexing header Type 3 (6.3.4) (Option d) (MAC_EXT = 01)
 *
 * @param MAC_Ext (2 bits).
 * @param IE_Type (6 bits).
 * @param Length (8 bits).
 * 
 * @param mlcf_c_3 (2 Bytes).
 */
typedef struct {
	uint8_t MAC_Ext		: 2;
	uint8_t IE_Type		: 6;
	uint8_t Length		: 8;

	uint8_t mlcf_c_3[2];
} mlcf_c_3t;


/**
 * @brief MAC Multiplexing header Type 4 (6.3.4) (Option e) (MAC_EXT = 10)
 *
 * @param MAC_Ext (2 bits).
 * @param IE_Type (6 bits).
 * @param Length (16 bits).
 * 
 * @param mlcf_c_4 (3 Bytes).
 */
typedef struct {
	uint8_t MAC_Ext		: 2;
	uint8_t IE_Type		: 6;
	uint16_t Length		: 16;

	uint8_t mlcf_c_4[3];
} mlcf_c_4t;


/**
 * @brief MAC Multiplexing header Type 5 (6.3.4) (Option f) (MAC_EXT = 11)
 *
 * @param MAC_Ext (2 bits).
 * @param IE_Type (6 bits).
 * @param Length (16 bits).
 * @param IE_Type_Extension (8 bits).
 * 
 * @param mlcf_c_5 (4 Bytes).
 */
typedef struct {
	uint8_t MAC_Ext				: 2;
	uint8_t IE_Type				: 6;
	uint16_t Length				: 16;
	uint8_t IE_Type_Extension	: 8;
	
	uint8_t mlcf_c_5[4];
} mlcf_c_5t;


int get_mlcf_a(mlcf_a_t* mlcf_a);
int get_mlcf_b_1(mlcf_b_1t* mlcf_b_1);
int get_mlcf_b_2(mlcf_b_2t* mlcf_b_2);
int get_mlcf_b_3(mlcf_b_3t* mlcf_b_3);
int get_mlcf_b_4(mlcf_b_4t* mlcf_b_4);
int get_mlcf_c_1(mlcf_c_1t* mlcf_c_1);
int get_mlcf_c_2(mlcf_c_2t* mlcf_c_2);
int get_mlcf_c_3(mlcf_c_3t* mlcf_c_3);
int get_mlcf_c_4(mlcf_c_4t* mlcf_c_4);
int get_mlcf_c_5(mlcf_c_5t* mlcf_c_5);

int get_mlcf_a_rev(mlcf_a_t* mlcf_a);
int get_mlcf_b_1_rev(mlcf_b_1t* mlcf_b_1);
int get_mlcf_b_2_rev(mlcf_b_2t* mlcf_b_2);
int get_mlcf_b_3_rev(mlcf_b_3t* mlcf_b_3);
int get_mlcf_b_4_rev(mlcf_b_4t* mlcf_b_4);
int get_mlcf_c_1_rev(mlcf_c_1t* mlcf_c_1);
int get_mlcf_c_2_rev(mlcf_c_2t* mlcf_c_2);
int get_mlcf_c_3_rev(mlcf_c_3t* mlcf_c_3);
int get_mlcf_c_4_rev(mlcf_c_4t* mlcf_c_4);
int get_mlcf_c_5_rev(mlcf_c_5t* mlcf_c_5);

#endif