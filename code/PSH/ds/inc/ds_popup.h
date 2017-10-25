#ifndef DS_PUPUP_H
#define DS_PUPUP_H
/*==================================================================================================

    HEADER NAME : ds_popup.h

    GENERAL DESCRIPTION
        This file include functions that handle popup window.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    07/02/2002   Ashley Lee       crxxxxx     Initial Creation
    06/14/2003    linda wang      P000608      Fix some popup and smart list focus number problems. 
    11/14/2003    sunshuwei       p002058      fix overlaping of menu-bar and scroll-bar
    03/12/2004   Penghaibo        P002619     fix popup msg display position
    03/19/2004   chouwangyun   c002692      create new style popup
    08/18/2004   liren            p007822     change the default height of popup window    
    This file include functions that handle popup window.

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#ifdef WIN32
#include    "windows.h"
#include    "portab_new.h"
#else
#include    "portab.h"
#endif

#include    "OPUS_typedef.h"
#include    "OPUS_events.h"  
#include    "ds_def.h"
#include    "ds_int_def.h"
#include    "ds_drawing.h"
#include    "ds_font.h"
#include    "ds_lcd.h"
#include    "ds_util.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
/* Popup attriute */

#define PUA_DEFAULT      0x00
/* not used anymore */
#define PUA_SOFTKEY      0x01        /* User assigned softkeys */
#define PUA_POSITION     0x02        /* User assigned popup position (size also) */
#define PUA_POINT        0x04        /* User assigned starting point */
#define PUA_COMMONDLG    0x08        /* Commom dialog popup */
#define PUA_USERLIST     0x10        /* User assigned list data */
#define PUA_STATIC       0x20        /* Static popup window */

#define PHANDLE_NONE     ( -1 )

#define POPUP_DEFAULT_TOP                42     /* desided by design */
#define POPUP_DEFAULT_LEFT               6     /* desided by design */


#define POPUP_DEFAULT_WIDTH              130  /* determined by design */
#define POPUP_DEFAULT_HEIGHT             68  /* determined by design */

#define POPUP_DEFAULT_TOP_MARGINE       10    /* margine over the first line */
#define POPUP_DEFAULT_BOTTOM             (POPUP_DEFAULT_TOP+POPUP_DEFAULT_HEIGHT-1)    /* desided by design */
#define POPUP_DEFAULT_RIGHT              (POPUP_DEFAULT_LEFT+POPUP_DEFAULT_WIDTH-1)    /* desided by design */
/*==================================================================================================
    MACROS
==================================================================================================*/
#define PHANDLE_ISVALID(h)   ( ((h)>=0 && (h)<POPUP_MAX_WIN) && popWin[(h)].handle != PHANDLE_NONE )

/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/
/* popup event handler retrun statuss */
typedef enum
{
    POPUP_INVALID,        /* the popup was invalid one */
    POPUP_CLOSE,          /* No longer effective popup. Close the popup */
    POPUP_PROC_EVENT,     /* The passed event was processed by the PDEH */
    POPUP_UNPROC_EVENT    /* The passed event was not processed by the PDEH */
#if 0 // Ashley!!!, Think about this. If it will be used, the popup is closed by the app */
    POPUP_STORED_EVENT    /* Notify that the passed event was stored key pressing */
#endif
}  DS_POPUP_PDEH_RET_ENUM_T;

/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
    FUNCTION: init_popup_windows

    DESCRIPTION:
        Initialize popup related data.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
extern void popup_init_popup_windows 
( 
    void 
);


/*==================================================================================================
    FUNCTION: ds_is_popup

    DESCRIPTION:
        Returns whether there is an active popup window or not.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
extern OP_BOOLEAN ds_is_popup 
( 
    void 
);


/*==================================================================================================
    FUNCTION: ds_is_popwin

    DESCRIPTION:
        Check the popup window is valid or not.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
extern OP_BOOLEAN ds_is_popwin 
( 
    DS_POPUPWIN_T *pwin 
);


/*==================================================================================================
    FUNCTION: ds_is_msg_popup_active

    DESCRIPTION:
        Returns whether an msg popup is currently active or not.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
extern OP_BOOLEAN ds_is_msg_popup_active 
( 
    void 
);


/*==================================================================================================
    FUNCTION: ds_get_popup

    DESCRIPTION:
        Returns the current active popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
extern DS_POPUPWIN_T *ds_get_popup 
(
    void 
);


/*==================================================================================================
    FUNCTION: ds_set_popup_pos

    DESCRIPTION:
        Set popup window position.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        use either ds_set_popup_pos() or ds_set_popup_point 
==================================================================================================*/
extern void ds_set_popup_pos 
(
    DS_POPUPWIN_T    *popwin,
    OP_INT16         left,
    OP_INT16         top,
    OP_INT16         right,
    OP_INT16         bottom
);


/*==================================================================================================
    FUNCTION: ds_set_popup_point

    DESCRIPTION:
        Set popup placing point in the screen. Left top of the popup window is placed on the point.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        use either ds_set_popup_pos() or ds_set_popup_point 
==================================================================================================*/
extern void ds_set_popup_point 
(
    DS_POPUPWIN_T    *popwin,
    OP_INT16         x,
    OP_INT16         y
);


/*==================================================================================================
    FUNCTION: ds_set_popup_softkeys

    DESCRIPTION:
        Set popup softkeys.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern void ds_set_popup_softkeys 
(
    DS_POPUPWIN_T    *popwin,
    OP_UINT8         *left,
    OP_UINT8         *center,
    OP_UINT8         *right
);

/*==================================================================================================
    FUNCTION: ds_set_popup_softkeys_rm

    DESCRIPTION:
        Set popup softkeys.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern void ds_set_popup_softkeys_rm 
(
    DS_POPUPWIN_T       *popwin,
    RM_RESOURCE_ID_T    left_res_id,
    RM_RESOURCE_ID_T    center_res_id,
    RM_RESOURCE_ID_T    right_res_id
);

/*==================================================================================================
    FUNCTION: ds_get_popup_selection

    DESCRIPTION:
        Returns the currently selected item in POPUP_LIST or POPUP_MENU.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern OP_UINT8 ds_get_popup_selection 
(
    DS_POPUPWIN_T    *popwin
);
 

/*==================================================================================================
    FUNCTION: ds_get_popup_check

    DESCRIPTION:
        Returns the currently exclusively selected (checked) item in POPUP_LIST or POPUP_MENU.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern OP_UINT16 ds_get_popup_check 
(
    DS_POPUPWIN_T    *popwin
);


/*==================================================================================================
    FUNCTION: ds_set_popup_selection

    DESCRIPTION:
        Returns the currently selected item in POPUP_LIST, POPUP_MENU, POPUP_DIALOG.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern void ds_set_popup_selection 
(
    DS_POPUPWIN_T    *popwin,
    OP_UINT8         index
);


/*==================================================================================================
    FUNCTION: ds_set_popup_check

    DESCRIPTION:
        Toggle a checkbox icon (exclusively select) in the popup list.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern void ds_set_popup_check 
(
    DS_POPUPWIN_T    *popwin,
    OP_UINT8         index,
    OP_BOOLEAN       onoff
);


/*==================================================================================================
    FUNCTION: ds_get_popup_highlight

    DESCRIPTION:
        Get the current highlight index of the passed popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern OP_UINT16 ds_get_popup_highlight 
(
    DS_POPUPWIN_T    *popwin
);


/*==================================================================================================
    FUNCTION: ds_set_popup_highlight

    DESCRIPTION:
        Set highlight of a popup item.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern void ds_set_popup_highlight 
(
    DS_POPUPWIN_T    *popwin,
    OP_UINT8         index
);

/*==================================================================================================
    FUNCTION: ds_get_popup_edited_number

    DESCRIPTION:
        Returns the edited digits through POPUP_NUMEDITOR. These digits are effective after the user 
        presses OK key (store key).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        The returned number shall be copied by the caller. 
==================================================================================================*/
extern OP_UINT8* ds_get_popup_edited_digits 
(
    DS_POPUPWIN_T    *popwin
);


/*==================================================================================================
    FUNCTION: ds_close_popup

    DESCRIPTION:
        Close the currently active popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None. 
==================================================================================================*/
extern void ds_close_popup 
( 
    void 
);


/*==================================================================================================
    FUNCTION: ds_popup_message

    DESCRIPTION:
        Cleate a message popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
       It is used to open a simple message Popup window such as for warning or temporal messages.
       If the timer is larger than "0", the popup window is automatically closed when the timer
       expires.

       If the timer is less than or equal to  "0", the caller shoud close te popup.

       If you want to open a popup which is continuously alive while you process, 
       use ds_popup_static().
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_message 
(
    OP_UINT8     *title,
    OP_UINT8     *msg,
    OP_UINT16    timer
);


extern DS_POPUPWIN_T * ds_new_popup_message 
(
    OP_UINT8    *title,
    OP_UINT8    *msg,
    OP_UINT16    style,
    OP_UINT16   timer,
    OP_BOOLEAN          (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event)    
);
/*==================================================================================================
    FUNCTION: ds_popup_message_rm

    DESCRIPTION:
        Cleate a message popup window.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
       It is used to open a simple message Popup window such as for warning or temporal messages.
       If the timer is larger than "0", the popup window is automatically closed when the timer
       expires.

       If the timer is less than or equal to  "0", the caller shoud close te popup.

       If you want to open a popup which is continuously alive while you process, 
       use ds_popup_static().
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_message_rm 
(
    RM_RESOURCE_ID_T    title_res_id,
    RM_RESOURCE_ID_T    msg_res_id,
    OP_UINT16           timer
);

/*==================================================================================================
    FUNCTION: ds_popup_static

    DESCRIPTION:
        Open a static popup window. It is alive until it is closed.

    ARGUMENTS PASSED:
        *itle       : popup title
        *sg,        : popup message 
        x           : popup x starting coordinate. If it is 0, it is centered.
        y           : popup y starting coordinate. If it is 0, it is centered.

    RETURN VALUE:

    IMPORTANT NOTES:
        Use ds_popup_message() for simple messages (warning...).
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_static 
(
    OP_UINT8    *title,
    OP_UINT8    *msg,
    OP_INT16    x,            
    OP_INT16    y             
);


/*==================================================================================================
    FUNCTION: ds_popup_static_rm

    DESCRIPTION:
        Open a static popup window. It is alive until it is closed.

    ARGUMENTS PASSED:
        *itle       : popup title
        *sg,        : popup message 
        x           : popup x starting coordinate. If it is 0, it is centered.
        y           : popup y starting coordinate. If it is 0, it is centered.

    RETURN VALUE:

    IMPORTANT NOTES:
        Use ds_popup_message() for simple messages (warning...).
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_static_rm 
(
    RM_RESOURCE_ID_T    title_res_id,
    RM_RESOURCE_ID_T    msg_res_id,
    OP_INT16            x,            
    OP_INT16            y             
);

/*==================================================================================================
    FUNCTION: ds_popup_static_redraw

    DESCRIPTION:
        Redraw an already opened static popup window with the new message.
        Use this function if the message of a static popup is changes.

    ARGUMENTS PASSED:
        *popwin    : the static popup window.
        *msg       : new message. If it is null, the old message remains.
    RETURN VALUE:

    IMPORTANT NOTES:
       None
==================================================================================================*/
extern void ds_popup_static_redraw 
( 
    DS_POPUPWIN_T    *popwin, 
    OP_UINT8         *msg 
);

/*==================================================================================================
    FUNCTION: ds_popup_static_redraw_rm

    DESCRIPTION:
        Redraw an already opened static popup window with the new message.
        Use this function if the message of a static popup is changes.

    ARGUMENTS PASSED:
        *popwin    : the static popup window.
        *msg       : new message. If it is null, the old message remains.
    RETURN VALUE:

    IMPORTANT NOTES:
       None
==================================================================================================*/
extern void ds_popup_static_redraw_rm 
( 
    DS_POPUPWIN_T       *popwin, 
    RM_RESOURCE_ID_T    msg_res_id 
);

/*==================================================================================================
    FUNCTION: ds_popup_dialog

    DESCRIPTION:
        Create and open an dialog type popup. (ask user's action, for example YES and NO).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_dialog 
(
    OP_UINT8      *title,
    OP_UINT8      *msg,
    OP_BOOLEAN    (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event)
);

/*==================================================================================================
    FUNCTION: ds_popup_dialog_rm

    DESCRIPTION:
        Create and open an dialog type popup. (ask user's action, for example YES and NO).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_dialog_rm 
(
    RM_RESOURCE_ID_T    title_res_id,
    RM_RESOURCE_ID_T    msg_res_id,
    OP_BOOLEAN          (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event)
);

/*==================================================================================================
    FUNCTION: ds_popup_menu

    DESCRIPTION:
        Create and open an menu popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_menu 
(
    DS_POPUP_MENU_TIER_T    *menu,
    OP_BOOLEAN              (*event_handler)( DS_POPUP_STATE_ENUM_T state, int event )
);

/*==================================================================================================
    FUNCTION: ds_popup_menu_rm

    DESCRIPTION:
        Create and open an menu popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_menu_rm 
(
    DS_POPUP_MENU_TIER_T    *menu,
    OP_BOOLEAN              (*event_handler)( DS_POPUP_STATE_ENUM_T state, int event )
);

/*==================================================================================================
    FUNCTION: ds_popup_list

    DESCRIPTION:
        Create and open an list popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_list 
(
    DS_POPUP_LIST_T    *list,
    OP_BOOLEAN         (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event )
);

/*==================================================================================================
    FUNCTION: ds_popup_list_rm

    DESCRIPTION:
        Create and open an list popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_list_rm 
(
    DS_POPUP_LIST_T    *list,
    OP_BOOLEAN         (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event )
);

/*==================================================================================================
    FUNCTION: ds_popup_list_ex

    DESCRIPTION:
        Create and open an user data list popup.

    ARGUMENTS PASSED:
        *list         : list
        attr          : list attributes 
        nitems        : total number of items in the lust
        (*event_handler)( DS_POPUP_STATE_ENUM_T state, int event ) : event handler of the list.
        (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data ) : user data handler. First
                      parameter is a user data type and the second parameter is the line index.

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_list_ex 
(
    DS_POPUP_LIST_T    *list,
    POPUPATTR          attr,
    OP_UINT8           nitems,     
    OP_BOOLEAN         (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event ),
    OP_UINT32          (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data )
);


/*==================================================================================================
    FUNCTION: ds_popup_list_ex_rm

    DESCRIPTION:
        Create and open an user data list popup.

    ARGUMENTS PASSED:
        *list         : list
        attr          : list attributes 
        nitems        : total number of items in the lust
        (*event_handler)( DS_POPUP_STATE_ENUM_T state, int event ) : event handler of the list.
        (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data ) : user data handler. First
                      parameter is a user data type and the second parameter is the line index.

    RETURN VALUE:

    IMPORTANT NOTES:

==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_list_ex_rm 
(
    DS_POPUP_LIST_T    *list,
    POPUPATTR          attr,
    OP_UINT8           nitems,     
    OP_BOOLEAN         (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event ),
    OP_UINT32          (*userdata_handler)( DS_POPUP_UDATA_ENUM_T type, OP_UINT32 data )
);


/*==================================================================================================
    FUNCTION: ds_popup_numeditor

    DESCRIPTION:
        Create and open an num editor type popup. (only accept digits).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_numeditor 
(
    DS_POPUP_NUMEDITOR_T    *numeditor,
    OP_BOOLEAN          (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event)
);


/*==================================================================================================
    FUNCTION: ds_popup_numeditor_rm

    DESCRIPTION:
        Create and open an num editor type popup. (only accept digits).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_numeditor_rm 
(
    DS_POPUP_NUMEDITOR_T    *numeditor,
    OP_BOOLEAN          (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event)
);


/*==================================================================================================
    FUNCTION: ds_popup_ownerdraw

    DESCRIPTION:
        Create and open an owner draw popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_ownerdraw 
(
    OP_BOOLEAN    (*event_handler)( DS_POPUP_STATE_ENUM_T state, OP_INT32 event ),
    void          (*ownerdraw_func)( DS_POPUPWIN_T *pw, DS_POPUP_DRAW_INFO_T *dpu)
);


/*==================================================================================================
    FUNCTION: ds_popup_by

    DESCRIPTION:
        Create and open a popup with the user passed popup data.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
extern DS_POPUPWIN_T * ds_popup_by 
(
  DS_POPUP_DATA_T    *popdata
);


/*==================================================================================================
    FUNCTION: ds_pass_event_to_popup

    DESCRIPTION:
        Popup event dispacher. 

    ARGUMENTS PASSED:

    RETURN VALUE:
        Returns the previous popup window handle if exist, otherwise returns PHANDLE_NONE. 
        
    IMPORTANT NOTES:
        None.       
==================================================================================================*/
extern DS_POPUP_PDEH_RET_ENUM_T ds_pass_event_to_popup 
(   
    DS_POPUPWIN_T           *pw,
    OPUS_EVENT_ENUM_TYPE    event, 
    void                    *EvData 
);

/*================================================================================================*/
#endif  /* DS_PUPUP_H */
