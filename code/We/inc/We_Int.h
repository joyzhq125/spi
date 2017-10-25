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
 * We_Int.h
 *
 *
 */

#ifndef _we_int_h
#define _we_int_h

#ifndef _we_def_h
#include "We_Def.h"
#endif
#ifndef _we_cfg_h
#include "We_Cfg.h"
#endif


/**********************************************************************
 * Messaging
 **********************************************************************/

#define TPI_SIGNAL_OK                    0
#define TPI_SIGNAL_ERROR_QUEUE_FULL     -1
#define TPI_SIGNAL_ERROR_QUEUE_UNKNOWN  -2

#define TPI_SOCKET_ERROR_BAD_ID                   -1
#define TPI_SOCKET_ERROR_INVALID_PARAM            -2
#define TPI_SOCKET_ERROR_MSG_SIZE                 -3
#define TPI_SOCKET_ERROR_DELAYED                  -4
#define TPI_SOCKET_ERROR_RESOURCE_LIMIT           -5
#define TPI_SOCKET_ERROR_CONNECTION_FAILED        -6
#define TPI_SOCKET_ERROR_HOST_NOT_FOUND           -7
#define TPI_SOCKET_ERROR_CLOSED                   -8

#define NSMS_DATA   1
#define NUDPS       5
#define NTCPS      15

#define WE_IS_VALID_MODULE_ID(ModuleId) ((ModuleId >= 0x00 && ModuleId < WE_NUMBER_OF_MODULES) ? 1 : 0)

void TPIa_SignalRegisterQueue (WE_UINT8 uiModuleId);

void TPIa_SignalDeregisterQueue (WE_UINT8 uiModuleId);

void *TPIa_SignalAllocMemory (WE_UINT16 uiSignalDataLength);

void TPIa_SignalFreeMemory (void* pvSignalData);

int TPIa_SignalSend
(
    WE_UINT8 uiModuleId, 
    void* pvSignalData, 
    WE_UINT16 uiSignalDataLength
);

void *TPIa_SignalRetrieve (WE_UINT8 uiDestModuleId);

int TPIa_SignalQueueLength (WE_UINT8 uiModuleId);


/**********************************************************************
 * Named Pipes
 **********************************************************************/

/* Notification types */
#define TPI_PIPE_EVENT_CLOSED                     1
#define TPI_PIPE_EVENT_READ                       2
#define TPI_PIPE_EVENT_WRITE                      3

#ifdef NEW_PIPE_ERROR_EVENT
#define TPI_PIPE_EVENT_CLOSED_ERROR               4
#endif

/* Error codes */
#define TPI_PIPE_ERROR_BAD_HANDLE                 -1
#define TPI_PIPE_ERROR_INVALID_PARAM              -2
#define TPI_PIPE_ERROR_EXISTS                     -3
#define TPI_PIPE_ERROR_IS_OPEN                    -4
#define TPI_PIPE_ERROR_DELAYED                    -5
#define TPI_PIPE_ERROR_RESOURCE_LIMIT             -6
#define TPI_PIPE_ERROR_CLOSED                     -7
#define TPI_PIPE_ERROR_NOT_FOUND                  -8

/* ERROR information. */
#define ERR_SOCKET_SUCC                     (0)
#define ERR_SOCKET_GENERAL                  (-1)
#define ERR_SOCKET_INVALID_TYPE             (-2)
#define ERR_SOCKET_AGAIN                    (-3)
#define ERR_SOCKET_DELAYED                  (-4)

int
TPIa_PipeCreate (WE_UINT8 uiModuleId, const char* pcName);

int
TPIa_PipeOpen (WE_UINT8 uiModuleId, const char *pcName);

int
TPIa_PipeClose (int iHandle);

#ifdef NEW_PIPE_ERROR_EVENT
int
TPIa_PipeCloseError (int iHandle);
#endif

int
TPIa_PipeDelete (const char *pcName);

long
TPIa_PipeRead (int iHandle, void* pvBuf, long lBufSize);

long
TPIa_PipeWrite (int iHandle, void* pvBuf, long lBufSize);

int
TPIa_PipePoll (int iHandle);

int
TPIa_PipeStatus (int iHandle, int *piIsOpen, long *piAvailable);

void
TPIa_PipeCloseAll (WE_UINT8 uiModuleId);

#ifdef NEW_PIPE_STATUS

/* set status of pipe */
int
TPIa_PipeUserDataStatusSet(int handle,int status);

/* get status of pipe */
int
TPIa_PipeUserDataStatusGet(int handle,int *status);

#endif


/**********************************************************************
 * File
 **********************************************************************/
#define TPI_FILE_SET_RDONLY      1
#define TPI_FILE_SET_WRONLY      2
#define TPI_FILE_SET_RDWR        4
#define TPI_FILE_SET_APPEND      8  
#define TPI_FILE_SET_CREATE      16
#define TPI_FILE_SET_EXCL        32
#define TPI_FILE_SET_BUFFERED    64

#define TPI_FILE_SEEK_SET        1
#define TPI_FILE_SEEK_CUR        2
#define TPI_FILE_SEEK_END        3

#define TPI_FILE_DIRTYPE         1
#define TPI_FILE_FILETYPE        2

#define TPI_FILE_EVENT_READ      1
#define TPI_FILE_EVENT_WRITE     2

#define TPI_FILE_READ_READY      1
#define TPI_FILE_WRITE_READY     2

#define TPI_FILE_OK              0
#define TPI_FILE_ERROR_ACCESS   -1
#define TPI_FILE_ERROR_DELAYED  -2
#define TPI_FILE_ERROR_PATH     -3
#define TPI_FILE_ERROR_INVALID  -4
#define TPI_FILE_ERROR_SIZE     -5
#define TPI_FILE_ERROR_FULL     -6
#define TPI_FILE_ERROR_EOF      -7
#define TPI_FILE_ERROR_EXIST    -8

int
TPIa_fileOpen
(
    WE_UINT8 uiModId,
    const char *pcFileName,
    int iMode,
    long lSize
);

int
TPIa_fileClose (int iFileHandle);

long
TPIa_fileWrite
(
    int iFileHandle,
    void *data,
    long lSize
);

long
TPIa_fileRead 
(
    int iFileHandle,
    void *data,
    long lSize
);

void
TPIa_fileFlush (int iFileHandle);

long
TPIa_fileSeek
(
    int iFileHandle,
    long lOffset,
    int iSeekMode
);

int
TPIa_fileRemove (const char *pcFileName);

int
TPIa_fileRename
(
    const char *pcSrcName,
    const char *pcDstName
);

int
TPIa_fileMkDir (const char *dirName);

int
TPIa_fileRmDir (const char *dirName);

int
TPIa_fileGetSizeDir (const char *dirName);

int
TPIa_fileReadDir
(
    const char *dirName,
    int pos,
    char *nameBuf,
    int nameBufLength,
    int *type,
    long *lSize
);

void 
TPIa_fileSelect
(
    int iFileHandle,
    int iEventType
);

long
TPIa_fileSetSize
(
    int iFileHandle,
    long lSize
);

long
TPIa_fileGetSize (const char *pcFileName);

void
TPIa_fileCloseAll (WE_UINT8 uiModId);

void
TPIa_fileQuotaGet
(
    WE_UINT8 uiModId,
    WE_INT32 iRequestId,
    const char *pcPath,
    const char *pcMimeType
);

void
TPIa_filePathPropertyGet
(
    WE_UINT8 uiModId,
    WE_INT32 iRequestId,
    const char *pcPath
);

/**********************************************************************
 * Time
 **********************************************************************/

#define TPI_TIME_ZONE_UNKNOWN       9999

WE_UINT32
TPIa_timeGetCurrent (void);

WE_INT16
TPIa_timeGetTimeZone (void);

WE_UINT32
TPIa_timeSecureClock (void);

WE_UINT32
TPIa_timeGetTicks (void);


/**********************************************************************
 * Sockets
 **********************************************************************/

/* Socket types */
#define TPI_SOCKET_TYPE_UDP                       1
#define TPI_SOCKET_TYPE_TCP                       2
#define TPI_SOCKET_TYPE_SMS_DATA                  3
#define TPI_SOCKET_TYPE_SMS_RAW                   4

/* Bearers */
#define TPI_SOCKET_BEARER_IP_ANY                  0
#define TPI_SOCKET_BEARER_GSM_SMS                 3
#define TPI_SOCKET_BEARER_GSM_CSD                 10
#define TPI_SOCKET_BEARER_GSM_GPRS                11
#define TPI_SOCKET_BEARER_BT                      150
#define TPI_SOCKET_BEARER_ANY                     255

/* Notification types */
#define TPI_SOCKET_EVENT_ACCEPT                   1
#define TPI_SOCKET_EVENT_CLOSED                   2
#define TPI_SOCKET_EVENT_CONNECTED                3
#define TPI_SOCKET_EVENT_RECV                     4
#define TPI_SOCKET_EVENT_SEND                     5

#define TPI_SOCKET_ERROR_BAD_ID                   -1
#define TPI_SOCKET_ERROR_INVALID_PARAM            -2
#define TPI_SOCKET_ERROR_MSG_SIZE                 -3
#define TPI_SOCKET_ERROR_DELAYED                  -4
#define TPI_SOCKET_ERROR_RESOURCE_LIMIT           -5
#define TPI_SOCKET_ERROR_CONNECTION_FAILED        -6
#define TPI_SOCKET_ERROR_HOST_NOT_FOUND           -7
#define TPI_SOCKET_ERROR_CLOSED                   -8


/* Address Type */
typedef struct {
  WE_INT16     addrLen;
  unsigned char addr[16];   /* Enough for IPv6 */
  WE_UINT16    port;
} we_sockaddr_t;


int
TPIa_SocketCreate (WE_UINT8 modId, int socketType, WE_INT32 networkAccountID);

int
TPIa_SocketClose (int socketId);

int
TPIa_SocketAccept (int socketId, we_sockaddr_t *addr);

int
TPIa_SocketBind (int socketId, we_sockaddr_t *addr);

int
TPIa_SocketConnect (int socketId, we_sockaddr_t *addr);

void
TPIc_SocketConnectResponse (WE_UINT8 modId, int socketId, int result);

int
TPIa_SocketGetName (int socketId, we_sockaddr_t *addr);

int
TPIa_SocketListen (int socketId);

long
TPIa_SocketRecv (int socketId, void *buf, long bufLen);

long
TPIa_SocketRecvFrom (int socketId, void *buf, long bufLen, we_sockaddr_t *
fromAddr);

long
TPIa_SocketSend (int socketId, void *data, long dataLen);

long
TPIa_SocketSendTo (int socketId, void *data, long dataLen, we_sockaddr_t *
toAddr);

int
TPIa_SocketSelect (int socketId, int eventType);

void
TPIa_SocketCloseAll (WE_UINT8 modId);

void
TPIa_SocketGetHostByName (WE_UINT8 modId, int requestId, const char *
domainName, WE_INT32 networkAccountID);

/**********************************************************************
 * Network Account
 **********************************************************************/

#define TPI_NETWORK_ACCOUNT_ERROR_BAD_ID -1
#define TPI_NETWORK_ACCOUNT_ERROR_NO_MORE_ID -2
#define TPI_NETWORK_ACCOUNT_ERROR_BEARER_NOT_FOUND -3

int
TPIa_networkAccountGetBearer(WE_INT32 networkAccountId);

int
TPIa_networkAccountGetName(WE_INT32 networkAccountId, char *buf, int bufLen);

WE_INT32
TPIa_networkAccountGetFirst(void);

WE_INT32
TPIa_networkAccountGetNext(WE_INT32 networkAccountId);

WE_INT32
TPIa_networkAccountGetId(int bearer);


/**********************************************************************
 * Telephone
 **********************************************************************/

/* Result types */
#define TPI_TEL_OK                            0
#define TPI_TEL_ERROR_UNSPECIFIED             -1
#define TPI_TEL_ERROR_CALLED_PARTY_IS_BUSY    -105
#define TPI_TEL_ERROR_NETWORK_NOT_AVAILABLE   -106
#define TPI_TEL_ERROR_CALLED_PARTY_NO_ANSWER  -107
#define TPI_TEL_ERROR_NO_ACTIVE_CONNECTION    -108
#define TPI_TEL_ERROR_INVALID                 -200

void
TPIa_telMakeCall (WE_UINT8 modId, WE_UINT16 telId, const char* number);

void
TPIa_telSendDtmf (WE_UINT8 modId, WE_UINT16 telId, const char* dtmf);


/**********************************************************************
 * Phonebook
 **********************************************************************/

/* Result types */
#define TPI_PB_OK                             0
#define TPI_PB_ERROR_UNSPECIFIED              -1
#define TPI_PB_ERROR_IN_NAME                  -100
#define TPI_PB_ERROR_NUMBER_TO_LONG           -102
#define TPI_PB_ERROR_PHONE_BOOK_ENTRY         -103
#define TPI_PB_ERROR_PHONE_BOOK_IS_FULL       -104
#define TPI_PB_ERROR_INVALID                  -200

void
TPIa_pbAddEntry (WE_UINT8 modId, WE_UINT16 pbId, const char* name, const char
* number);

 
 /**********************************************************************
 * Logging
 **********************************************************************/

#define TPI_LOG_TYPE_DETAILED_LOW       0
#define TPI_LOG_TYPE_DETAILED_MEDIUM    1
#define TPI_LOG_TYPE_DETAILED_HIGH      2
#define TPI_LOG_TYPE_MEMORY             3

void
TPIa_logMsg (int type, WE_UINT8 modId, const char *format, ...);

void
TPIa_logData (int type, WE_UINT8 modId, const unsigned char *data, int dataLen
);

void
TPIa_logSignal (WE_UINT8 srcModId, WE_UINT8 dstModId, const char *data);

/**********************************************************************
 * Cryptolib
 **********************************************************************/

/* Bulk encryption algorithms. */
#define TPI_CRPT_CIPHER_NULL             0
#define TPI_CRPT_CIPHER_RC5_CBC_40       1
#define TPI_CRPT_CIPHER_RC5_CBC_56       2
#define TPI_CRPT_CIPHER_RC5_CBC          3
#define TPI_CRPT_CIPHER_DES_CBC_40       4
#define TPI_CRPT_CIPHER_DES_CBC          5
#define TPI_CRPT_CIPHER_3DES_CBC_EDE     6
#define TPI_CRPT_CIPHER_IDEA_CBC_40      7
#define TPI_CRPT_CIPHER_IDEA_CBC_56      8
#define TPI_CRPT_CIPHER_IDEA_CBC         9
#define TPI_CRPT_CIPHER_RC5_CBC_64       10
#define TPI_CRPT_CIPHER_IDEA_CBC_64      11
#define TPI_CRPT_CIPHER_RC2_CBC_40       20
#define TPI_CRPT_CIPHER_RC4_40           21
#define TPI_CRPT_CIPHER_RC4_56           22
#define TPI_CRPT_CIPHER_RC4_128          23
#define TPI_CRPT_CIPHER_AES_CBC_128      30
#define TPI_CRPT_CIPHER_VIV_U            40
#define TPI_CRPT_CIPHER_VIV_P            41
#define TPI_CRPT_CIPHER_VIV_C            42
#define TPI_CRPT_CIPHER_HUF              43
#define TPI_CRPT_CIPHER_HUF_LIGHT        44
/* Secure hash algorithms. */
#define TPI_CRPT_HASH_NULL               0
#define TPI_CRPT_HASH_MD5                1
#define TPI_CRPT_HASH_SHA                2
#define TPI_CRPT_HASH_MD2                3

/* Public Key Algorithms */
#define TPI_CRPT_PKC_RSA                 1
#define TPI_CRPT_PKC_DSA                 2

/* Function return values */
#define TPI_CRPT_OK                          0
#define TPI_CRPT_GENERAL_ERROR              -1
#define TPI_CRPT_BUFFER_TOO_SIALL           -2
#define TPI_CRPT_UNSUPPORTED_METHOD         -3
#define TPI_CRPT_INSUFFICIENT_MEMORY        -5
#define TPI_CRPT_CRYPTOLIB_NOT_INITIALISED  -6
#define TPI_CRPT_KEY_TOO_LONG               -7
#define TPI_CRPT_NOT_IMPLEMENTED            -8
#define TPI_CRPT_INVALID_PARAMETER          -9
#define TPI_CRPT_DATA_LENGTH               -10
#define TPI_CRPT_INVALID_KEY               -11
#define TPI_CRPT_INVALID_HANDLE            -12
#define TPI_CRPT_KEY_LENGTH                -13
#define TPI_CRPT_MISSING_KEY               -14

typedef struct we_crpt_key_object_st {
  unsigned char *key;
  int            keyLen;
  unsigned char *iv;
  int            ivLen;
} we_crpt_key_object_t;

typedef struct we_crpt_pub_key_dsa_st {  
  unsigned char *key;
  WE_UINT16     keyLen;
  unsigned char *dsaP;
  WE_UINT16     dsaPLen;
  unsigned char *dsaQ;
  WE_UINT16     dsaQLen;
  unsigned char *dsaG;
  WE_UINT16     dsaGLen;
} we_crpt_pub_key_dsa_t;

typedef struct we_crpt_pub_key_rsa_st {
  unsigned char *exponent;
  WE_UINT16     expLen;
  unsigned char *modulus;  
  WE_UINT16     modLen;
} we_crpt_pub_key_rsa_t;

typedef struct we_crpt_pub_key_st {
  union {
    we_crpt_pub_key_dsa_t dsa;
    we_crpt_pub_key_rsa_t rsa;
  } _u;
} we_crpt_pub_key_t;

typedef struct we_crpt_priv_key_st {
  WE_UINT16     bufLen;
  unsigned char *buf;
} we_crpt_priv_key_t;

typedef struct we_crpt_dh_params_st {
  unsigned char *dhP;
  int            dhPLen;
  unsigned char *dhG;
  int            dhGLen;
  unsigned char *dhYs;
  int            dhYsLen;
} we_crpt_dh_params_t;

/************************************************************
 * Start/Stop
 ************************************************************/

int 
TPIa_crptInitialise (WE_UINT8 modId);

int 
TPIa_crptTerminate (WE_UINT8 modId);

/************************************************************
 * Hash 
 ************************************************************/

int
TPIa_crptHash (WE_UINT8 modId, int alg, const unsigned char *data, 
               int dataLen, unsigned char *digest);

int
TPIa_crptHashInit (WE_UINT8 modId, int alg, WE_UINT32* handleptr);

int
TPIa_crptHashUpdate (WE_UINT32 handle, const unsigned char *part, 
                     int partLen);

int
TPIa_crptHashFinal (WE_UINT32 handle, unsigned char *digest);

/*************************************************************
 * HMAC
 *************************************************************/

int
TPIa_crptHmacInit (WE_UINT8 modId, int alg, const unsigned char *key, 
                   int keyLen, WE_UINT32* handleptr);

int
TPIa_crptHmacUpdate (WE_UINT32 handle, const unsigned char *part, 
                     int partLen);

int
TPIa_crptHmacFinal (WE_UINT32 handle, unsigned char *digest);

/*************************************************************
 * Bulk encryption 
 *************************************************************/

int
TPIa_crptEncrypt (WE_UINT8 modId, int cipherAlg, we_crpt_key_object_t key,
                  const unsigned char *data, int dataLen,
                  unsigned char *encryptedData);

int
TPIa_crptDecrypt (WE_UINT8 modId, int cipherAlg, we_crpt_key_object_t key,
                  const unsigned char *data, int dataLen,
                  unsigned char *decryptedData);

int
TPIa_crptEncryptInit (WE_UINT8 modId, int cipherAlg, 
                      const unsigned char *key, int keyLen, 
                      WE_UINT32* handleptr);

int
TPIa_crptEncryptUpdate (WE_UINT32 handle, const unsigned char *data, 
                        int dataLen, unsigned char *encryptedData);

int
TPIa_crptEncryptFinal (WE_UINT32 handle);

int
TPIa_crptDecryptInit (WE_UINT8 modId, int cipherAlg, 
                      const unsigned char *key, int keyLen,  
                      WE_UINT32* handleptr);

int
TPIa_crptDecryptUpdate (WE_UINT32 handle, const unsigned char *data, 
                        int dataLen, unsigned char *decryptedData);

int
TPIa_crptDecryptFinal (WE_UINT32 handle);

/***********************************************************************
 * Random number generation
 ***********************************************************************/

int
TPIa_crptGenerateRandom (WE_UINT8 modId, unsigned char *randomData, 
                         int randomLen);

/***********************************************************************
 * Public Key Cryptography
 ***********************************************************************/

int
TPIa_crptEncryptPkc (WE_UINT8 modId, int wid,  int pkcAlg, 
                     we_crpt_pub_key_t pubKey, const unsigned char *data,
                     int dataLen);

int
TPIa_crptDecryptPkc (WE_UINT8 modId, int wid, int pkcAlg,
                     we_crpt_priv_key_t privKey, const unsigned char *data,
                     int dataLen);

int
TPIa_crptVerifySignature (WE_UINT8 modId, int wid, int pkcAlg,
                          we_crpt_pub_key_t pubKey, 
                          const unsigned char *msg, int msgLen, 
                          const unsigned char *sig, int sigLen);

int
TPIa_crptComputeSignature (WE_UINT8 modId, int wid, int pkcAlg,
                           we_crpt_priv_key_t privKey, 
                           const unsigned char *buf, int bufLen);
int
TPIa_crptGenerateKeyPair (WE_UINT8 modId, int wid,  int pkcAlg, int size);
                            
int
TPIa_crptDhKeyExchange (WE_UINT8 modId, int wid, we_crpt_dh_params_t params);

/**********************************************************************
 * Widget
 **********************************************************************/
#define TPI_WIDGET_ERROR_UNEXPECTED    -1
#define TPI_WIDGET_ERROR_NOT_SUPPORTED -2


#define WeWidgetHandle WE_UINT32
#define WeScreenHandle WE_UINT32
#define WeWindowHandle WE_UINT32
#define WeGadgetHandle WE_UINT32
#define WeActionHandle WE_UINT32
#define WeImageHandle WE_UINT32
#define WeStringHandle WE_UINT32
#define WeSoundHandle WE_UINT32
#define WeBrushHandle WE_UINT32
#define WeStyleHandle WE_UINT32
#define WeAccessKeyHandle WE_UINT32
#define WeFontFamilyNumber WE_INT16


typedef enum {
  WeBack = 0,
  WeCancel = 1,
  WeExit = 2,
  WeHelp = 3,
  WeOk = 4,
  WeMenu = 5,
  WeStop = 6,
  WeSelect = 7
} WeActionType;


typedef enum {
  WeImplicitChoice = 0,
  WeExclusiveChoice = 1,
  WeMultipleChoice = 2,
  WeExclusiveChoiceDropDown = 3
} WeChoiceType;


typedef struct {
  int iconPos;
  int indexPos;
  int string1Pos;
  int string2Pos;
  int image1Pos;
  int image2Pos;
} WeElementPosition;


typedef enum {
  WeAlert = 0,
  WeConfirmation = 1,
  WeWarning = 2,
  WeInfo = 3,
  WeError = 4
} WeDialogType;


typedef enum {
  WeProgressBar = 0,
  WeVerticalScrollBar = 1,
  WeHorizontalScrollBar = 2
} WeBarType;


typedef enum {
  WeText = 0,
  WeName = 1,
  WeIpAddress = 2,
  WeUrl = 3,
  WeEmailAddress = 4,
  WePhoneNumber = 5,
  WeNumeric = 6,
  WeNumber = 7,
  WeInteger = 8,
  WePasswordText = 9,
  WeFile = 10,
  WeFilePath = 11,
  WeDate = 12,
  WeTime = 13,
  WeNoTextType = 14,
  WeTextLastReserved = 127
} WeTextType;


typedef enum {
  WeMarqueeNone = 0,
  WeMarqueeScroll = 1,
  WeMarqueeSlide = 2,
  WeMarqueeAlternate = 3,
  WeMarqueeInsideScroll = 4
} WeMarqueeType;


typedef struct {
  WE_INT16 x;
  WE_INT16 y;
} WePosition;


typedef struct {
  WE_INT16 height;
  WE_INT16 width;
} WeSize;


typedef enum {
  WeHigh = 0,
  WeMiddle = 1,
  WeLow = 2,
  WeVerticalPosDefault = 3
} WeVerticalPos;


typedef enum {
  WeLeft = 0,
  WeCenter = 1,
  WeRight = 2,
  WeHorizontalPosDefault = 3
} WeHorizontalPos;


typedef struct {
  WeVerticalPos verticalPos;
  WeHorizontalPos horisontalPos;
} WeAlignment;


typedef struct {
  WeSize displaySize;
  int isTouchScreen;
  int isColorDisplay;
#ifdef WE_CFG_DISP_NBR_OF_COLORS_UINT32
  WE_UINT32 numberOfColors;
#else  
  int numberOfColors;
#endif
} WeDisplayProperties;


typedef enum {
  WeNotifyLostFocus = 0,
  WeNotifyFocus = 1,
  WeNotifyStateChange = 2,
  WeNotifyMoveResize = 3,
  WeNotifyRedrawNeeded = 4,
  WeNotifyResourceReady = 5,
  WeNotifyResourceFailed = 6,
  WeNotifyTransparent = 7,
  WeNotifyAnimationReady = 8,
  WeNotifySoundPlayed = 9,
  WeNotifyReleased = 10,
  WeNotifyEvtModuleChanged = 11,
  WeNotifyAddedToDisplay = 12,
  WeNotifyRemovedFromDisplay = 13,
  WeNotifyLastReserved = 127
} WeNotificationType;


typedef enum {
  WePress = 0,
  WeRelease = 1,
  WeRepeat = 2
} WeEventClass;


typedef enum {
  WeKey_0 = 0, WeKey_1 = 1, WeKey_2 = 2, WeKey_3 = 3, WeKey_4
  = 4, WeKey_5 = 5, WeKey_6 = 6, WeKey_7 = 7, WeKey_8 = 8,
  WeKey_9 = 9, WeKey_Star = 10, WeKey_Pound = 11, WeKey_Up =
  12, WeKey_Down = 13, WeKey_Left = 14, WeKey_Right = 15,
  WeKey_Select = 16, WeKey_Clear = 17, WeKey_Yes = 18,
  WeKey_No = 19, WeKey_Menu = 20,  WeKey_PageUp = 21,
  WeKey_PageDown = 22, WePointerer = 23,
  WeKey_LastReserved = 127
} WeEventType;


typedef struct {
  WE_UINT8 eventMode;
  WeEventClass eventClass;
  int eventType;
  WePosition position;
} WeEvent;


#define WE_EVENT_MODE_MULTITAP            0x01


typedef struct {
  int r;
  int g;
  int b;
} WeColor;


typedef enum {
  WeFontNormal = 0,
  WeFontOblique = 1,
  WeFontItalic = 2
} WeFontStyle;


typedef enum {
  WeGenericFontSerif = 0,
  WeGenericFontSansSerif = 1,
  WeGenericFontCursive = 2,
  WeGenericFontFantasy = 3,
  WeGenericFontMonospace = 4,
  WeGenericFontLastReserved = 15
} WeGenericFont;


typedef struct {
  WeFontStyle fontStyle;
  int size;
  int weight;
  int variant;
  WeFontFamilyNumber fontFamily;
} WeFont;


typedef enum {
  WeNone = 0,
  WeDotted = 1,
  WeDashed = 2,
  WeSemiDotted = 3,
  WeSolid = 4
} WeLineStyle;


typedef struct {
  int thickness;
  WeLineStyle style;
} WeLineProperty;


typedef struct {
  int decoration;
} WeTextProperty;
#define WE_TEXT_DECORATION_UNDERLINE      0x01
#define WE_TEXT_DECORATION_OVERLINE       0x02
#define WE_TEXT_DECORATION_STRIKETHROUGH  0x04
#define WE_TEXT_DECORATION_BLINK          0x08


typedef enum {
  WeNoPadding = 0,
  WeHorisontal = 1,
  WeVertical = 2,
  WeTile = 3,
  WeFullPadding = 4
} WePadding;


typedef struct {
  WeImageHandle image;
  WeAlignment alignment;
  WePadding padding;
} WePattern;


typedef enum {
  WeImageZoom100 = 0,
  WeImageZoomAutofit = 1
} WeImageZoom;


typedef struct {
  const char* data;
  int dataSize;
} WeDirectData;


typedef union {
  const char* resource;
  WeDirectData* directData;
} WeCreateData;


typedef enum {
  WeResourceFile = 0,
  WeResourcePipe = 1,
  WeResourceBuffer = 2
} WeResourceType;


typedef enum {
  WeUtf8 = 106,
} WeStringFormat;

typedef enum {
  WeStringStyle = 0
} WeStyleCategory;


void TPIa_setCurNetID(signed short id);

signed short TPIa_getCurNetID(void);


int TPIa_widgetDisplayGetProperties(WeDisplayProperties* displayProperties);


int TPIa_widgetSetInFocus(WeWidgetHandle handle, int focus);


int TPIa_widgetHasFocus(WeWidgetHandle handle);


int TPIa_widgetRelease(WeWidgetHandle handle);


int TPIa_widgetReleaseAll(WE_UINT8 modId);


int TPIa_widgetRemove(WeWidgetHandle handle, WeWidgetHandle handleToBeRemoved);


int TPIa_widgetAddAction(WeWidgetHandle handle, WeActionHandle action);


int TPIa_widgetHandleWeEvt(WeWidgetHandle handle, int eventType, int override,
                            int subscribe);


int TPIa_widgetChangeWeEvtModule(WeWidgetHandle handle, WE_UINT8 modId);


int TPIa_widgetHandleWeNotification(WeWidgetHandle handle, WE_UINT8 modId,
                                     int notificationType, int subscribe);


int TPIa_widgetSetTitle(WeWidgetHandle handle, WeStringHandle title,
                        WeImageHandle image);


int TPIa_widgetSetPosition(WeWidgetHandle handle, const WePosition* position);


int TPIa_widgetSetSize(WeWidgetHandle handle, const WeSize* size);


int TPIa_widgetSetMarquee(WeWidgetHandle handle, WeMarqueeType marqueeType,
                          int direction, int repeat, int scrollAmount, int 
scrollDelay);


int TPIa_widgetGetSize(WeWidgetHandle handle, WeSize* size);


int TPIa_widgetWeEvent2Utf8(WeEvent *event, int multitap, char *buffer);


int TPIa_widgetWindowAddGadget(WeWindowHandle window, WeGadgetHandle gadget,
                               const WePosition* position);


int TPIa_widgetWindowAddWindow(WeWindowHandle window, WeWindowHandle 
addedWindow,
                               const WePosition* position );


WeScreenHandle TPIa_widgetScreenCreate(WE_UINT8 modId, WeStyleHandle 
defaultStyle);


int TPIa_widgetDisplayAddScreen(WeScreenHandle screen);


int TPIa_widgetScreenAddWindow(WeScreenHandle screen, WeWindowHandle window,
                               const WePosition* position);


int TPIa_widgetWindowSetProperties(WeWindowHandle window, int propertyMask,
                                   int setCondition);
#define WE_WINDOW_PROPERTY_SCROLLBARVER   0x0001
#define WE_WINDOW_PROPERTY_SCROLLBARHOR   0x0002
#define WE_WINDOW_PROPERTY_TITLE          0x0004
#define WE_WINDOW_PROPERTY_BORDER         0x0008
#define WE_WINDOW_PROPERTY_SECURE         0x0010
#define WE_WINDOW_PROPERTY_BUSY           0x0020
#define WE_WINDOW_PROPERTY_MOVERESIZE     0x0040 /*To be removed?*/


int TPIa_widgetGetInsideArea(WeWidgetHandle handle, WePosition* position,
                             WeSize* size);


WeWindowHandle TPIa_widgetPaintboxCreate(WE_UINT8 modId, const WeSize* size,
                                          int propertyMask, WeStyleHandle 
defaultStyle);


WeWindowHandle TPIa_widgetFormCreate(WE_UINT8 modId, const WeSize* size,
                                      int propertyMask, WeStyleHandle 
defaultStyle);


int TPIa_widgetTextSetText(WeWidgetHandle text, WeStringHandle initialString, 
int type,
                           int maxSize, WeStringHandle inputString, int 
singleLine);


WeWindowHandle TPIa_widgetEditorCreate(WE_UINT8 modId, WeStringHandle 
initialString,
                                        WeStringHandle inputString, 
WeTextType type,
                                        const char* formatString, int 
inputRequired,
                                        int maxSize, int singleLine, const 
WeSize* size,
                                        int propertyMask, WeStyleHandle 
defaultStyle);


int TPIa_widgetChoiceSetElement(WeWidgetHandle choice, int index, int header,
                                WeStringHandle string1, WeStringHandle string2,
                                WeImageHandle image1, WeImageHandle image2,
                                WeStringHandle toolTip, WE_UINT32 tag, int 
hasSubMenu);


int TPIa_widgetChoiceRemoveElement(WeWidgetHandle choice, int index);


int TPIa_widgetChoiceGetElemState(WeWidgetHandle choice, int index);
#define WE_CHOICE_ELEMENT_SELECTED        0x01
#define WE_CHOICE_ELEMENT_DISABLED        0x02
#define WE_CHOICE_ELEMENT_FOCUSED         0x04


int TPIa_widgetChoiceSetElemState(WeWidgetHandle choice, int index, int state);


int TPIa_widgetChoiceGetElemTag(WeWidgetHandle choice, int index, WE_UINT32* 
tag);


int TPIa_widgetChoiceSize(WeWidgetHandle choice);


int TPIa_widgetChoiceGetSelected(WeWidgetHandle choice, int* selected, 
WE_UINT32* tag);


WeWindowHandle TPIa_widgetMenuCreate(WE_UINT8 modId, WeChoiceType type, const 
WeSize* size,
                                      const WeElementPosition* elementPos,
                                      int shownAttributes, WeWidgetHandle 
parentHandle,
                                      int parentIndex, int propertyMask,
                                      WeStyleHandle defaultStyle);
#define WE_CHOICE_ELEMENT_ICON            0x01
#define WE_CHOICE_ELEMENT_INDEX           0x02
#define WE_CHOICE_ELEMENT_STRING_1        0x04
#define WE_CHOICE_ELEMENT_STRING_2        0x08
#define WE_CHOICE_ELEMENT_IMAGE_1         0x10
#define WE_CHOICE_ELEMENT_IMAGE_2         0x20


WeWindowHandle TPIa_widgetDialogCreate(WE_UINT8 modId, WeStringHandle 
dialogText,
                                        WeDialogType type, int propertyMask);


WeActionHandle TPIa_widgetActionCreate(WE_UINT8 modId, WeStringHandle label,
                                        WeImageHandle image, int actionType);


int TPIa_widgetActionGetType(WeActionHandle action, int* actionType);


int TPIa_widgetGadgetSetProperties(WeGadgetHandle gadget, int propertyMask,
                                   int setCondition);
#define WE_GADGET_PROPERTY_SCROLLBARVER   0x0001
#define WE_GADGET_PROPERTY_LABEL          0x0002
#define WE_GADGET_PROPERTY_BORDER         0x0004
#define WE_GADGET_PROPERTY_FOCUS          0x0008


WeGadgetHandle TPIa_widgetStringGadgetCreate(WE_UINT8 modId, WeStringHandle 
text,
                                              const WeSize* size, WE_INT16 
fixedWidth,
                                              int singleLine, int propertyMask,
                                              WeStyleHandle defaultStyle);


int TPIa_widgetStringGadgetSet(WeGadgetHandle stringGadget, WeStringHandle 
text,
                               int singleLine);


WeGadgetHandle TPIa_widgetTextInputCreate(WE_UINT8 modId, WeStringHandle 
initialString,
                                           WeStringHandle inputString, 
WeTextType type,
                                           const char* formatString, int 
inputRequired,
                                           int maxSize, int singleLine,
                                           const WeSize* size, int 
propertyMask,
                                           WeStyleHandle defaultStyle);


WeGadgetHandle TPIa_widgetSelectgroupCreate(WE_UINT8 modId, WeChoiceType type,
                                             const WeSize* size,
                                             const WeElementPosition* 
elementPos,
                                             int shownAttributes, int 
propertyMask,
                                             WeStyleHandle defaultStyle);


WeGadgetHandle TPIa_widgetImageGadgetCreate(WE_UINT8 modId, WeImageHandle 
image,
                                             const WeSize* size, WeImageZoom 
imageZoom,
                                             int propertyMask,
                                             WeStyleHandle defaultStyle);


int TPIa_widgetImageGadgetSet(WeGadgetHandle imageGadget, WeImageHandle image,
                              WeImageZoom imageZoom);



WeGadgetHandle TPIa_widgetBarCreate(WE_UINT8 modId, WeBarType barType, int 
maxValue, 
                                     int partSize, int initialValue, const 
WeSize* size, 
                                     int propertyMask, WeStyleHandle 
defaultStyle);

int TPIa_widgetBarSetValues(WeGadgetHandle bar, int value, int maxValue, int 
partSize);

int TPIa_widgetBarGetValues(WeGadgetHandle bar, int* value, int* maxValue, int
* partSize);


WeAccessKeyHandle TPIa_widgetSetAccessKey(WeWidgetHandle handle,
                                           const char* accessKeyDefinition, 
int override,
                                           int index);


int TPIa_widgetRemoveAccessKey(WeWidgetHandle handle, WeAccessKeyHandle 
accessKey);


int TPIa_widgetDrawLine(WeWidgetHandle weHandle, const WePosition* start,
                        const WePosition* end);


int TPIa_widgetDrawRect(WeWidgetHandle weHandle, const WePosition* position,
                        const WeSize* size, int fill);


int TPIa_widgetDrawCircle(WeWidgetHandle weHandle, const WePosition* position,
                          const WeSize* size, int fill);


int TPIa_widgetDrawString(WeWidgetHandle weHandle, WeStringHandle string,
                          const WePosition* position, const WeSize* maxSize, 
int index,
                          int nbrOfChars, int useBrushStyle);


int TPIa_widgetDrawImage(WeWidgetHandle weHandle, WeImageHandle image,
                         const WePosition* position, const WeSize* size,
                         WeImageZoom imageZoom, const WePosition* subPosition,
                         const WeSize* subSize);


int TPIa_widgetDrawPolygon(WeWidgetHandle weHandle, int nrOfCorners,
                           const WE_INT16* corners, int fill);


int TPIa_widgetDrawAccessKey(WeWidgetHandle weHandle, WeAccessKeyHandle 
accessKey,
                             const WePosition* position);


int TPIa_widgetHoldDraw(WeWidgetHandle weHandle);


int TPIa_widgetPerformDraw(WeWidgetHandle weHandle);


WeBrushHandle TPIa_widgetGetBrush(WeWidgetHandle handle);


int TPIa_widgetSetColor(WeWidgetHandle weHandle, const WeColor* color,
                        int background);


int TPIa_widgetSetLineProperty(WeWidgetHandle weHandle,
                               const WeLineProperty* lineProperty);


int TPIa_widgetSetTextProperty(WeWidgetHandle handle,
                               const WeTextProperty* textProperty);


int TPIa_widgetSetPattern(WeWidgetHandle weHandle, const WePattern* pattern, 
                          int background);


int TPIa_widgetSetFont(WeWidgetHandle weHandle, const WeFont* font);


WeFontFamilyNumber TPIa_widgetFontGetFamily( const char* fontName);


WeStringHandle TPIa_widgetStringCreate(WE_UINT8 modId, const char* stringData,
                                        WeStringFormat stringFormat, int 
length,
                                        WeStyleHandle defaultStyle);


WeStringHandle TPIa_widgetStringGetPredefined(WE_UINT32 resId);


WeStringHandle TPIa_widgetStringCreateText(WE_UINT8 modId, WeWidgetHandle 
handle);


WeStringHandle TPIa_widgetStringCreateDateTime(WE_UINT8 modId, WE_UINT32 time
, int dateTime);


int TPIa_widgetStringGetLength(WeStringHandle string, int lengthInBytes,
                               WeStringFormat format);


int TPIa_widgetStringGetData(WeStringHandle string, char* buffer,
                             WeStringFormat format);


int TPIa_widgetStringGetWidth(WeStringHandle string, WeBrushHandle brush, int 
index,
                              int nbrOfChars, WE_INT16* width);


int TPIa_widgetStringGetNbrOfChars(WeStringHandle string, WeBrushHandle brush
, int index,
                                   WE_INT16 width, int* nbrOfChars);


int TPIa_widgetFontGetValues(WeFont* font, int* ascent, int* height, int* 
xHeight);


WeImageHandle TPIa_widgetImageCreate(WE_UINT8 modId, WeCreateData* imageData,
                                      const char* imageFormat,
                                      WeResourceType resourceType,
                                      WeStyleHandle defaultStyle);


WeImageHandle TPIa_widgetImageGetPredefined(WE_UINT32 resId, const char* 
resString);


WeImageHandle TPIa_widgetImageCreateEmpty(WE_UINT8 modId, const WeSize* size,
                                           WeStyleHandle defaultStyle);


WeImageHandle TPIa_widgetImageCreateNative(WE_UINT8 modId, const WeSize* size);


WeStyleHandle TPIa_widgetStyleCreate(WE_UINT8 modId, const WeColor* color,
                                      const WeColor* backgroundColor,
                                      const WePattern* foreground,
                                      const WePattern* background,
                                      const WeLineProperty* lineProperty,
                                      const WeFont* font,
                                      const WeTextProperty* textProperty);


int TPIa_widgetStyleGetDefault(WeStyleCategory styleCategory,
                               WeColor* color, WeColor* backgroundColor,
                               WePattern* foreground, WePattern* background,
                               WeLineProperty* lineProperty, WeFont* font,
                               WeTextProperty* textProperty);


WeSoundHandle TPIa_widgetSoundCreate(WE_UINT8 modId, WeCreateData* soundData,
                                      const char* soundFormat,
                                      WeResourceType resourceType);


int TPIa_widgetSoundPlay(WeSoundHandle sound);


WeSoundHandle TPIa_widgetSoundGetPredefined(WE_UINT32 resId);


int TPIa_widgetSoundStop(WeSoundHandle sound);


/**********************************************************************
 * Object Actions
 **********************************************************************/
void TPIa_objectAction (const char *action_cmd, const char *mime_type, 
                        WeResourceType data_type, const unsigned char *data, 
                        WE_INT32 data_len, const char *src_path, 
                        const char *default_name, const char *content_type);


/**********************************************************************
 * Memory
 **********************************************************************/

#ifdef WE_CONFIG_INTERNAL_MALLOC

void*
TPIa_memInternalInit (WE_UINT8 modId, WE_UINT32* size);

#else

void*
TPIa_memAlloc (WE_UINT8 modId, WE_UINT32 size);

void
TPIa_memFree (WE_UINT8 modId, void* p);

#endif

void*
TPIa_memExternalAlloc (WE_UINT8 modId, WE_UINT32 size);

void
TPIa_memExternalFree (WE_UINT8 modId, void* p);

/**********************************************************************
 * Errors
 **********************************************************************/

/* Main error code categories */
#define TPI_ERR_SYSTEM                              0x100
#define TPI_ERR_MODULE                              0x200

/* System errors */
#define TPI_ERR_SYSTEM_REG_ABORTED                  TPI_ERR_SYSTEM + 1
#define TPI_ERR_SYSTEM_REG_FILE_CORRUPT             TPI_ERR_SYSTEM + 2
#define TPI_ERR_SYSTEM_FATAL                        TPI_ERR_SYSTEM + 3
#define TPI_ERR_SYSTEM_REG_TXT_FILE                 TPI_ERR_SYSTEM + 4
#define TPI_ERR_SYSTEM_REG_TXT_PARSE                TPI_ERR_SYSTEM + 5

/* Module errors */
#define TPI_ERR_MODULE_OUT_OF_MEMORY                TPI_ERR_MODULE + 1


void
TPIa_error (WE_UINT8 modId, int errorNo);


/**********************************************************************
 * Module
 **********************************************************************/

WE_UINT8
TPIa_getModuleID (void);

void
TPIa_killTask (WE_UINT8 modId);

#endif

