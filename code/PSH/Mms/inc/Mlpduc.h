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





#ifndef MLPDUC_H
#define MLPDUC_H









unsigned char *createWspSendHeader( WE_UINT8 modId, const MmsSetHeader *header, 
    WE_UINT32 *length, MmsResult *result, MmsVersion version, WE_UINT32 numOfBodyParts);
unsigned char *createWspBodyPartHead( WE_UINT8 modId, 
    const MmsBodyParams *dataValues, WE_UINT32 *length, WE_UINT32 size, 
    MmsResult *result);
unsigned char *createWspForward( WE_UINT8 modId, const MmsForward *header,
    char *contentLocation, WE_UINT32 *length, MmsResult *result, MmsVersion version);


#endif 
