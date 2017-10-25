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
/*
 * We_Crpt.h
 *
 * Created by Petter Wallin
 *
 * Revision  history:
 *
 */

#ifndef _we_crpt_h
#define _we_crpt_h

#ifndef _we_int_h
#include "We_Int.h"
#endif

/*Functions*/
#define WE_CRPT_INITIALISE                TPIa_crptInitialise
#define WE_CRPT_TERMINATE                 TPIa_crptTerminate

#define WE_CRPT_HASH_INIT                 TPIa_crptHashInit 
#define WE_CRPT_HASH_UPDATE               TPIa_crptHashUpdate 
#define WE_CRPT_HASH_FINAL                TPIa_crptHashFinal 
#define WE_CRPT_HASH                      TPIa_crptHash 

#define WE_CRPT_HMAC_INIT                 TPIa_crptHmacInit 
#define WE_CRPT_HMAC_UPDATE               TPIa_crptHmacUpdate 
#define WE_CRPT_HMAC_FINAL                TPIa_crptHmacFinal 
#define WE_CRPT_HMAC                      TPIa_crptHmac 

#define WE_CRPT_ENCRYPT_INIT              TPIa_crptEncryptInit 
#define WE_CRPT_ENCRYPT_UPDATE            TPIa_crptEncryptUpdate 
#define WE_CRPT_ENCRYPT_FINAL             TPIa_crptEncryptFinal 
#define WE_CRPT_ENCRYPT                   TPIa_crptEncrypt 

#define WE_CRPT_DECRYPT_INIT              TPIa_crptDecryptInit 
#define WE_CRPT_DECRYPT_UPDATE            TPIa_crptDecryptUpdate 
#define WE_CRPT_DECRYPT_FINAL             TPIa_crptDecryptFinal 
#define WE_CRPT_DECRYPT                   TPIa_crptDecrypt 

#define WE_CRPT_GENERATE_RANDOM           TPIa_crptGenerateRandom 

#define WE_CRPT_ENCRYPT_PKC               TPIa_crptEncryptPkc 
#define WE_CRPT_DECRYPT_PKC               TPIa_crptDecryptPkc 
#define WE_CRPT_VERIFY_SIGNATURE          TPIa_crptVerifySignature 
#define WE_CRPT_COMPUTE_SIGNATURE         TPIa_crptComputeSignature
#define WE_CRPT_GENERATE_KEY_PAIR         TPIa_crptGenerateKeyPair 
#define WE_CRPT_DH_KEY_EXCHANGE           TPIa_crptDhKeyExchange
                                           
/* Bulk encryption algorithms. */          
#define WE_CRPT_CIPHER_NULL               TPI_CRPT_CIPHER_NULL        
#define WE_CRPT_CIPHER_RC5_CBC_40         TPI_CRPT_CIPHER_RC5_CBC_40  
#define WE_CRPT_CIPHER_RC5_CBC_56         TPI_CRPT_CIPHER_RC5_CBC_56  
#define WE_CRPT_CIPHER_RC5_CBC            TPI_CRPT_CIPHER_RC5_CBC     
#define WE_CRPT_CIPHER_DES_CBC_40         TPI_CRPT_CIPHER_DES_CBC_40  
#define WE_CRPT_CIPHER_DES_CBC            TPI_CRPT_CIPHER_DES_CBC     
#define WE_CRPT_CIPHER_3DES_CBC_EDE       TPI_CRPT_CIPHER_3DES_CBC_EDE
#define WE_CRPT_CIPHER_IDEA_CBC_40        TPI_CRPT_CIPHER_IDEA_CBC_40 
#define WE_CRPT_CIPHER_IDEA_CBC_56        TPI_CRPT_CIPHER_IDEA_CBC_56 
#define WE_CRPT_CIPHER_IDEA_CBC           TPI_CRPT_CIPHER_IDEA_CBC    
#define WE_CRPT_CIPHER_RC5_CBC_64         TPI_CRPT_CIPHER_RC5_CBC_64  
#define WE_CRPT_CIPHER_IDEA_CBC_64        TPI_CRPT_CIPHER_IDEA_CBC_64 
#define WE_CRPT_CIPHER_RC2_CBC_40         TPI_CRPT_CIPHER_RC2_CBC_40  
#define WE_CRPT_CIPHER_RC4_40             TPI_CRPT_CIPHER_RC4_40      
#define WE_CRPT_CIPHER_RC4_56             TPI_CRPT_CIPHER_RC4_56  
#define WE_CRPT_CIPHER_RC4_128            TPI_CRPT_CIPHER_RC4_128     
#define WE_CRPT_CIPHER_AES_CBC_128        TPI_CRPT_CIPHER_AES_CBC_128 
#define WE_CRPT_CIPHER_VIV_U              TPI_CRPT_CIPHER_VIV_U    
#define WE_CRPT_CIPHER_VIV_P              TPI_CRPT_CIPHER_VIV_P    
#define WE_CRPT_CIPHER_VIV_C              TPI_CRPT_CIPHER_VIV_C    
#define WE_CRPT_CIPHER_HUF                TPI_CRPT_CIPHER_HUF      
#define WE_CRPT_CIPHER_HUF_LIGHT          TPI_CRPT_CIPHER_HUF_LIGHT

                                           
/* Secure hash algorithms. */              
#define WE_CRPT_HASH_NULL                 TPI_CRPT_HASH_NULL          
#define WE_CRPT_HASH_MD5                  TPI_CRPT_HASH_MD5           
#define WE_CRPT_HASH_SHA                  TPI_CRPT_HASH_SHA           
#define WE_CRPT_HASH_MD2                  TPI_CRPT_HASH_MD2  
                                           
/* Public Key Cryptograhy */               
#define WE_CRPT_PKC_RSA                   TPI_CRPT_PKC_RSA 
#define WE_CRPT_PKC_DSA                   TPI_CRPT_PKC_DSA       

/* Return values. */
#define WE_CRPT_OK                        TPI_CRPT_OK                           
#define WE_CRPT_GENERAL_ERROR             TPI_CRPT_GENERAL_ERROR                
#define WE_CRPT_BUFFER_TOO_SIALL          TPI_CRPT_BUFFER_TOO_SIALL             
#define WE_CRPT_UNSUPPORTED_METHOD        TPI_CRPT_UNSUPPORTED_METHOD           
#define WE_CRPT_INSUFFICIENT_MEMORY       TPI_CRPT_INSUFFICIENT_MEMORY          
#define WE_CRPT_CRYPTOLIB_NOT_INITIALISED TPI_CRPT_CRYPTOLIB_NOT_INITIALISED    
#define WE_CRPT_KEY_TOO_LONG              TPI_CRPT_KEY_TOO_LONG                 
#define WE_CRPT_NOT_IMPLEMENTED           TPI_CRPT_NOT_IMPLEMENTED              
#define WE_CRPT_INVALID_PARAMETER         TPI_CRPT_INVALID_PARAMETER            
#define WE_CRPT_DATA_LENGTH               TPI_CRPT_DATA_LENGTH                  
#define WE_CRPT_INVALID_KEY               TPI_CRPT_INVALID_KEY                  
#define WE_CRPT_INVALID_HANDLE            TPI_CRPT_INVALID_HANDLE               
#define WE_CRPT_KEY_LENGTH                TPI_CRPT_KEY_LENGTH                   
#define WE_CRPT_MISSING_KEY               TPI_CRPT_MISSING_KEY              

#endif
