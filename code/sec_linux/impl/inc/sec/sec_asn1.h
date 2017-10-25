/*=====================================================================================
    FILE NAME : sec_asn1.h
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        This file define macro and function about asn1 syntax.
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date            Author          Number    Description of changes
    ---------- --------------   ---------   --------------------------------------
    2006-07-07      Sam              none        Initialization
    
=====================================================================================*/

/*******************************************************************************
*   multi-Include-Prevent Section
*******************************************************************************/
#ifndef _WE_ASN1_H
#define _WE_ASN1_H


/*******************************************************************************
*   macro Define Section
*******************************************************************************/
/* ASN.1 Tag Types */
#define WE_ASN1_BOOLEAN                       1 
#define WE_ASN1_INTEGER                       2
#define WE_ASN1_BITSTRING                     3 
#define WE_ASN1_OCTETSTRING                   4 
#define WE_ASN1_NULL                          5 
#define WE_ASN1_OID                           6 
#define WE_ASN1_OBJECT_DESCRIPTOR             7 
#define WE_ASN1_EXTERNAL_TYPE_AND_INSTANCE_OF 8 
#define WE_ASN1_REAL                          9 
#define WE_ASN1_ENUMERATED                    10 
#define WE_ASN1_EMBEDDED_PDY                  11 
#define WE_ASN1_UTF8STRING                    12 
#define WE_ASN1_SEQUENCE                      16  
#define WE_ASN1_SEQUENCE_OF                   16  
#define WE_ASN1_SET                           17  
#define WE_ASN1_SET_OF                        17  
#define WE_ASN1_NUMERICSTRING                 18 
#define WE_ASN1_PRINTABLESTRING               19 
#define WE_ASN1_T61STRING                     20 
#define WE_ASN1_VIDEOTEXSTRING                21 
#define WE_ASN1_IA5STRING                     22 
#define WE_ASN1_UTCTIME                       23 
#define WE_ASN1_GENERALIZEDTIME               24 
#define WE_ASN1_GRAPHICSTRING                 25 
#define WE_ASN1_VISIBLESTRING                 26 
#define WE_ASN1_GENERALSTRING                 27 
#define WE_ASN1_UNIVERSALSTRING               28 
#define WE_ASN1_BMPSTRING                     30 
#define WE_ASN1_SEQUENCE_TYPE                (0x30)
#define WE_ASN1_SET_TYPE                     (0x31)
#define WE_ASN1_IMPLICIT_TAG                 (0xa3)
#define WE_ASN1_NEW_TAG1                     (0xa0)
#define WE_ASN1_NEW_TAG2                     (0xa1)

#define WE_ASN1_CONT0               0
#define WE_ASN1_CONT1               1 
#define WE_ASN1_CONT2               2 
#define WE_ASN1_CONT3               3 
#define WE_ASN1_CONT4               4 
#define WE_ASN1_CONT5               5 
#define WE_ASN1_CONT6               6 
#define WE_ASN1_CONT7               7 
#define WE_ASN1_CONT8               8 
#define WE_ASN1_CONT9               9 

/* ASN.1 Tag Method */
#define WE_ASN1_PRIM             0x00
#define WE_ASN1_CONSTRUCTED      0x01
#define WE_ASN1_CONS             0x20

/* ASN.1 Tag Class */
#define WE_ASN1_UNIV             0x00
#define WE_ASN1_APPL             0x40
#define WE_ASN1_CONT             0x80
#define WE_ASN1_PRIV             0xC0

#define WE_ASN1_BOOLEAN_SIZE        1
#define WE_ASN1_NULL_SIZE           0

/* ASN.1 Decoding errors */
#define WE_ASN1_DER_SUCCESS         0

#define WE_ASN1_INIT_ERROR          -11
#define WE_ASN1_TYPE_ERROR          -12
#define WE_ASN1_TAG_ERROR           -13
#define WE_ASN1_DER_VIOLATE_ERROR   -14
#define WE_ASN1_DER_ERROR           -15


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
/* The object used for converting between asn1
 * and der data representation. */
typedef struct tagSt_SecAsn1
{
  WE_INT32         iTclass;
  WE_INT32         iTmethod;
  WE_INT32         iTtype;
  WE_INT32         iTlen;
  WE_INT32         iLlen;
  WE_INT32         iVlen;
  WE_INT32         iTLVlen;
  WE_INT32         iDecodedLen;  /* length of decoded value if exists. */
  const WE_UCHAR  * pucValue;
  WE_UINT8          ucModule;
}St_SecAsn1;


#endif
