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
 
#ifndef MCREATE_H
#define MCREATE_H















unsigned char *createBodyPartHeader(WE_UINT8 modId, WE_UINT32 *encHeaderLen,
    const MmsBodyParams *bodyParams, WE_UINT32 size, MmsResult *result );

MmsResult writeDataToFile( WE_UINT8 modId, unsigned char **buf, 
    WE_UINT32 *sizeLeft, int fileHandle, WE_INT32 *filePos);
void freeBodyParams( WE_UINT8 modId, MmsBodyParams *bodyParams);


#endif 
