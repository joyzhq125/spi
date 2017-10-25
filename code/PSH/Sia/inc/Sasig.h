/*
 * Copyright (C) Techfaith, 2002-2005.
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

#ifndef SASIG_H
#define SASIG_H

/*--- Definitions/Declarations ---*/

#define SIA_SIG_PLAY_SMIL_REQ   ((WE_MODID_SIA << 8) + 0x01)

/*--- Types ---*/

/*********************************************
 * Memory Macro
 *********************************************/

/*********************************************
 * Exported function
 *********************************************/

void smaReceiveExtSignal(WE_UINT8 srcModule, WE_UINT16 signal, void* p);

#endif /* SASIG_H */
