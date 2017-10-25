#ifndef INT_WIDGET_REMOVE_H
#define INT_WIDGET_REMOVE_H
/*==================================================================================================

    HEADER NAME : int_widget_common.h

    GENERAL DESCRIPTION
        General description of the contents of this header file.

    SEF Telecom Confidential Proprietary
    (c) Copyright 2003 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    MM/DD/YYYY   name of author   crxxxxx     Brief description of changes made
    08/05/2003   Steven Lai       Cxxxxxx     Initial file creation. 

    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

====================================================================================================
    INCLUDE FILES
==================================================================================================*/

/*==================================================================================================
    CONSTANTS
==================================================================================================*/

/*==================================================================================================
    MACROS
==================================================================================================*/


/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/

/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* 
 * Define Application's management data structure here as APP_appname_STRUCTURE_NAME_T
 * "APP_DATA_STRUCTURE_NAME_T app" field should be defined first for OPUS framework.
 */


/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/


/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/


extern int screen_release(OP_UINT32 handle);

extern int window_release(OP_UINT32 handle);

extern int gadget_release(OP_UINT32 handle);

extern int image_release(OP_UINT32 handle);

extern int string_release(OP_UINT32 handle);

extern int icon_release(OP_UINT32 handle);

extern int sound_release(OP_UINT32 handle);

extern int style_release(OP_UINT32 handle);

extern int coloranim_release(OP_UINT32 handle);

extern int move_release(OP_UINT32 handle);

extern int rotation_release(OP_UINT32 handle);

extern int action_release(OP_UINT32 handle);

extern int decrease_ref_count(OP_UINT32 handle);

extern int increase_ref_count(OP_UINT32 handle);

extern int window_remove(  void *pParWidget,
                         MsfWidgetType iParWidgetType, 
                         void *pChldWidget,
                         MsfWidgetType iChldWidgetType);

extern int gadget_remove( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType);

extern int action_remove( void *pParWidget,
                            MsfWidgetType iParWidgetType, 
                            void *pChldWidget,
                            MsfWidgetType iChldWidgetType);

extern int coloranim_remove ( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType);
                        
extern int move_remove ( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType);
                        
extern int rotation_remove( void *pParWidget,
                        MsfWidgetType iParWidgetType, 
                        void *pChldWidget,
                        MsfWidgetType iChldWidgetType);

/*================================================================================================*/
#endif  /* INT_WIDGET_REMOVE_H */

