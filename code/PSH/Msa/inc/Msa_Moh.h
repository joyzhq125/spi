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

/* \file mamoh.h
 * \brief Media object handling.
 */

#ifndef _MAMOH_H_
#define _MAMOH_H_

/******************************************************************************
 * Prototypes
 *****************************************************************************/

MsaMoDataItem *msaAddMo(MsaMoDataItem **mediaObjects, unsigned char *data, 
    unsigned int dataSize, MmsContentType *mmsType, char *name);
void msaDeleteMo(MsaMoDataItem **mediaObjects, const MsaSmilSlide *sSlide, 
    MsaMoDataItem *mo);
int msaGetMoReferenceCount(const MsaSmilSlide *sSlide, const MsaMoDataItem *mo);
MsaMoDataItem *msaFindMo(MsaMoDataItem *mediaObjects, const char *name);
unsigned int msaGetMoCount(const MsaMoDataItem *mediaObjects);
int msaGetMoIndex(const MsaMoDataItem *mediaObjects, const MsaMoDataItem *mo);
MsaMoDataItem *msaFindObjByType(MsaMoDataItem *mo, MsaMediaGroupType gType);
MsaMoDataItem *msaFindObjectByMmsIndex(MsaMoDataItem *mediaObjects, 
    WE_UINT32 index);

#endif /* _MAMOH_H_ */
