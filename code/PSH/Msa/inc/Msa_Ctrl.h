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

/*! \file mtr_ctrl.h
 */

#ifndef _MTRCTRL_H_
#define _MTRCTRL_H_

/******************************************************************************
 * Constants
 *****************************************************************************/
#define MTR_MAX_INSTANCES 5

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void mtrCtrlInit(void);
void mtrCtrlTerminate(void);

#endif /* _MTRCTRL_H_ */
