#ifndef INT_WIDGET_FOCUS_H
#define INT_WIDGET_FOCUS_H
/*==================================================================================================

    HEADER NAME : int_widget_focus.h

    GENERAL DESCRIPTION
        General description of the contents of this header file.

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    07/26/2003   Steven Lai             Cxxxxxx     Initial file creation.
    08/20/2003   Steven Lai       P001310       Improve the method of checking whether a widget is in focus
    08/29/2003    Zhuxq             P001396      Add scroll-contents feature to string gadget
    09/26/2003   linda wang        P001451      separate the gif play and show function.    
    10/23/2003   Zhuxq             P001842     Optimize the paintbox and input method solution and fix some bug
    03/31/2004   Dingjianxin         P002754     Delete the function protype of imagegadgetGetFocus and imagegadgetLoseFocus 
    06/09/2004    zhuxq            P006048        forbid Menu to respond successive multiple actions
    Self-documenting Code
    This file provide some API for interface between OPUS SOCKET SP and AUS MSF !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
/*==================================================================================================
    CONSTANTS
==================================================================================================*/


/*==================================================================================================
    MACROS
==================================================================================================*/
/*
 * description of this macro, if needed. - Remember self documenting code
 */

/* indicates if the window is scrolled   */
#define WIDGET_WINDOW_SCROLLED     2
/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/



/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

typedef int ( *pfWidGetFocus) (void *);
typedef void ( *pfWidLoseFocus) (void *);



typedef struct _WidFocusFunc
{
  pfWidGetFocus pWidGetFocus;
  pfWidLoseFocus pWidLoseFocus;

}WidFocusFunc;


/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/

extern WidFocusFunc msfWidFocusFuncs[];


/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/
int screenGetFocus(void *pWidget);
void screenLoseFocus(void *pWidget);
int windowGetFocus(void *pWidget);
void windowLoseFocus(void *pWidget);

void dialogLoseFocus(void * pWidget);
void menuLoseFocus(void * pWidget);
#if 0
void editorGetFocus(void *pWidget);
void editorLoseFocus(void *pWidget);
#endif

int gadgetGetFocus(void *pWidget);
void gadgetLoseFocus(void *pWidget);
void selectgroupLoseFocus(void *pWidget);
int textinputGetFocus(void *pWidget);
void textinputLostFocus(void *pWidget);
void stringGadgetLoseFocus(void *pWidget);

extern int removeScreenFocus(MsfScreen *pScreen);
extern int removeWindowFocus(MsfWindow *pWin);
extern int removeGadgetFocus(MsfGadget *pGadget);

extern int setScreenInFocus(MsfScreen  *pScreen);
extern int setWindowInFocus(MsfWindow *pWin);
extern int setGadgetInFocus(MsfGadget *pGadget);


/*================================================================================================*/

#endif  /* INT_WIDGET_FOCUS_H */


