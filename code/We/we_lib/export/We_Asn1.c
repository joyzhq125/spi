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

#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Asn1.h"
 
/************************************************************
 * Constants
 ************************************************************/
#define WE_ASN1_UTCTIME_LENGTH             13  /*   YYMMDD000000Z */
#define WE_ASN1_GENERALIZED_TIME_LENGTH    15  /* YYYYMMDD000000Z */

#define WE_ASN1_28_BIT_INT                 0xFFFFFFF
#define WE_ASN1_24_BIT_INT                 0x0FFFFFF
#define WE_ASN1_23_BIT_INT                 0x07FFFFF
#define WE_ASN1_21_BIT_INT                 0x01FFFFF
#define WE_ASN1_16_BIT_INT                 0x000FFFF
#define WE_ASN1_15_BIT_INT                 0x0007FFF
#define WE_ASN1_14_BIT_INT                 0x0003FFF
#define WE_ASN1_08_BIT_INT                 0x00000FF
#define WE_ASN1_07_BIT_INT                 0x000007F
#define WE_ASN1_06_BIT_INT                 0x000003F
#define WE_ASN1_05_BIT_INT                 0x000001F

#define WE_ASN1_MINUS_23_BIT_INT           -8388608
#define WE_ASN1_MINUS_24_BIT_INT           -16777216
#define WE_ASN1_MINUS_15_BIT_INT           -32768
#define WE_ASN1_MINUS_16_BIT_INT           -65536
#define WE_ASN1_MINUS_07_BIT_INT           -128
#define WE_ASN1_MINUS_08_BIT_INT           -256

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
                int tag_class, unsigned char *outbuf)
{
  int k; 
  int outlen;
  unsigned char tmp[4];
  int type = tag_type;

  if (outbuf == NULL) {
    return FALSE;
  }

  /* Tag numbers less then 31 will produce one T part octet, tag numbers 
    between 31 and 127 will produce two octets, tag numbers between 128
    and 16383 will produce three octets */

  /* Short Tag form - one octet */
  if (tag_type < 0x1F) {
    outbuf[0] = (unsigned char)((tag_method & 0xC0) + (tag_class & 0x20) +
                                (type & 0x1F));
    outlen = 1;
  }

  /* Long Tag form - two and more octets */
  else {
    outbuf[0] = (unsigned char)((tag_method & 0xC0) + (tag_class & 0x20) + 0x1F);
    k=0;

    /* The first octet has all ones in bits 5 to 1 */
    while (type) {
      tmp[k++] = (unsigned char)(type & 0x7F);
      type = type >> 7;
    }

    outlen = k + 1;
    
    /*The most significant bit (the "more" bit) is set to 
      1 in the first following octet */
    while (k--) { 
      outbuf[outlen-1-k] = (unsigned char)(tmp[k] + 0x80);
    }

    outbuf[outlen-1] -= 0x80;  /* and to zero in the last */
  }

 return TRUE;
}


/*
 * Encode ASN.1 Length part (TLV) that preface each block of message data
 * inlen - length value to be encoded
 * outbuf - buffer that contains the Length part encoding
 * Return: TRUE on success, FALSE on fault  
*/
int
we_asn1_c_length (long inlen, unsigned char *outbuf)

{
  int k;
  int outlen;
  long len = inlen;
  unsigned char tmp[4];

  if (outbuf == NULL) {
    return FALSE;
  }

  /* The short form, one L part octet, bit 8 set to zero  
     with the length count in bits 7 to 1  */
  if (len < 128) {
    if (outbuf != NULL) { 
      outbuf[0] = (unsigned char)len;
    }

    outlen = 1; 
  }
 
  /* The long form, two and more octets */
  else {
    k = 0;
    while (len) {
      tmp[k++] = (unsigned char)(len & 0xFF);
      len = len >> 8;
    }
    outlen = k + 1;

    /* First octet encodes in its remaining seven bits a value N
       which is the length of a series of octets that themselves
       encode the length of the V part.The bit 8 of the first length 
       octet is set to 1, this msans long L part form */
    if (outbuf != NULL){
      outbuf[0] = (unsigned char)((k & 0x7F) + 0x80);
      
      /* Next octets encode a length value */
      while (k--) 
          outbuf[outlen-1-k] = tmp[k];  
    }
  }
  return TRUE;
}


/*
 * Encode ASN.1 string types (Value part): NUMERIC STRING, 
 * PRINTABLE STRING, IA5 STRING, VISIBLE STRING, OCTET STRING. 
 * The different types of string refer to the kind of alphabet used.
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int
we_asn1_c_char_string (const char *inbuf, unsigned char *outbuf)
{
  /* Gets the string length */
  int inlen = strlen (inbuf);
  
  if (outbuf == NULL) {
    return FALSE;  
  }
  
  /* Octets that contains encoded string */
  memcpy (outbuf, inbuf, inlen);
 
  return TRUE;
}


/*
 * Encode ASN.1 Object Identifier type (Value part).
 * The OID is basically a sequence of integers delimited 
 * with dot. For example (1.2.840.113549.1).
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_object_identifier (const char *inbuf, unsigned char *outbuf)
{
  char last;  
  int outlen; 
  long val[10];  
  char tmp[20];   
  int i = 0;
  int j = 0;
  int k = 0;

  if (outbuf == NULL) {
    return FALSE;
  }

  /* OID value is saved as string of char delimited with dots, ended with NULL */
  while (inbuf[i] != '\0') {

    if (inbuf[i] == '.') {
      tmp[j] = '\0';
      
      /* convert from char to int */
      val[k] = atol(tmp); 
      j = 0;
      memset(tmp, '\0', 20);
      i++;                         
      k++;                         
    }

    tmp[j] = inbuf[i];               
    i++;
    j++;                       
  }

  val[k] = atol(tmp);            
  k++; 
  outlen = 0;

  /* The first octet has value (40 × integer1 + integer2) */
  outbuf[outlen++] = (unsigned char) (val[0] * 40 + val[1]); 
  j=2;

    /* The following octets, if any, encode integer3, …, integerN. 
    Each value is encoded base 128, most significant digit first, 
    with as few digits as possible, and the most significant bit 
    of each octet except the last in the value's encoding set to "1."*/

  while (j < k) {
    
    /* Integer is greater then 0xFFFFFFF, WE set to "1", next octets below */
    if (val[j] > WE_ASN1_28_BIT_INT) {
      outbuf[outlen++] = (unsigned char)((val[j] >> 28) | 0x80);
    }
    
    /* Integer is greater then 0x1FFFFF, WE set to "1",*/
    if (val[j] > WE_ASN1_21_BIT_INT) {
      outbuf[outlen++] = (unsigned char)((val[j] >> 21) | 0x80);
    }
    
    /* Integer is greater then 0x3FFF, WE set to "1" */
    if (val[j] > WE_ASN1_14_BIT_INT) {
       outbuf[outlen++] = (unsigned char)((val[j] >> 14) | 0x80);
    }

    /* Integer is greater then 0x7F, WE set to "1" */
    if (val[j] > WE_ASN1_07_BIT_INT) {
      outbuf[outlen++] = (unsigned char)((val[j] >> 7) | 0x80);
      last = (unsigned char)(val[j] << 1);
      outbuf[outlen++] = (unsigned char)(~(~(last >> 1) | 0x80));
    }

    /* Integer is less then 0x7F we need one octet, WE set to "0" */
    else {
      outbuf[outlen++] = (unsigned char)(val[j]);
    }

    j++;
  }

  return TRUE;
}


/*
 * Encode ASN.1 Bit string type (Value part).
 * The BIT STRING type denotes an arbitrary string 
 * of bits (ones and zeroes).
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_bit_string (char *inbuf,unsigned char *outbuf)
{       
  int number;
  int input;
  int unusedbit;
  int i;
  int j;
  int left = 7;
  int inlen;
 
  if (outbuf == NULL) {
    return FALSE;
  }

  /* Gets the length of the encoded type value */
  inlen = strlen (inbuf);

  /* the first octet gives the number of bits by which
  the length of the bit string is less than the next multiple 
  of eight (this is called the "number of unused bits").  */
  unusedbit = ((inlen % 8) > 0)? (8 - (inlen % 8)) : 0;
  number = (inlen % 8 == 0) ? (inlen / 8 + 1) : (inlen / 8 + 2);
  memset(outbuf,'\0',number);
  outbuf[0] = (char)unusedbit;
  
  /*The second and following octets give the value of 
  the bit string, converted to an octet string. */
  j = 1;
  for(i=0 ;inbuf[i] != '\0'; i++){
    input = inbuf[i] - '0';
    outbuf[j] |= (char)input << left;
    left --;

    if(left < 0){
      outbuf[j] |= (char)input;
      left = 7;
      j++;
    }
  } 
  return TRUE;
}


/*
 * Encode ASN.1 INTEGER type (Value part). The INTEGER type denotes 
 * an arbitrary integer. INTEGER values can be positive, negative, or zero.
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_integer (long inbuf, unsigned char *outbuf)
{
  char zero = TRUE;
  char ones = TRUE;
  int outlen = 0;

  if (outbuf == NULL) 
    return FALSE;
  
   /* Contents octets give the value of the integer, base 256, in two's 
   complement form, most significant digit first, with the minimum
   number of octets. */

  /* Negative integer */
  if (inbuf < 0){ 
      
    /* Integer is less then -16777216, shift 24 bits and take octet */
    if (inbuf < WE_ASN1_MINUS_24_BIT_INT) {
      outbuf[outlen++] = (unsigned char)(inbuf >> 24);
      ones = FALSE;
    }

    /* Integer is between  -16777215 and -8388608, add one octet with 0x00 */
    if ( (inbuf < WE_ASN1_MINUS_23_BIT_INT) && ones) {
      outbuf[outlen++] = 0xFF;
    }

    /* Integer is between -8388607 and -65536, shift 16 bits and take octet */
    if (inbuf <= WE_ASN1_MINUS_16_BIT_INT) { 
      outbuf[outlen++] = (unsigned char)(inbuf >> 16);
      ones = FALSE;
    }

    /* Integer is between  -65535 and -32768, add one octet with 0x00 */
    if ( (inbuf < WE_ASN1_MINUS_15_BIT_INT) && ones) {
      outbuf[outlen++] = 0xFF;
    }

    /* Integer is between -32767 and -256, shift 8 bits and take octet */
    if (inbuf <= WE_ASN1_MINUS_08_BIT_INT) {
      outbuf[outlen++] = (unsigned char)(inbuf >> 8);
      ones = FALSE;
    }

    /* Integer is between  -255 and -128, add one octet with 0x00 */
    if ( (inbuf < WE_ASN1_MINUS_07_BIT_INT) && ones) {
      outbuf[outlen++] = 0xFF;
    }

    /* Integer is between -127 and 0, take octet */
    outbuf[outlen++] = (unsigned char)inbuf;
  }

  /* Positive integer */
  else { 
      
      /* Integer is greater then 0xFFFFFFF, shift 24 bits and take octet */
      if (inbuf  > WE_ASN1_24_BIT_INT) {
      outbuf[outlen++] = (unsigned char)(inbuf  >> 24);
      zero = FALSE;
    }

     /* Integer is between 0x7FFFFF and 0xFFFFFF, add one octet with 0x00, 
        for example 128 will be encoded with two octets as (00 80), we needs 00 */
    if (inbuf  > WE_ASN1_23_BIT_INT && zero) {
      outbuf[outlen++] = 0;
    }

    /* Integer is between 0xFFFF and 0x7FFFFF, shift 16 bits and take octet */
    if (inbuf  > WE_ASN1_16_BIT_INT) {
      outbuf[outlen++] = (unsigned char)(inbuf  >> 16);
      zero = FALSE;
    }

    /* Integer is between 0x7FFF and 0xFFFF, add one octet with 0x00 */
    if (inbuf  > WE_ASN1_15_BIT_INT && zero) {
      outbuf[outlen++] = 0;
    }

    /* Integer is between 0xFF and 0x7FFF, shift 8 bits and take octet */
    if (inbuf  > WE_ASN1_08_BIT_INT) {
      outbuf[outlen++] = (unsigned char)(inbuf >> 8);
      zero = FALSE;
    }

    /* Integer is between 0x7F and 0xFF, add one octet with 0x00 */
    if ( inbuf  > WE_ASN1_07_BIT_INT && zero) {
      outbuf[outlen++] = 0;
    }
    
    /* Integer is less then 0x7F, take one octet */
    outbuf[outlen++] = (unsigned char)inbuf;
  }

  return TRUE;
}


/*
 * Encode ASN.1 BOOLEAN type (Value part - TRUE or FALSE).
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/
int 
we_asn1_c_boolean (int inbuf, unsigned char *outbuf)
{

  if (outbuf == NULL) { 
    return FALSE;
  }

  /* For the value TRUE, an encoding of hex FF is shown */
  if (inbuf  > 0x00) {
    *outbuf = 0xFF;
  }

  /* For the value FALSE, an encoding of hex 00 is shown */
  else {
    *outbuf = 0x00;
  }

  return TRUE;
}


/*
 * Encode ASN.1 UTC TIME type (Value part).
 * The UTCTime type denotes a "coordinated universal time".
 * It takes the following form: YYMMDDhhmmssZ.
 * inbuf - inbuf to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/ 
int 
we_asn1_c_utc_time (const char *inbuf, unsigned char *outbuf)
{
  /* This type is a string type */
  int inlen = strlen (inbuf);
  
  if (outbuf==NULL) { 
    return FALSE; 
  }
  
  /* check the length of the encoded value */
  if (!(inlen == WE_ASN1_UTCTIME_LENGTH)) { 
    return FALSE;
  }

  memcpy(outbuf, inbuf, inlen);

  return TRUE;
}


/*
 * Encode ASN.1 Generalized TIME type (Value part).
 * The UTCTime use a two digit year and GeneralizedTime
 * a four-digit year. It takes the following form: YYYYMMDDhhmmssZ.
 * inbuf - value to be encoded
 * outbuf - buffer that contains the DER encoding
 * Return: TRUE on success, FALSE on fault  
*/ 
int 
we_asn1_c_generalized_time (const char *inbuf, unsigned char *outbuf)
{
  /* This type is a string type */
  int inlen = strlen (inbuf);
  
  if (outbuf == NULL) { 
    return FALSE;
  }

  /* Check the length of the encoded value */
  if (!(inlen == WE_ASN1_GENERALIZED_TIME_LENGTH)) { 
    return FALSE;
  }

  memcpy(outbuf, inbuf, inlen);
 
  return TRUE;
}


/*
 * Gets the size of a DER-encoded ASN.1 Integer type value 
 * (V part). The INTEGER type denotes an arbitrary integer. INTEGER 
 * values can be positive, negative, or zero.
 * Returns the number of octets of memory necessary 
 * to store the DER-encoded Integer type value.
 * inbuf - value to be checked.
*/  
int
we_asn1_s_integer (long inbuf)
{

  /* Positive integers greater then 8388607  will produce four V part octets */
  /* Negative integers less then -8388608 will produce four V part octets */ 
  if (inbuf > WE_ASN1_23_BIT_INT || inbuf < WE_ASN1_MINUS_23_BIT_INT) {
    return 4;
  }

  /* Positive integers between 65536 and 8388608 will produce three V part octets */
  /* Negative integers between -8388607 and -65536 will produce three V part octets */ 
  else if (inbuf > WE_ASN1_16_BIT_INT || inbuf <= WE_ASN1_MINUS_16_BIT_INT) {
    return 3;
  }

  /* Positive integers between 128 and 65535 will produce three V part octets */
  /* Negative integers between -65535 and -128 will produce three V part octets */ 
  else if (inbuf > WE_ASN1_07_BIT_INT || inbuf <= WE_ASN1_MINUS_07_BIT_INT) {
    return 2;
  }

  /* Positive integers between 0 and 127 will produce one V part octet */
  /* Negative integers between -127 and 0 will produce one V part octet */
  else {
    return 1;
  }

}


/*
 * Gets the size of a DER-encoded ASN.1 Object Identifier type value 
 * (V part). The OID is basically a sequence of integers delimited 
 * with dot. For example (1.2.840.113549.1).
 * Returns the number of octets of memory necessary 
 * to store the DER-encoded Object Identifier type value.
 * inbuf - value to be checked.
*/ 
int
we_asn1_s_object_identifier (const char *inbuf)
{
  int outlen;
  long val[10];  
  char temp[20];    
  int i = 0;
  int j = 0;
  int k = 0;

  /* OID value is saved as string of char delimited with dots, ended with NULL */
  while (inbuf[i] != '\0') {
    if (inbuf[i] == '.') {
      temp[j] = '\0'; 
      
      /* convert from char to int */
      val[k] = atol(temp);        
      memset (temp,'\0',20);
      j = 0;
      i++;                         
      k++;                         
    }

    temp[j] = inbuf[i];               
    i++;
    j++;                       
  }

  /* convert from char to int */
  val[k] = atol(temp);            
  k++;

  /*  The first octet is calculated from the first two integers */
  outlen=1;
  j=2;

  /* The next integers are encoded base 128 */
  while (j < k) {

    /* If integer is greater then 0xFFFFFFF we need five octets */
    if (val[j] > WE_ASN1_28_BIT_INT) {
      outlen++;
    }
    
    /* If integer is greater then 0x1FFFFF we need four octets*/
    if (val[j] > WE_ASN1_21_BIT_INT) {
      outlen++;
    }
    
    /* If integer is greater then 0x3FFF we need three octets */
    if (val[j] > WE_ASN1_14_BIT_INT) {
      outlen++;
    }
    
    /* If integer is greater then 0x7F  we need two octets */
    if (val[j] > WE_ASN1_07_BIT_INT) {
      outlen++;
    }
    
    /* If integer is less then 0x7F  we need only one octet */
    outlen++;
    j++;
  }
  
  return outlen;
}


/*
 * Gets the size of a DER-encoded ASN.1 Bit String type value (V part). 
 * The BIT STRING type denotes an arbitrary string of bits 
 * (ones and zeroes). Returns the number of octets of memory necessary 
 * to store the DER-encoded Bit String type value.
 * inbuf - value to be checked.
*/
int 
we_asn1_s_bit_string (const char *inbuf)
{
  int intlen;
  int outlen;
  
  /* Gets the string size */
  intlen = strlen (inbuf);

  /* The size contains one octet for unused bits and next octets 
  calculated as the length of the bit string divided by eight and
  one octet with padding if we have rest */
  outlen = (intlen % 8 == 0) ? (intlen / 8 + 1) : (intlen / 8 + 2);

  return outlen;
}


/*
 * Gets the size of a DER-encoded ASN.1 String type value (V part): 
 * NUMERIC STRING, PRINTABLE STRING, IA5 STRING, VISIBLE STRING, 
 * OCTET STRING, UTC TIME, Generalized TIME.
 * Returns the number of octets of memory necessary to store 
 * the DER-encoded String type value.
 * inbuf - value to be checked.
*/
int 
we_asn1_s_char_string (const char *inbuf)
{
  /* Gets the string size */
  int outlen = strlen(inbuf);

  return outlen;
}


/*
 * Gets the size of a DER-encoded ASN.1 type tag.
 * Returns the number of octets of memory necessary to store 
 * the DER-encoded type tag for the indicated type.
 * inbuf - Tag field value to be checked
*/
int 
we_asn1_s_tag (long inbuf )
{

  if (inbuf < 0) {
    return -1;
  }

  /* Tag numbers between 128 and 16383 will produce three octets */
  else if (inbuf >  WE_ASN1_07_BIT_INT)
    return 3;

  /* Tag numbers between 31 and 127 will produce two octets */
  else if (inbuf >= WE_ASN1_05_BIT_INT)
    return 2;

  /* Tag numbers less then 31 will produce one T part octet */
  else
    return 1;   
}


/*
 * Gets the size of a DER-encoded ASN.1 Length part (Length length).
 * Returns the number of octets of memory necessary to store 
 * the DER-encoded Length value.
 * inbuf - Length value to be checked.
*/
int 
we_asn1_s_length (long inbuf )
{
  if (inbuf < 0) {
    return -1; 
  }

  /* Length numbers greater than 65535 will produce five L part octets */
  else if (inbuf > WE_ASN1_24_BIT_INT) {
    return 5;
  } 

  /* Length numbers between 256 and 65535 will produce four L part octets */
  else if (inbuf > WE_ASN1_16_BIT_INT) {
    return 4;
  }

  /* Length numbers between 128 and 255 will produce three L part octets */
  else if (inbuf > WE_ASN1_08_BIT_INT) {
    return 3;
  }

  /* Length numbers between 31 and 127 will produce two L part octets */
  else if (inbuf > WE_ASN1_07_BIT_INT) {
    return 2;
  }

  /* Length numbers less then 31 will produce one L part octet */
  else {
    return 1;
  }
}


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
                int *tag_method, int *tag_type, int *tag_len)

{
  unsigned int byte_number;
  unsigned int value;

  if (der_buf == NULL)
    return WE_ASN1_DER_ERROR;

  /* method : primitive (6th bit set to "zero")
   * or constructed (5th bit set to"one") */
  if ( (der_buf[0]& 0x20) == 0 )
    *tag_method = 0;
  else
    *tag_method = 1;
    
    /* class type of Tag (UNIVERSAL or APPLICATION, etc.) */
    *tag_class = (der_buf[0]& 0xC0);

  if ( (der_buf[0]& 0x1F) != 0x1F ) {

    /* low Tag number is presented
     * for Tag type < 30. */
    *tag_len = 1;

    /* value of bits 5-1 gives the Tag ASN.1 type */
    *tag_type = (der_buf[0]&0x1F);

    return WE_ASN1_DER_SUCCESS;
  }
  else {    

    value = 0;
    byte_number = 1;

    /* high Tag number
     * 7-1 bits of each following octets give a number of Tag type.
     * 8 bit is used to recognized last octet (set to zero if last) */
    while(der_buf[byte_number]& 128)
               value = value * 128 + (der_buf[byte_number++]& 0x7F);
    /*  take last octet */
    value = value * 128 + (der_buf[byte_number++]&0x7F);   
    
    /* parameter stores number of octet(s) of the whole Tag section */
    *tag_len = byte_number;
    *tag_type = value;

    return WE_ASN1_DER_SUCCESS;

  } /* else */

}  /* end of we_asn1_d_tag function */




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
                int *value_len, int *len_len)

{
  int value_lentmp;
  int max_oct_num;
  int byte_number;

  if (der_buf == NULL)
  return WE_ASN1_DER_ERROR;

  /* short form of Length.
   * check if first bit of first octet is "0" (zero) value. */
  if ( (der_buf[0]& 0x80) == 0 ) {
    *len_len = 1;

    /* Return value of Length part of DER stream ONLY,
     * according to TLV methodology for BER coding. */
    *value_len = der_buf[0];
    return WE_ASN1_DER_SUCCESS;
  }
  else {  /* when long form of Length */

    /* number of octets which should store a value of lenght is
     * at least 2 octets.
     * base on DER restrictions, the maximum number of octets
     * should be 127 (0x7f)  */

    /* number of octets to store Length value. */
    max_oct_num = der_buf[0]&0x7F;  

    byte_number = 1;    
    if (max_oct_num) {
      value_lentmp = 0;

      /* long form : Second and following octets give
       * the length, base 256, most significant digit first */
      while (byte_number <= max_oct_num )
        value_lentmp = value_lentmp * 256 + der_buf[byte_number++];
    }
    else {
      return WE_ASN1_DER_ERROR;
    }

    /* whole number of octes used for Lenght section (TLV) */
    *len_len = byte_number;
  
    *value_len = value_lentmp;

    return WE_ASN1_DER_SUCCESS;
  
  } /* else if long form */

}  /* end of we_asn1_d_len */



/*
 * Prepare an object to decoding routins
 * input_der  - buffer with binary data coded in ASN.1
 *              data should present TLV BER/DER encoding standards.
 * obj        - obj to initialize
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
               int *der_in_len, WE_UINT8 modId)
{

  int value_len;
  int len_len;
  int tag_class;
  int tag_method; 
  int tag_type;
  int tag_len;


  if( input_der == NULL )
    return WE_ASN1_DER_ERROR;

  /* get all information about TLV ASN.1 */
  we_asn1_d_tag (input_der, &tag_class, &tag_method, &tag_type, &tag_len);  
  we_asn1_d_len (input_der + tag_len, &value_len, &len_len);

  /* check if input_DER binary data are TLV */
  if ( (tag_len + len_len + value_len) == *der_in_len ) {
    
    obj->l_len = len_len;
    obj->module = modId;
    obj->t_class = tag_class;
    obj->t_len = tag_len;
    obj->t_method = tag_method;
    obj->t_type = tag_type;
    obj->tlv_len = tag_len + len_len + value_len;
    obj->v_len = value_len;
    obj->decoded_len = 0;

    if ( (obj->t_type == WE_ASN1_NULL) && (value_len != 0) )
      return WE_ASN1_INIT_ERROR;

    if(     (obj->t_type == WE_ASN1_NULL) &&
            (value_len == 0)   )
      obj->value = NULL;
    else
      obj->value = input_der + tag_len + len_len;


    return WE_ASN1_DER_SUCCESS;

  }
  else /* if not TLV */
    return WE_ASN1_INIT_ERROR;

}  /* end of we_asn1_init */



/*
 * Check long of asn1 integer type
 */
int
we_asn1_is_int32 (we_asn1_t obj)

{

  if ( obj.v_len <= sizeof(WE_UINT32) )
    return TRUE;
  else
    return FALSE;

}  /* end of we_asn1_is_int32 */


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
we_asn1_d_oct_str (we_asn1_t *obj, char **outbuf)

{
  unsigned int j;
  char tab[] = "0123456789ABCDEF";
  int k;
  int m;


  if ( (obj->t_type != WE_ASN1_OCTETSTRING) &&
       (obj->t_type != WE_ASN1_BITSTRING) &&
       (obj->t_type != WE_ASN1_INTEGER)  )
    return WE_ASN1_TYPE_ERROR;
  else
    if ( (obj->t_class != WE_ASN1_UNIV) || (obj->t_method != WE_ASN1_PRIM)  ) 
      return WE_ASN1_TAG_ERROR;


  
  j = 0;

  /* allocate memory for outgoing string buffer */
  *outbuf = WE_MEM_ALLOC (obj->module, (obj->v_len * 2) + 1 );

  /* for MASF_ASN1_BITSTRING type leading obj->value[0] octet
   * needs to be ommited */
  if (obj->t_type == WE_ASN1_BITSTRING)
    m = 1;
  else
    m = 0;

  /* convert each octet into hexhex */
  for (k = m; k <= obj->v_len - 1 ; k++)
    j += sprintf(*outbuf + j,"%c%c", tab[(obj->value[k]& 0xf0) >> 4], tab[obj->value[k]& 0x0f] );
  
  /* length of decoded string */
  obj->decoded_len = strlen(*outbuf);

  if (j == strlen(*outbuf))
    return j;
  else
    return 0;

}  /* end of we_asn1_d_oct_str */


/*
 * Decodes ASN.1 INTEGER TYPE.
 * If above type is coded with four octect an integer value is counted.
 * Otherwise pointer to hex string is returned to parameter.
 * Function is also used by BOOLEAN, NULL
 * and ENUMERATED ASN.1 types. we_asn1_d_bit_str calls it too.
 * obj     - pointer to object which has been already
 *           initialized by we_asn1_init
 * hex_out - pointer to aloccated string with hexes.
 * int_out - integer value of decoded asn.1 type is counted if
 *           it was placed on not more like 4 bytes.
 * Return:   WE_ASN1_DER_SUCCESS if success or number equels to length of
 *           hex string. Otherwise :
 *           WE_ASN1_TYPE_ERROR
 *           WE_ASN1_DER_ERROR
 *           WE_ASN1_TAG_ERROR
 *           WE_ASN1_DER_VIOLATE_ERROR
 * NOTE:   it is the caller's responsibility to dellocate
 *         the hex_out buffer !
 *         obj needs to be initialize first by calling
 *         we_asn1_init function.
 */
int
we_asn1_d_integer (we_asn1_t *obj, char **hex_out, int *int_out)

{
  unsigned int bitstr_value;
  int          value;
  int          byte_number;
  int          msb_value;
  char         *hex_retbuf;

  value        = 0;
  byte_number  = 0;
  msb_value    = 0;
  bitstr_value = 0;


  if(    ( (obj->t_type != WE_ASN1_INTEGER) &&
           (obj->t_type != WE_ASN1_BOOLEAN) &&
           (obj->t_type != WE_ASN1_NULL) &&
           (obj->t_type != WE_ASN1_ENUMERATED) )    )
    return WE_ASN1_TYPE_ERROR;
  else {
    if (  (obj->t_method == WE_ASN1_CONSTRUCTED) ||
         (obj->t_class != WE_ASN1_UNIV)  )
      return WE_ASN1_TAG_ERROR; /* finction should work only for
                                  * UNIVERSAL types and PRIMITIVE method */

    if (obj->t_type != WE_ASN1_NULL){
      if ( (obj->v_len == 0) || (obj->value == NULL) )
        return WE_ASN1_DER_ERROR;
    }
    else
      if( (obj->value == NULL) && (obj->v_len == 0) ){
        *int_out = 0;
        return WE_ASN1_DER_SUCCESS;
      }
  }

  /* If the contents octets of an integer value encoding consist
   *  of more than one octet, then the bits of the first octet and
   * bit 8 of the second octet:
   *  a) shall not all be ones; and
   *  b) shall not all be zero.
   * NOTE - These rules ensure that an integer value is always encoded
   * in the siallest possible number of octets.*/
  if (  (obj->v_len > 1) && (obj->t_type == WE_ASN1_INTEGER)  ){

    if(    (  ((obj->value[0]&0xFF) == 0xFF) && ((obj->value[1]&0x80) == 0x80)  ) ||
           (  ((obj->value[0]&0xFF) == 0x00) && ((obj->value[1]&0x80) == 0x00)  )    )

      return WE_ASN1_DER_VIOLATE_ERROR;

  }
  
  /* check length of asn1 integer object*/
  if ( we_asn1_is_int32(*obj) ) {

      value = obj->value[0];
    
    if ( obj->t_type == WE_ASN1_INTEGER) {

      /* check if negativ */
      if ( obj->value[0]& 0x80 ) {   /* if negative */
        msb_value = (int)pow(2, ( (obj->v_len * 8) - 1 ));
        value = (obj->value[0]& 0x07F);
      }
      else /*  if not negative */
        msb_value = 0;

    }


      /* add signed bytes values */
      byte_number = 1;
      while ( byte_number < obj->v_len )
      value = (value * 256) + obj->value[byte_number++];
    
      value = value - msb_value ; /* msb_value is 0 when positive integer */

 

    if( obj->t_type == WE_ASN1_BOOLEAN ) {
      if(value != 0)
        *int_out = TRUE;
      else
        *int_out = FALSE;
    }
    else
      *int_out = value;

  }  
  else {    /* if asn1 integer longer then 32 bits */
    we_asn1_d_oct_str(obj, &hex_retbuf);
    *hex_out = hex_retbuf;
    return strlen(hex_retbuf);
  }


  return WE_ASN1_DER_SUCCESS;

}  /* end of we_asn1_integer */


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
 *
 * NOTE:    it is the caller's responsibility to dellocate
 *          the outbuf buffer
 *          obj needs to be initialize first by calling
 *          we_asn1_init function.
 */
int
we_asn1_d_bit_str (we_asn1_t *obj, unsigned int *unused_bits, int choice, char **outbuf)

{
  unsigned int base;
  unsigned int num;
  char         *p;
  int          bits_count;
  int          nbytes2dec;
  int          j = 0;
  int          k;

  
  if(  (obj->t_type != WE_ASN1_OCTETSTRING) &&
       (obj->t_type != WE_ASN1_INTEGER) &&
       (obj->t_type != WE_ASN1_BITSTRING)  )
      return WE_ASN1_TYPE_ERROR;
    else
      if(    (  (obj->t_method != WE_ASN1_PRIM) ||
                (obj->t_class != WE_ASN1_UNIV)  ) ||
             (  (obj->t_type == WE_ASN1_INTEGER) &&
                (obj->v_len == 0)  )    )
      return WE_ASN1_TAG_ERROR;
  
  if(choice == 0) {
    *unused_bits = obj->value[0];


    base = 2; /* convert to binary system */
    bits_count = 0;

    nbytes2dec = (obj->v_len - 1) * 8;
    *outbuf = WE_MEM_ALLOC (obj->module, nbytes2dec + 1 );

    p = (*outbuf + nbytes2dec);
    *p = '\0';
    for(k = 1; k < obj->v_len; k++) {
  
      num = (unsigned char)obj->value[k];

      do {
        *--p = (char)( ( num % base ) == 0 ? '0' : '1' );
        num /= base;
        bits_count++;
      } while (num!=0);

      while ( bits_count != 8 ) {
        *--p = '0';
        bits_count++;
      } 

      j += sprintf (*outbuf + j,"%s", p);
      p = (p + bits_count);
      bits_count = 0;


    } /* end of for */

  }
  else  {  /* print in hex */

    /* Priint bit string in hex */
    we_asn1_d_oct_str(obj, &(*outbuf));

  }
  

  /* length of decoded string */
  obj->decoded_len = strlen(*outbuf);

  return strlen(*outbuf);

}  /* end of we_asn1_d_bit_str */


/*
 * Decodes ASN.1 OBJECT IDENTIFIER object and format it to string.
 * obj    - pointer to appropriate we_asn1_t object.
 * outbuf - string buffer stores arcs of object after decoding
 *          delimited by ordinary character.
 * delim  - delimiter for string.
 * Return:  number of characters of string buffer (>0) if success. Otherwise:
 *          length of string is zero or
 *          WE_ASN1_TYPE_ERROR
 *          WE_ASN1_TAG_ERROR
 *
 *          
 * NOTE:    1.it is the caller's responsibility to dellocate
 *            the outbuf buffer
 *          2.obj needs to be initialize first by calling
 *            we_asn1_init function.
 */
int
we_asn1_d_obj_id2str (we_asn1_t *obj, char **outbuf, char delim)

{
  
  unsigned int *int_out;
  unsigned int j;
  int          str_size;
  int          arc_num; 
  int          k;
  char         s_tmp[30];  /* 32 bits integer holds maximum
                            * 10 digit numbers */


    if(  (obj->t_type != WE_ASN1_OID) )
      return WE_ASN1_TYPE_ERROR;
    else
      if( (obj->t_method != WE_ASN1_PRIM) ||
          (obj->t_class != WE_ASN1_UNIV) )
        return WE_ASN1_TAG_ERROR;

  j = 0;

  arc_num = we_asn1_d_obj_id (obj, &int_out);

  /* count number of digits for each out[] element plus delimiter chars
   * (one delimiter for each arc except last one) */
  for (k = 0; k < arc_num; k++) {
    j += sprintf(s_tmp,"%d%c", int_out[k], delim); 
  }

  str_size = j - 1;  /* cut this last delimiter */

  *outbuf = WE_MEM_ALLOC (obj->module, str_size);

  j = 0;
  j = sprintf(*outbuf,"%d%c", int_out[0], delim);

  for (k = 1; k <= arc_num; k++)
    if (k == arc_num) {
      sprintf(*outbuf + (j-1),"%c", '\0' );
      j--;
    }
    else
      j += sprintf(*outbuf + j,"%d%c", int_out[k], delim);
    
  if (strlen(*outbuf) == j)
    return j;
  else
    return 0;


}  /* end of we_asn1_d_obj_id2str */



/*
 * Decodes ASN.1 OBJECT IDENTIFIER object.
 * obj    - pointer to appropriate we_asn1_t object.
 * outbuf - buffer stores arcs of object after decoding
 * Return:  number of arcs of object identifier stored
 *          in buffer (>0) if success. Otherwise:
 *          WE_ASN1_TYPE_ERROR
 *          WE_ASN1_TAG_ERROR
 * NOTE:  1.it is the caller's responsibility to dellocate
 *          the outbuf buffer
 *        2.obj needs to be initialize first by calling
 *          we_asn1_init function.
 */
int
we_asn1_d_obj_id (we_asn1_t *obj, unsigned int **outbuf)

{
  unsigned int *s;
  int          value;
  int          len_tmp;
  int          i;
  int          j;
  int          k;

  
  value = 0;
      s = NULL;
      i = 0;
      j = 0;
      k = 0;


  len_tmp = obj->v_len;

  if ( (obj->t_type != WE_ASN1_OID) )
        return WE_ASN1_TYPE_ERROR;
  else
    if ( (obj->t_class != WE_ASN1_UNIV)  ||
         (obj->t_method != WE_ASN1_PRIM) ||
         (obj->value[0]& 0x80)  ) /* 1xxxxxxx octet is reserved for
                                   * asn1 relative object identifier.
                                   */
      return WE_ASN1_TAG_ERROR;


    
  /* count the number of rest arc if any
   * arc1 and arc2 will be decoded from the first octet later */
  for (k = 1; k <= obj->v_len; k++) {
    if (obj->value[k]& 0x80)
      j++;
  }

  k = 0;

  /* allocate memory for first two arc
   * which come from first octet as well as for j rest of arc. */
  s = WE_MEM_ALLOC (obj->module, j + 2 );


  /* which range the value of first arc belongs to ? */

  if (  ( (obj->value[0] >=  0) && (obj->value[0]  <= 39) ) ||
        ( (obj->value[0] >= 40) && (obj->value[0]  <= 79) )  ) {
      s[k++] = obj->value[0]/40;
      s[k++] = obj->value[0]%40;
  }

  if (obj->value[0] >= 80) {
      s[k++] = obj->value[0]/80;
      s[k++] = obj->value[0]%80;
  }


  /* count values for rest of arces
   * start from second octet */
  i = 1; 
  while (len_tmp - 1 > 0) {
    value = 0;
    do {
      value = (value << 7) | (obj->value[i]& 0x7F);
      len_tmp--;
    } while (obj->value[i++]& 0x80);

    s[k++] = value;
  }

  *outbuf = s;

  return k;  /* return number of octets stored
              * in bufout what is equals to number
              * of arc in asn1 object identifier type */

}  /* end of we_asn1_d_obj_id */


/*
 * Decodes ASN.1 UTC and GeneralizedTime TYPE to string format.
 * obj    - pointer to object which has been already
 *          initialized by we_asn1_d_init function.
 * outbuf - outgoing string buffer which stores
 *          decoded ASCII string.
 * Return:  length of decoded string (>0) if success.
 *          WE_ASN1_TYPE_ERROR
 *          WE_ASN1_TAG_ERROR (negativ value)
 *          or zero length string if non success.
 *
 * NOTE:    it is the caller's responsibility to dellocate
 *          the outbuf buffer
 *          obj needs to be initialize first by calling
 *          we_asn1_init function.
 */
int
we_asn1_d_time (we_asn1_t *obj, char **outbuf)

{
  int   k;
  int   j = 0;

  if ( (obj->t_type != WE_ASN1_UTCTIME) &&
       (obj->t_type != WE_ASN1_GENERALIZEDTIME) )
    return WE_ASN1_TYPE_ERROR;
  else
    if ( (obj->t_class != WE_ASN1_UNIV) &&
       (obj->t_method != WE_ASN1_PRIM) )
    return WE_ASN1_TAG_ERROR;


  *outbuf = WE_MEM_ALLOC (obj->module, obj->v_len + 1);
    
  for (k = 0; k < obj->v_len; k++) {
    j += sprintf (*outbuf + j , "%c", obj->value[k]);

  }

  obj->decoded_len = strlen(*outbuf);
  if(obj->decoded_len == j)
    return j;
  else
    return 0;
}  /* end of we_asn1_d_time */



