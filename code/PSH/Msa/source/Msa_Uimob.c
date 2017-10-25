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

/* !\file mauimob.c
 *  \brief Media object browser UI logic
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Mem.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Mem.h"
#include "Msa_Utils.h"
#include "Msa_Uicmn.h"
#include "Msa_Uisig.h"
#include "Msa_Mob.h"
#include "Msa.h"
#include "Msa_Uimob.h"

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*! \enum MsaUiMobEntryType
 * Identifies the menu entry as a media object or a special operation */
typedef enum
{
    MSA_MOB_SOP,                    /*!< Entry is special operation */
    MSA_MOB_MO                      /*!< Entry is media object */
}MsaUiMobEntryType;

/*! \struct MsaUiMobSpecialOp
 * Data-structure for storing the table of menu selections for special 
   operations */
typedef struct
{
    MsaMobSpecialOp op;             /*!< special operation */
    WE_UINT32 string;              /*!< predefined string wid */
}MsaUiMobSpecialOp;


/*! \struct MsaUiMobEntry
 * Data-structure for storing widget/gadget info for each list entry
 */
typedef struct 
{
    MsaUiMobEntryType   type;           /*!< entry type: object or special op*/
    MsaMediaGroupType   mediaGroup;     /*!< for media objects: type of mo */
    MsaMobSpecialOp     op;             /*!< for special ops: op type */
    WE_UINT32          widgetHandle;   /*!< widget handle */
    int                 mtrInstance;    /*!< mtr instance */
    WeGadgetHandle     gadgetHandle;   /*!< gadget handle */
    WeSize             size;           /*!< Size of gadget after addition */
}MsaUiMobEntry;

/*! \struct MsaUiMobWindow
 * Data-structure for storing widgets and gadgets
 */
typedef struct
{
    int noOfHandles;                /*!< number of handles on page */
    MsaUiMobEntry handles[MSA_MAX_NUMBER_OF_MO_ON_PAGE+MSA_MOB_SOP_COUNT]; /*!<
                                         list of widget/gadget handles */
    WeWindowHandle winHandle;      /*!< the handle for the form */
    WeSize displaySize;            /*!< the size of the display */
    WePosition displayPos;         /*!< Left top of the display */
}MsaUiMobWindow;

/******************************************************************************
 * Constants
 *****************************************************************************/

static const MsaUiMobSpecialOp mobSpecialOps[] =
{
    /* Select next page */
    MSA_MOB_SOP_NEXT_PAGE, (WE_UINT32)MSA_STR_ID_NEXT_SLIDE, 

    /* Select previous page */
    MSA_MOB_SOP_PREV_PAGE, (WE_UINT32)MSA_STR_ID_PREVIOUS_SLIDE,

    /* Add media object */
    MSA_MOB_SOP_ADD_MO, (WE_UINT32)MSA_STR_ID_ADD_MEDIA_OBJECT,

    /* End of list */
    MSA_MOB_SOP_NO_OP, 0
};

#define MSA_MOB_MO_DISTANCE     (2)     /*!< Object distance in pixels */
#define MSA_MOB_MARKER_SIZE     (8)    /*!< Selected object extra size */
#define MSA_MOB_MAX_MO_HEIGHT   (64)    /*!< Max height of a media object 
                                          before it is scaled */
#define MSA_MOB_TEXT_HEIGHT     (16)    /*!< Height of text widget */
#define MSA_MOB_TEXTBOX_HEIGHT  (16)    /*!< Height of textbox widget */
#define MSA_MOB_OP_DISTANCE     (5)     /*!< The space between media objects 
                                             and special operations */

/******************************************************************************
 * Variables
 *****************************************************************************/

static MsaUiMobWindow *msaUiMobWindow;   /*!< Data related to the Mob window */

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static void addWidget(WE_UINT32 handle, int mtrInstance, 
    MsaUiMobEntryType type, MsaMediaGroupType group, MsaMobSpecialOp sop);
static WE_UINT32 *getWidgetHandle(int index);
static int *getWidgetInstance(int index);
static int getWidgetIndex(MsaMediaGroupType type, WE_UINT32 handle, int inst);
static WeGadgetHandle *getGadget(int index);
static MsaMediaGroupType *getMediaGroup(int index);
static MsaUiMobEntryType *getEntryType(int index);
static MsaMobSpecialOp *getSpecialOp(int index);
static void setGadget(int index, WeGadgetHandle gadget);
static void rescale(WeSize *size);
static WE_BOOL msaUiMobCreateTextBox(const char *text);
static WE_BOOL addImageGadget(int index, WeSize *size, WePosition *pos,
    WE_BOOL useNotif);
static WE_BOOL addTextGadget(int index, WeSize *size, WePosition *pos);
static void textBoxLostFocus(WeWindowHandle winHandle);
static void textBoxKeyPressUp(WeWindowHandle winHandle);
static void textBoxKeyPressDown(WeWindowHandle winHandle);
static void textBoxGotFocus(WeWindowHandle winHandle);
static WE_BOOL addTextBoxGadget(int index, WeSize *size, WePosition *pos);
static WE_INT16 centerXPos(const WeSize *size);
static void setWidgetSize(int i, WeSize *size);
static WE_BOOL msaUiMobAddGadgets(void);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Add a widget handle to the list of widgets
 * \param handle    The handle of the widget
 * \param mtrInstance The instance for a mtr textbox
 * \param type      If handle is a media object or a special operation
 * \param group     Media group type (image/text etc), if type = MSA_MOB_MO
 * \param sop       Special operation, if type = MSA_MOB_SOP
 *****************************************************************************/
static void addWidget(WE_UINT32 handle, int mtrInstance, 
    MsaUiMobEntryType type, MsaMediaGroupType group, MsaMobSpecialOp sop)
{
    memset(&msaUiMobWindow->handles[msaUiMobWindow->noOfHandles], 0, 
        sizeof(MsaUiMobEntry));
    msaUiMobWindow->handles[msaUiMobWindow->noOfHandles].type = type;
    switch (type)
    {
    case MSA_MOB_MO:
        msaUiMobWindow->handles[
            msaUiMobWindow->noOfHandles].mediaGroup = group;
        if (MSA_MEDIA_GROUP_TEXT == group)
        {
            msaUiMobWindow->handles[
                msaUiMobWindow->noOfHandles].mtrInstance = mtrInstance;
        }
        else
        {
            msaUiMobWindow->handles[
                msaUiMobWindow->noOfHandles].widgetHandle = handle;
        }
        break;
    case MSA_MOB_SOP:
        msaUiMobWindow->handles[
            msaUiMobWindow->noOfHandles].widgetHandle = handle;
        msaUiMobWindow->handles[msaUiMobWindow->noOfHandles].op = sop;
        break;
    }
    msaUiMobWindow->noOfHandles++;
}

/*!
 * \brief Get a widget handle in the list
 * \param index The position in list
 * \return The widget handle at this position
 *****************************************************************************/
static WE_UINT32 *getWidgetHandle(int index)
{
    return &msaUiMobWindow->handles[index].widgetHandle;
} 

/*!
 * \brief Get an instance in the list
 * \param index The position in list
 * \return The instance at this position
 *****************************************************************************/
static int *getWidgetInstance(int index)
{
    return &msaUiMobWindow->handles[index].mtrInstance;
} 

/*!
 * \brief Returns selected link in a textbox, if a link in a textbox is 
 *        selected.
 * 
 * \param index The index of the widget to investigate for links
 * \return Link data or NULL if none found
 *****************************************************************************/
MtrLinkEntry *msaUiMobGetLink(int index)
{
    MtrLinkEntry *link;    

    if ((MSA_MEDIA_GROUP_TEXT == msaUiMobWindow->handles[index].mediaGroup) &&
        (mtrHasActiveLink(
        (int)msaUiMobWindow->handles[index].mtrInstance , &link)))
    {
        return link;
    }
    else
    {
        return NULL;
    } /* if */
} /* msaUiMobGetLink */


/*!
 * \brief Get a widget's position in the list
 * \param type   the type of widget
 * \param handle the handle to look for (for text/image widgets)
 * \param inst   the instance to look for (for mtr textboxes)
 * \return The position for the widget handle/mtr instance
 *****************************************************************************/
static int getWidgetIndex(MsaMediaGroupType type, WE_UINT32 handle, int inst)
{
    int i;

    for (i = 0; i < msaUiMobWindow->noOfHandles; i++)
    {
        if (type == msaUiMobWindow->handles[i].mediaGroup)
        {
            if (type == MSA_MEDIA_GROUP_TEXT &&
                inst == msaUiMobWindow->handles[i].mtrInstance ||
                type != MSA_MEDIA_GROUP_TEXT &&
                handle == msaUiMobWindow->handles[i].widgetHandle)
            {
                return i;           
            } /* if */
        }
    } /* for */
    return -1;
}

/*!
 * \brief Get a gadget handle in the list
 * \param index The position in list
 * \return The gadget handle at this position
 *****************************************************************************/
static WeGadgetHandle *getGadget(int index)
{
    return &msaUiMobWindow->handles[index].gadgetHandle;
}

/*!
 * \brief Get a media group type of a widget in the list
 * \param index The position in list
 * \return The media group type of the widget at this position
 *****************************************************************************/
static MsaMediaGroupType *getMediaGroup(int index)
{
    return &msaUiMobWindow->handles[index].mediaGroup;
}

/*!
 * \brief Get a entry type of a widget in the list
 * \param index The position in list
 * \return The entry type, i.e. media object or special operation
 *****************************************************************************/
static MsaUiMobEntryType *getEntryType(int index)
{
    return &msaUiMobWindow->handles[index].type;
}

/*!
 * \brief Get the special op of a widget in the list
 * \param index The position in list
 * \return The special operation of the widget at this position
 *****************************************************************************/
static MsaMobSpecialOp *getSpecialOp(int index)
{
    return &msaUiMobWindow->handles[index].op;
}

/*!
 * \brief Set gadget handle for a widget in the list
 * \param index  Position in list
 * \param gadget The gadget handle to set
*****************************************************************************/
static void setGadget(int index, WeGadgetHandle gadget)
{
    msaUiMobWindow->handles[index].gadgetHandle = gadget;
}

/*!
 * \brief Rescale size to fit defined limitation
 * \param size In/out size data
 *****************************************************************************/
static void rescale(WeSize *size)
{
    if (size->width + MSA_MOB_MARKER_SIZE > msaUiMobWindow->displaySize.width)
    {
        size->height = (WE_INT16)((size->height * 
            (msaUiMobWindow->displaySize.width - MSA_MOB_MARKER_SIZE)) / size->width);
        size->width = (WE_INT16)(msaUiMobWindow->displaySize.width - MSA_MOB_MARKER_SIZE);
    }
    if (size->height + MSA_MOB_MARKER_SIZE > MSA_MOB_MAX_MO_HEIGHT)
    {
        size->width = (WE_INT16)((size->width * 
            MSA_MOB_MAX_MO_HEIGHT) / size->height);
        size->height = MSA_MOB_MAX_MO_HEIGHT;
    }
}

/*!
 * \brief Creates an MTR textbox widget and adds it to the list of widgets
 * 
 * \param text The text to display
 * \return TRUE on success, otherwise FALSE
 *****************************************************************************/
static WE_BOOL msaUiMobCreateTextBox(const char *text)
{
    WeStringHandle             sHandle;     
    MtrCreateRspData            *rspDdata;
    int                         num;
    MtrCreateTextBoxProperties  tProp = 
    {
        {0,0},                          /* size (set below) */
        WeCenter,                      /* alignment */
        {0xFF, 0xFF, 0xFF},             /* background white */
        {0},                            /* foreground black*/
        MTR_SCHEME_HTTP             | 
        MTR_SCHEME_HTTPS            | 
        MTR_SCHEME_WWW              | 
        MTR_SCHEME_MAIL             | 
        MTR_SCHEME_PHONE,               /* schemes to recognize */
        MTR_PROP_RENDER_LINKS       |
        MTR_PROP_USE_SCROLLBARS     |
        MTR_PROP_VERTICAL_RESIZE    |
        MTR_PROP_HORIZONTAL_RESIZE  /*|
        MTR_PROP_RETURN_LINKS*/         /* flags */
    };

    tProp.size.width = (WE_INT16)(msaUiMobWindow->displaySize.width - 
        MSA_MOB_MARKER_SIZE);
    /* Use half of the display size for a text part */
    tProp.size.height = (WE_INT16)(msaUiMobWindow->displaySize.height/2); 

    /* Create the text to display */
    if (NULL == text)
    {
        /* The text part is empty, use a empty string instead */
        text = "";
    }
    if (0 == (sHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
        text, WeUtf8, strlen(text) + 1, 0)))
    {
        return FALSE; 
    }   
    
    /* Create the textbox to display it in, add it to the list of widgets */
    if (MTR_RESULT_OK != mtrCreateTextBox(sHandle, &tProp, &rspDdata))
    {
        (void)WE_WIDGET_RELEASE(sHandle);
        return FALSE;
    } /* if */

    /* save gadget handle, size, links */
    addWidget(0, rspDdata->instance, MSA_MOB_MO, MSA_MEDIA_GROUP_TEXT,
        MSA_MOB_SOP_NO_OP);
    num = getWidgetIndex(MSA_MEDIA_GROUP_TEXT, 0, rspDdata->instance);
    setGadget(num, rspDdata->gadgetHandle);
    msaUiMobWindow->handles[num].size = rspDdata->gadgetSize;
    msaUiMobWindow->handles[num].widgetHandle = sHandle;
    
    /* Free response data from MTR */
    mtrFreeMtrCreateRspData(&rspDdata);
    return TRUE;
} /* msaUiMobCreateTextBox */

/*!
 * \brief Add text/image media object to page
 * \param mo   the media object to add 
 * \return TRUE on success
 *****************************************************************************/
WE_BOOL msaUiMobAddWidget(const MsaMoDataItem *mo)
{   
    WeImageHandle imgHandle;
    WeDirectData directData;
    WeCreateData createData;

    createData.directData = &directData;

    /* prevent list overrun */
    if (msaUiMobWindow->noOfHandles >= 
        MSA_MAX_NUMBER_OF_MO_ON_PAGE+MSA_MOB_SOP_COUNT)
    {
        return FALSE;
    }
    
    switch (msaMimeToObjGroup((char *)mo->type->strValue))
    {
    case MSA_MEDIA_GROUP_IMAGE:    
        directData.data = (const char*)mo->data;
        directData.dataSize = (int)mo->size;
        imgHandle = WE_WIDGET_IMAGE_CREATE(WE_MODID_MSA, &createData, 
            (char *)mo->type->strValue, WeResourceBuffer, 0); /* Q04A */
        if (0 == imgHandle)
        {
            /* The image could not be created, display the "broken" icon 
               instead */
            imgHandle = MSA_GET_IMG_ID(MSA_IMG_BROKEN_IMAGE);
        }
        
        addWidget(imgHandle, 0, MSA_MOB_MO, MSA_MEDIA_GROUP_IMAGE, 
            MSA_MOB_SOP_NO_OP);
        return TRUE;
        
    case MSA_MEDIA_GROUP_TEXT:
        return msaUiMobCreateTextBox((char *)mo->data);
        
    case MSA_MEDIA_GROUP_AUDIO:       
        addWidget(MSA_GET_IMG_ID(MSA_IMG_AUDIO), 0, MSA_MOB_MO, 
            MSA_MEDIA_GROUP_AUDIO, MSA_MOB_SOP_NO_OP);
        return TRUE;
        
    case MSA_MEDIA_GROUP_NONE:
    default:
        addWidget(MSA_GET_IMG_ID(MSA_IMG_BROKEN_IMAGE), 0, 
            MSA_MOB_MO, MSA_MEDIA_GROUP_IMAGE, MSA_MOB_SOP_NO_OP);
        return TRUE;
    }
}

/*!
 * \brief Add menu choice for a special operation to list
 * \param op  the special operation to add
 * \return TRUE on successful operation
 *****************************************************************************/
WE_BOOL msaUiMobAddSpecialOp(MsaMobSpecialOp op)
{
    int i;
    char *str;
    WeStringHandle handle; 

    /* find special op in table and create widget accordingly */
    for (i = 0; i < MSA_MOB_SOP_COUNT; i++)
    {
        if (mobSpecialOps[i].op == op)
        {
            if (NULL == (str = msaGetStringFromHandle(MSA_GET_STR_ID(
                mobSpecialOps[i].string))))
            {
                return FALSE;
            }

            handle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, str, WeUtf8, 
                (int)strlen(str) + 1, 0);
            /*lint -e{774} */
            MSA_FREE(str);

            if (0 == handle)
            {
                return FALSE;
            }

            addWidget(handle, 0, MSA_MOB_SOP, MSA_MEDIA_GROUP_NONE,
                mobSpecialOps[i].op);
           
            return TRUE;
        }
    }

    /* none found */
    return FALSE;
}

/*!
 * \brief Create and add new image gadget
 * \param index Widget number in list
 * \param size  Struct containing size information
 * \param pos   Struct containing position information
 * \param useNotif Register notification changes for this gadget
 * \return TRUE on successful operation
 *****************************************************************************/
static WE_BOOL addImageGadget(int index, WeSize *size, WePosition *pos,
    WE_BOOL useNotif)
{
    WeGadgetHandle handle;
    if (NULL == size || NULL == pos)
    {
        return FALSE;
    }
    setWidgetSize(index, size);
    pos->x = centerXPos(size);
    handle = WE_WIDGET_IMAGE_GADGET_CREATE(WE_MODID_MSA, 
        *getWidgetHandle(index), size, WeImageZoomAutofit,
        WE_GADGET_PROPERTY_FOCUS, 0); /* Q04A */

    if (0 == handle)
    {
        return FALSE; 
    }       

    setGadget(index, handle);
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaUiMobWindow->winHandle, 
        *getGadget(index), pos)) /* Q04A */
    {
        (void)WE_WIDGET_RELEASE(handle);
        return FALSE;
    }

    /* Focus change action */
    if (useNotif)
    {
        if (0 == msaRegisterNotification(*getGadget(index),
            WeNotifyLostFocus, NULL, MSA_MOB_FSM, MSA_SIG_MOB_FOCUS_ACTIVITY))
        {
            (void)WE_WIDGET_REMOVE(msaUiMobWindow->winHandle, handle);
            (void)WE_WIDGET_RELEASE(handle);
            return FALSE;
        }
    }
    
    return TRUE;
}

/*!
 * \brief Create and add new text gadget
 * \param index Widget number in list
 * \param size  The size of the gadget that was created.
 * \param pos   Struct containing position information
 * \return TRUE on successful operation
 *****************************************************************************/
static WE_BOOL addTextGadget(int index, WeSize *size, WePosition *pos)
{   
    WeGadgetHandle handle;
    WeStringHandle strHandle;
    if (NULL == size || NULL == pos)
    {
        return FALSE;
    }
    strHandle = *getWidgetHandle(index);
    (void)WE_WIDGET_GET_SIZE(strHandle, size);
    pos->x = centerXPos(size);
    handle = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA, 
        *getWidgetHandle(index), NULL, 0, FALSE,
        WE_GADGET_PROPERTY_FOCUS | WE_GADGET_PROPERTY_SCROLLBARVER, 0);
    if (0 == handle)
    {
        return FALSE;
    }    
    
    setGadget(index, handle);

    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaUiMobWindow->winHandle,
        *getGadget(index), pos)) /* Q04A */
    {
        (void)WE_WIDGET_RELEASE(handle);
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Callback received when a textbox looses focus 
 * 
 * \param winHandle the gadget handle of the textbox that lost focus
 *****************************************************************************/
static void textBoxLostFocus(WeWindowHandle winHandle)
{
    (void)winHandle;
    if (msaUiMobWindow->noOfHandles > 1) 
    {
        /* Only delete user events if we have another gadget to go to */
        (void)msaDeleteUserEvent(msaUiMobWindow->winHandle, WeKey_Up   );
        (void)msaDeleteUserEvent(msaUiMobWindow->winHandle, WeKey_Down );
    }
} /* textBoxLostFocus */

/*!
 * \brief Callback called when a navigation key is pressed and a textbox has 
 *        focus
 * \param winHandle the textbox
 *****************************************************************************/
static void textBoxKeyPressUp(WeWindowHandle winHandle)
{
    WE_BOOL lostFocus;
    int index;
    
    index = msaUiMobCheckFocus();

    if (index >= 0)
    {
        if (MTR_RESULT_OK != mtrRedraw(*getWidgetInstance(index), WeKey_Up,
            &lostFocus) || lostFocus)
        {
            textBoxLostFocus(winHandle);
        } /* if */
    }
}

/*!
 * \brief Callback called when a navigation key is pressed and a textbox has 
 *        focus
 * \param winHandle the textbox
 *****************************************************************************/
static void textBoxKeyPressDown(WeWindowHandle winHandle)
{
    WE_BOOL lostFocus;
    int index;
    
    index = msaUiMobCheckFocus();

    if (index >= 0)
    {
        if (MTR_RESULT_OK != mtrRedraw(*getWidgetInstance(index), WeKey_Down,
            &lostFocus) || lostFocus)
        {
            textBoxLostFocus(winHandle);
        } /* if */
    }
}

/*!
 * \brief Callback received when a textbox gets focus  
 * \param winHandle  the window handle in which the textbox got focus
 *****************************************************************************/
static void textBoxGotFocus(WeWindowHandle winHandle)
{   
    (void)winHandle;

    /* Textbox has focus! Handle the events for the navigation keys. */
    (void)msaRegisterUserEvent(msaUiMobWindow->winHandle, WeKey_Up, 
        textBoxKeyPressUp, 0, 0);
    (void)msaRegisterUserEvent(msaUiMobWindow->winHandle, WeKey_Down,
        textBoxKeyPressDown, 0, 0);
} /* textBoxGotFocus */

/*!
 * \brief Add new textbox gadget
 * \param index Widget number in list
 * \param pos   Struct containing position information
 * \return TRUE on successful operation
 *****************************************************************************/
static WE_BOOL addTextBoxGadget(int index, WeSize *size, WePosition *pos)
{   
    /* Set the size */
    (void)WE_WIDGET_GET_SIZE(msaUiMobWindow->handles[index].gadgetHandle, 
        size);
    pos->x = centerXPos(size);
    /* Gadget is already created, just add it to window */
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaUiMobWindow->winHandle, 
        msaUiMobWindow->handles[index].gadgetHandle, pos)) /* Q04A */
    {
        return FALSE;
    }
    
    /* Focus change actions */
    if (0 == msaRegisterNotification(*getGadget(index), WeNotifyFocus, 
        textBoxGotFocus, 0, 0))
    {
        return FALSE;
    }
/*    WE_WIDGET_SET_IN_FOCUS(*getGadget(index), 1); */
    return TRUE;
}

/*!
 * \brief Calculate X position to center object, based on size
 * \param size size of object 
 * \return The calculated x position
 *****************************************************************************/
static WE_INT16 centerXPos(const WeSize *size)
{
    return (WE_INT16)(msaUiMobWindow->displayPos.x + 
        (msaUiMobWindow->displaySize.width - size->width)/2);
}


/*!
 * \brief Set size for a widget, possibly with rescaling if too big 
 * \param i    The widget number in list
 * \param size Where to store size data
 *****************************************************************************/
static void setWidgetSize(int i, WeSize *size)
{
    if (!WE_WIDGET_GET_SIZE(*getWidgetHandle(i), size))
    {
        size->height = 2;
        size->width = 2;
        return; 
    }
    rescale(size);
}

/*!
 * \brief Create and add gadgets to form
 * \param The window struct to add gadgets to
 * \return TRUE on successful operation
 *****************************************************************************/
static WE_BOOL msaUiMobAddGadgets(void)
{
    int             i;
    WePosition     pos = {MSA_GADGET_DEF_PADDING, MSA_GADGET_DEF_PADDING};
    WeSize         size = {0,0};

    /* Show media objects first */
    for (i = 0 ; i < msaUiMobWindow->noOfHandles; i++)
    {
        /* Add image gadgets for images/audio */
        if (MSA_MOB_MO == *getEntryType(i) &&
            (MSA_MEDIA_GROUP_IMAGE == *getMediaGroup(i) ||
            MSA_MEDIA_GROUP_AUDIO == *getMediaGroup(i)))
        {
            if (!addImageGadget(i, &size, &pos,
                (MSA_MEDIA_GROUP_AUDIO == *getMediaGroup(i))))
            {
                return FALSE;
            }
            pos.y = (WE_INT16)(pos.y + size.height + MSA_MOB_MO_DISTANCE); 
        }
        else if (MSA_MEDIA_GROUP_TEXT == *getMediaGroup(i))
        {
            if (!addTextBoxGadget(i, &size, &pos))
            {
                return FALSE;
            }
            pos.y = (WE_INT16)(pos.y + size.height + MSA_MOB_MO_DISTANCE); 
        } /* if */
    }
    /* Add distance between the objects and the special operations */
    if (pos.y != 0)
    {
        pos.y = (WE_INT16)(pos.y + MSA_MOB_OP_DISTANCE);
    }
    /* Show special operations */
    for (i = 0 ; i < msaUiMobWindow->noOfHandles; i++)
    {
        if (MSA_MOB_SOP == *getEntryType(i))
        {
            if (!addTextGadget(i, &size, &pos))
            {
                return FALSE;
            }
            pos.y = (WE_INT16)(pos.y + size.height + MSA_MOB_MO_DISTANCE);
        }
    }
    return TRUE;
}

/*!
 * \brief Create a form and set its size
 * \return TRUE upon success
 *****************************************************************************/
WE_BOOL msaUiInitializeForm(void)
{
    /* Create form */
    if (0 == (msaUiMobWindow->winHandle = WE_WIDGET_FORM_CREATE(WE_MODID_MSA,
        NULL, 0, 0)))  /* Q04A */
    {
        return FALSE;
    }
    (void)WE_WIDGET_GET_INSIDE_AREA(msaUiMobWindow->winHandle, 
        &msaUiMobWindow->displayPos, &msaUiMobWindow->displaySize); /* Q04A */

    return TRUE;
} /* msaUiInitializeForm */

/*!
 * \brief Create form
 * \param winTitle Preallocated window title
 * \return TRUE on successful operation
 *****************************************************************************/
WE_BOOL msaUiMobFinalizeForm(WE_UINT32 winTitle)
{

    /* Set title */
    if (!msaSetTitle(msaUiMobWindow->winHandle,
        MSA_GET_STR_ID(winTitle), 0))
    {
        return FALSE;
    }
   
    /* Gadgets */
    if (!msaUiMobAddGadgets())
    {
        return FALSE;
    }

    /* Ok action */
    if (0 == msaCreateAction(msaUiMobWindow->winHandle, WeSelect,
        NULL, MSA_MOB_FSM, MSA_SIG_MOB_OK_SELECTED, MSA_STR_ID_SELECT))
    {
        return FALSE;
    }
    /* Back action */
    if (0 == msaCreateAction(msaUiMobWindow->winHandle, WeBack, 
        NULL, MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE, MSA_STR_ID_BACK))
    {
        return FALSE;
    }

    /* Menu action */
    if (0 == msaCreateAction(msaUiMobWindow->winHandle, WeMenu, 
        NULL, MSA_MOB_FSM, MSA_SIG_MOB_MENU_SELECTED, MSA_STR_ID_MENU))
    {
        return FALSE;
    }    

    (void)msaRegisterUserEvent(msaUiMobWindow->winHandle, WeKey_Left,  NULL, 
        MSA_MOB_FSM, MSA_SIG_MOB_KEY_LEFT);
    (void)msaRegisterUserEvent(msaUiMobWindow->winHandle, WeKey_Right, NULL, 
        MSA_MOB_FSM, MSA_SIG_MOB_KEY_RIGHT);
    
    return msaDisplayWindow(msaUiMobWindow->winHandle, msaGetPosLeftTop());    
}

/*!
 * \brief Check which widget is in focus
 * \return Index in list
 *****************************************************************************/
int msaUiMobCheckFocus(void)
{
    int i;

    for (i = 0; i < msaUiMobWindow->noOfHandles; i++)
    {
        if (WE_WIDGET_HAS_FOCUS(*getGadget(i)))
        {
            return i;
        }
    }
    return -1;
}

/*!
 * \brief Return which special operation was selected
 * \return The operation or MSA_MOB_SOP_NO_OP if not special op was selected
 *****************************************************************************/
MsaMobSpecialOp msaUiMobGetSpecialOp(void)
{
    int i = msaUiMobCheckFocus();
    
    if (i < 0 || *getEntryType(i) != MSA_MOB_SOP)
    {
        return MSA_MOB_SOP_NO_OP;
    }

    return *getSpecialOp(i);    
}

/*!
 * \brief Delete all widgets, gadgets, window
 *****************************************************************************/
void msaUiMobDeleteAll(void)
{
    int i;

    if (msaUiMobWindow->winHandle)
    {
        (void)msaDeleteUserEvent(msaUiMobWindow->winHandle, WeKey_Left );
        (void)msaDeleteUserEvent(msaUiMobWindow->winHandle, WeKey_Right);
        (void)msaDeleteUserEvent(msaUiMobWindow->winHandle, WeKey_Up   );
        (void)msaDeleteUserEvent(msaUiMobWindow->winHandle, WeKey_Down );

        for (i = 0; i < msaUiMobWindow->noOfHandles; i++)
        {
            /* free MTR instance for text boxes */
            if ((MSA_MOB_MO == msaUiMobWindow->handles[i].type) && 
                (MSA_MEDIA_GROUP_TEXT == msaUiMobWindow->handles[i].mediaGroup))
            {
                (void)msaDeleteNotification(msaUiMobWindow->handles[i].gadgetHandle);
                msaUiMobWindow->handles[i].gadgetHandle = 0;

                /* takes care of both widget and gadget destruction */
                (void)mtrDeleteTextBox(
                    msaUiMobWindow->handles[i].mtrInstance);
                msaUiMobWindow->handles[i].mtrInstance = 0;
            }

            /* free widget */
            if (msaUiMobWindow->handles[i].widgetHandle)
            {
                (void)WE_WIDGET_RELEASE(
                    msaUiMobWindow->handles[i].widgetHandle);
                msaUiMobWindow->handles[i].widgetHandle = 0;
            }

            /* free gadget */
            if (msaUiMobWindow->handles[i].gadgetHandle)
            {
                (void)WE_WIDGET_RELEASE(
                    msaUiMobWindow->handles[i].gadgetHandle);
                (void)msaDeleteNotification(
                    msaUiMobWindow->handles[i].gadgetHandle);
                msaUiMobWindow->handles[i].gadgetHandle = 0;
            }
        }   
        /* Delete form and actions */
        (void)msaDeleteWindow(msaUiMobWindow->winHandle);
        msaUiMobWindow->winHandle = 0;
    }
    msaUiMobWindow->noOfHandles = 0;
}

/*!
 * \brief Initialize the mob UI 
 *****************************************************************************/
void msaUiMobInit(void)
{
    MSA_CALLOC(msaUiMobWindow, sizeof(MsaUiMobWindow));
}

/*!
 * \brief Terminate the mob UI
 *****************************************************************************/
void msaUiMobTerminate(void)
{
    if (msaUiMobWindow != NULL)
    {
        msaUiMobDeleteAll();
        MSA_FREE(msaUiMobWindow);
        msaUiMobWindow = NULL;
    }
}

