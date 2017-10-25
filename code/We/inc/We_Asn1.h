/*
 * Copyright (C) Techfaith 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */

#ifndef _we_asn1_h
#define _we_asn1_h

#ifndef  _we_def_h
#include "We_Def.h"
#endif

/************************************************************
 * Constants
 ************************************************************/

/*
 * ASN.1 Tag Types
*/
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

#define WE_ASN1_CONT0             0
#define WE_ASN1_CONT1             1 
#define WE_ASN1_CONT2             2 
#define WE_ASN1_CONT3             3 
#define WE_ASN1_CONT4             4 
#define WE_ASN1_CONT5             5 
#define WE_ASN1_CONT6             6 
#define WE_ASN1_CONT7             7 
#define WE_ASN1_CONT8             8 
#define WE_ASN1_CONT9             9 

/*
 * ASN.1 Tag Method
*/
#define WE_ASN1_PRIM             0x00
#define WE_ASN1_CONSTRUCTED      0x01
#define WE_ASN1_CONS             0x20

/*
 * ASN.1 Tag Class
 */
#define WE_ASN1_UNIV    0x00
#define WE_ASN1_APPL    0x40
#define WE_ASN1_CONT    0x80
#define WE_ASN1_PRIV    0xC0

/*
 *
 */
#define WE_ASN1_BOOLEAN_SIZE      1
#define WE_ASN1_NULL_SIZE         0


/*
 * ASN.1 Decoding errors
 */
#define WE_ASN1_DER_SUCCESS         0


#define WE_ASN1_INIT_ERROR          -11
#define WE_ASN1_TYPE_ERROR          -12
#define WE_ASN1_TAG_ERROR           -13
#define WE_ASN1_DER_VIOLATE_ERROR   -14
#define WE_ASN1_DER_ERROR           -15


/************************************************************
 * Type definitions
 ************************************************************/

/*
 * The object used for converting between asn1
 * and der data representation.
 */
typedef struct we_asn1_st {
  int         t_class;
  int         t_method;
  int         t_type;
  int         t_len;
  int         l_len;
  int         v_len;
  int         tlv_len;
  int         decoded_len;  /* length of decoded value if exists. */
  const unsigned char  *value;
  WE_UINT8   module;
} we_asn1_t;

/************************************************************
 * Functions
 ************************************************************/

/*
 * Encode ASN.1 T part that preface L part in each block 
 * of message data (TLV).
 * tag_type  - tag type to be encoded
 * tag_method - tag method to be encoded
 * tag_class - tag class to be encoded
 * outbuf - buffer that contains the T part encoding
 * Return: TRUE on success, FALSE on fault
*/
int
we_asn1_c_tag (int tag_type, int tag_method,
                int tag_class, unsigned char *outbuf);


/*
 * Encode ASN.1 Length part (TLV) that preface each block of message data
 * inlen - length value to be encoded
 * outbuf - buffer that contains the Length part encoding
 * Return: TRUE on success, FALSE on fault  
*/
int
we_asn1_c_length (long length, unsigned char *outbuf);


/*
 * Encode ASN.1 string types (Value part): OCTET STRING, NUMERIC STRING, 
 * PRINTABLE STRING, IA5 STRING, VISIBLE STRING. The different 
 * types of string refer to the kind of alphabet used.  
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int
we_asn1_c_char_string (const char *inbuf, unsigned char *outbuf);


/*
 * Encode ASN.1 Bit string type (Value part).
 * The BIT STRING type denotes an arbitrary string 
 * of bits (ones and zeroes).
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_bit_string(char *inbuf, unsigned char *outbuf);


/*
 * Encode ASN.1 Object Identifier type (Value part).
 * The OID is basically a sequence of integers delimited 
 * with dot. For example (1.2.840.113549.1).
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_object_identifier (const char *inbuf, unsigned char *outbuf);


/*
 * Encode ASN.1 INTEGER type (Value part). The INTEGER type denotes 
 * an arbitrary integer. INTEGER values can be positive, negative, or zero.
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_integer (long inbuf, unsigned char *outbuf);


/*
 * Encode ASN.1 BOOLEAN type (Value part - TRUE or FALSE).
 * inbuf - inbuf to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_boolean (int inbuf, unsigned char *outbuf);


/*
 * Encode ASN.1 UTC TIME type (Value part).
 * The UTCTime type denotes a "coordinated universal time".
 * It takes the following form: YYMMDDhhmmssZ.
 * inbuf - inbuf to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_utc_time (const char *inbuf,unsigned char *outbuf);


/*
 * Encode ASN.1 Generalized TIME type (Value part).
 * The UTCTime use a two digit year and GeneralizedTime
 * a four-digit year. It takes the following form: YYYYMMDDhhmmssZ.
 * inbuf - inbuf to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_generalized_time (const char *inbuf, unsigned char *outbuf);


/*
 * Gets the size of a DER-encoded ASN.1 type tag.
 * Returns the number of octets of memory necessary to store 
 * the DER-encoded type tag for the indicated type.
 * inbuf - Tag field inbuf to be checked
*/
int
we_asn1_s_tag (long inbuf );


/*
 * Gets the size of a DER-encoded ASN.1 Length part (Length length).
 * Returns the number of octets of memory necessary to store 
 * the DER-encoded Length value.
 * inbuf - Length value to be checked.
*/
int 
we_asn1_s_length (long inbuf );


/*
 * Check the size of ASN.1 INTEGER type inbuf
 *  inbuf   - integer inbuf to be checked
 * Calling: we_asn1_s_integer(&test);
 * Return: the size of checked inbuf    
*/
int
we_asn1_s_integer (long inbuf);
 

/*
 * Check the size of ASN.1 OBJECT IDENTIFIER type inbuf
 *  inbuf   - object Identifier inbuf to be checked
 * Calling: we_asn1_s_objectIdentifier(test);
 * Return: the size of checked inbuf    
*/
int
we_asn1_s_object_identifier (const char *inbuf);


/*
 * Gets the size of a DER-encoded ASN.1 Bit String type value (V part). 
 * The BIT STRING type denotes an arbitrary string of bits 
 * (ones and zeroes). Returns the number of octets of memory necessary 
 * to store the DER-encoded Bit String type value.
 * inbuf - value to be checked.
*/
int 
we_asn1_s_bit_string (const char *inbuf);


/*
 * Gets the size of a DER-encoded ASN.1 String type value (V part): 
 * NUMERIC STRING, PRINTABLE STRING, IA5 STRING, VISIBLE STRING, 
 * OCTET STRING.
 * Returns the number of octets of memory necessary to store 
 * the DER-encoded String type value.
 * inbuf - Length value to be checked.
*/
int 
we_asn1_s_char_string (const char *inbuf);


/***************************************************************** 
 * DECODING ROUTINS
/*****************************************************************/

/*
 * Decodes ASN.1 Tag octets.
 * der_buf    - pointer to Tag octet(s) of DER stream.
 * tag_class  - number of type of Header/Tag ASN.1 class.
 * tag_method - encoding method ( CONSTRUCTED or PRIMITIVE ).
 * tag_type   - number of type type of ASN.1 object.
 * tag_length - number of octet used for the whole Tag section.
 *              (BER/DER used TLV convention)
 * Return:      WE_ASN1_DER_SUCCESS if success.
 *              Otherwise WE_ASN1_DER_ERROR
 */
int
we_asn1_d_tag (const unsigned char *der_buf, int *tag_class,
                int *tag_method, int *tag_type, int *tag_len);

/*
 * Decodes Length section of TLV.
 * der_buf   - pointer to Length octet of TVL DER ASN.1 object stream.
 * len_len   - number of octet(s) used for the whole Length section.
 * value_len - length of V setction of TLV.
 *
 * Return:     WE_ASN1_DER_SUCCESS. Otherwise returns
 *             WE_ASN1_DER_ERROR.
 */
int
we_asn1_d_len (const unsigned char *der_buf,
                int *value_len, int *len_len);

/*
 * Prepare an object to decoding routins
 * input_der  - buffer with binary data coded in ASN.1
 *              data should present TLV BER/DER encoding standards.
 * obj        - object to initialize
 * der_in_len - length of input_DER buffer
 * modId      - module wid number.
 * Return:    WE_ASN1_DER_SUCCESS if success or 
 *            WE_ASN1_DER_ERROR if input_DER points to NULL
 *            WE_ASN1_INIT_ERROR if binary data are not
 *            compatibile with ASN.1 TLV triple.
 *            
 */
int
we_asn1_init (const unsigned char *input_der, we_asn1_t *obj,
               int *der_in_len, WE_UINT8 modId);



/*
 * Check long of asn1 integer type
 */
int
we_asn1_is_int32(we_asn1_t obj);



/*
 * Decodes ASN.1 OCTET STRING TYPE to string format.
 * obj    - pointer to object which has been already
 *          initialized by we_asn1_d_init function.
 * outbuf - outgoing string buffer which stores
 *          decoded octet string asn1 type.
 * Return:  length of decoded string.
 *          WE_ASN1_TAG_ERROR (negativ value)
 *          or zero length string if non success.
 *
 * NOTE:    it is the caller's responsibility to dellocate
 *          the outbuf buffer
 *          obj needs to be initialize first by calling
 *          we_asn1_init function.
 */
int
we_asn1_d_oct_str (we_asn1_t *obj, char **outbuf);




/*
 * Decodes ASN.1 INTEGER TYPE.
 * If above type is coded with four octect an integer value is counted.
 * Otherwise pointer to hex string is returned to parameter.
 * Function is also used by BOOLEAN, NULL
 * and ENUMERATED ASN.1 types. we_asn1_d_bit_str calls it too.
 * obj     - pointer to object which has been already
 *           initialized by we_asn1_init
 * hex_out - pointer to aloccated string with hexes.
 * Return:   WE_ASN1_DER_SUCCESS if success. Otherwise :
 *           WE_ASN1_TYPE_ERROR
 *           WE_ASN1_DER_ERROR
 *           WE_ASN1_DER_VIOLATE_ERROR
 * NOTE:   it is the caller's responsibility to dellocate
 *         the hex_out buffer !
 *         obj needs to be initialize first by calling
 *         we_asn1_init function.
 */
int
we_asn1_d_integer (we_asn1_t *obj, char **hex_out, int *int_out);

/*
 * Decodes ASN.1 BIT STRING type.
 * obj         - pointer to object (needs to be already
 *               initialized by we_asn1_init !)
 * unused_bits - number of unused bits in ASN.1 BIT STRING type.
 * choice      - zero if bits output, one for hex
 * outbuf      - buffer which stores decoded bits information.
 * Return:   Length of decoded bit string including unused bits.
 *           WE_ASN1_DER_SUCCESS if success. Otherwise :
 *           WE_ASN1_TYPE_ERROR
 *           WE_ASN1_DER_ERROR
 *           WE_ASN1_DER_VIOLATE_ERROR
 *
 * NOTE:    it is the caller's responsibility to dellocate
 *          the outbuf buffer
 *          obj needs to be initialize first by calling
 *          we_asn1_init function.
 */
int
we_asn1_d_bit_str (we_asn1_t *obj, unsigned int *unused_bits,
                    int choice, char **outbuf);


/*
 * Decodes ASN.1 OBJECT IDENTIFIER object and format it to string.
 * obj    - pointer to appropriate we_asn1_t object.
 * outbuf - string buffer stores arcs of object after decoding
 *          delimited by ordinary character.
 * delim  - delimiter for string.
 * Return:  number of characters of string buffer
 * NOTE:    1.it is the caller's responsibility to dellocate
 *            the outbuf buffer
 *          2.obj needs to be initialize first by calling
 *            we_asn1_init function.
 */
int
we_asn1_d_obj_id2str (we_asn1_t *obj, char **outbuf, char delim);


/*
 * Decodes ASN.1 OBJECT IDENTIFIER object.
 * obj    - pointer to appropriate we_asn1_t object.
 * outbuf - buffer stores arcs of object after decoding
 * Return:  number of arcs of object identifier stored
 *          in buffer
 * NOTE:    1.it is the caller's responsibility to dellocate
 *            the outbuf buffer
 *          2.obj needs to be initialize first by calling
 *            we_asn1_init function.
 */
int
we_asn1_d_obj_id (we_asn1_t *obj, unsigned int **outbuf);



/*
 * Decodes ASN.1 UTC and GeneralizedTime TYPE to string format.
 * obj    - pointer to object which has been already
 *          initialized by we_asn1_d_init function.
 * outbuf - outgoing string buffer which stores
 *          decoded ASCII string.
 * Return:  length of decoded string.
 *          WE_ASN1_TAG_ERROR (negativ value)
 *          or zero length string if non success.
 *
 * NOTE:    it is the caller's responsibility to dellocate
 *          the outbuf buffer
 *          obj needs to be initialize first by calling
 *          we_asn1_init function.
 */
int
we_asn1_d_time (we_asn1_t *obj, char **outbuf);


#endif
