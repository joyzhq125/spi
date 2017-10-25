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
 * ubs_ecfg.h
 *
 * It is allowed for an integrator to make changes in this file.
 *
 * Created by Klas Hermodsson, Thu Nov 20 11:00 2003.
 *
 * Revision  history:
 *
 * 2004-01-16 (KOGR): CR 13932: entry for UBS_MSG_KEY_STORAGE added in
 *                      UBS_USER_TITLE_PROPERTY_ARRAY
 * 2004-02-19 (KOGR): CR 13636: entry for UBS_MSG_KEY_NBR_OF_MESSAGES_WAITING
 *                      added in UBS_USER_TITLE_PROPERTY_ARRAY
 * 2004-02-19 (KOGR): CR 13934/13626: entry for UBS_MSG_KEY_MSG_STATUS
 *                      added in UBS_USER_TITLE_PROPERTY_ARRAY
 * 2005-01-21 (HUZH): TR_17904: entry for UBS_MSG_KEY_CLASS
 *                      added in UBS_USER_TITLE_PROPERTY_ARRAY

 */

#ifndef UBS_ECFG_H
#define UBS_ECFG_H

#ifndef UBS_IF_H
  #include "Ubs_If.h"
#endif


/******************************************************************************************
 *  The number of user message types.
 *
 *  This MUST be set to the number of user message types listed in this file.
 *
 */
#define UBS_NBR_OF_USER_MSG_TYPES           0

/*
 *  The definition of the user message types
 *
 *  The user defined message types MUST have values based on UBS_NBR_OF_SYSTEM_MSG_TYPES!
 *  This ensures that an update of the system message types do not interfere with the 
 *  user message types. All code handling these message types MUST use these 
 *  defines to reference the message type values.
 *  
 *  For each user message type, add a define.
 *
 *  The message types MUST be numbered in sequence with no gaps in between!
 *  That msans if there are a total of 10 user message types, they must be numbered 
 *  from UBS_NBR_OF_SYSTEM_MSG_TYPES + 0 to UBS_NBR_OF_SYSTEM_MSG_TYPES + 9!
 *
 *  Example of two user message type definitions:
 *  #define UBS_MSG_TYPE_USER_1                  UBS_NBR_OF_SYSTEM_MSG_TYPES + 0
 *  #define UBS_MSG_TYPE_USER_2                  UBS_NBR_OF_SYSTEM_MSG_TYPES + 1
 */

/* place user message type definitions here */

/******************************************************************************************
 *  The number of user message property keys.
 *
 *  This MUST be set to the number of user message property keys listed in this file.
 *
 */
#define UBS_NBR_OF_USER_KEYS                1

/*
 *  The definition of the user message property keys
 *
 *  The user defined keys MUST have values based on UBS_NBR_OF_SYSTEM_KEYS!
 *  This ensures that an update of the system keys do not interfere with the user keys.
 *  All code handling these user properties MUST use these defines to reference
 *  the message property key values.
 *  
 *  For each user message property, add a define.
 *
 *  The keys MUST be numbered in sequence with no gaps in between!
 *  That msans if there are a total of 10 user message property keys, 
 *  they must be numbered from UBS_NBR_OF_SYSTEM_KEYS + 0 to UBS_NBR_OF_SYSTEM_KEYS + 9!
 *
 *  Example of two user message property key definitions:
 *  #define UBS_MSG_USER_KEY_1                  UBS_NBR_OF_SYSTEM_KEYS + 0
 *  #define UBS_MSG_USER_KEY_2                  UBS_NBR_OF_SYSTEM_KEYS + 1
 */

/* place user message property key definitions here */
 
/******************************************************************************************
 *  The title property array.
 *
 *  For a description see UBS_TITLE_PROPERTY_ARRAY in ubs_if.h
 *
 *  NOTE that the size of this array MUST be UBS_NBR_OF_KEYS. Which msans that
 *  in addition to the system keys there must also be one entry for each defined 
 *  user message property key.
 */      
#define UBS_USER_TITLE_PROPERTY_ARRAY { \
                        {TRUE, 0}, \
                        {TRUE, 0}, \
                        {TRUE, 0}, \
                        {TRUE, 0}, \
                        {TRUE, 0}, \
                        {TRUE, 0}, \
                        {TRUE, 0}, \
                        {TRUE, 0}, \
                        {TRUE, 0}, \
                       {FALSE, 0}, \
                       {FALSE, 0}, \
                       {FALSE, 0}, \
                       {FALSE, 0}, \
                        {TRUE, 0}, \
                       {FALSE, 0}, \
                       {FALSE, 0}, \
                       {FALSE, 0}, \
                       {TRUE, 0}, \
/*TR 17904, 18100*/	   {TRUE, 0},\
/* add one entry below for each user message property key defined in this file */ \
        }


#endif      /*UBS_ECFG_H*/
