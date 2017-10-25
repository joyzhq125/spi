

#include "Uba_Kvc.h"

#include "Uba_Cfg.h"
#include "Uba_Gui.h"
#include "Uba_Rc.h"
#include "Uba_Pitm.h"

#include "Ubs_If.h"

#include "We_Cfg.h"

/************************************************************************/
/* Constants                                                            */
/************************************************************************/


/*
 *  Key value criteria wid bit masks
 *
 */
#define UBA_KVC_1         (uba_page_criteria_t) (1)
#define UBA_KVC_2         (uba_page_criteria_t) (1 << 1)
#define UBA_KVC_3         (uba_page_criteria_t) (1 << 2)
#define UBA_KVC_4         (uba_page_criteria_t) (1 << 3)
#define UBA_KVC_5         (uba_page_criteria_t) (1 << 4)
#define UBA_KVC_6         (uba_page_criteria_t) (1 << 5)
#define UBA_KVC_7         (uba_page_criteria_t) (1 << 6)
#define UBA_KVC_8         (uba_page_criteria_t) (1 << 7)
#define UBA_KVC_9         (uba_page_criteria_t) (1 << 8)
#define UBA_KVC_10        (uba_page_criteria_t) (1 << 9)
#define UBA_KVC_11        (uba_page_criteria_t) (1 <<10)
#define UBA_KVC_12        (uba_page_criteria_t) (1 <<11)
#define UBA_KVC_13        (uba_page_criteria_t) (1 <<12)
#define UBA_KVC_14        (uba_page_criteria_t) (1 <<13)
#define UBA_KVC_15        (uba_page_criteria_t) (1 <<14)
#define UBA_KVC_16        (uba_page_criteria_t) (1 <<15)
#define UBA_KVC_17        (uba_page_criteria_t) (1 <<16)
#define UBA_KVC_18        (uba_page_criteria_t) (1 <<17)
#define UBA_KVC_19        (uba_page_criteria_t) (1 <<18)
#define UBA_KVC_20        (uba_page_criteria_t) (1 <<19)
/*TR17904*/
#define UBA_KVC_21        (uba_page_criteria_t) (1 <<20)
#define UBA_KVC_22        (uba_page_criteria_t) (1 <<21)


/************************************************************************/
/* Global variables                                                     */
/************************************************************************/


/*
 *  Defines a criteria used in uba_page_item_icon_menu_map
 *  First is KVC_1 and so on.
 *  KEY = in a ubs msg it is the wid (key) on a Titel property
 *  VALUE = The expected type for this key
 *  data len = the the total leng of value to compare
 *  value = byte array of a value
 */
                    /* KEY */             /* VALUE type */    /*data len */  /*value[UBA_KEY_VALUE_CRITERIA_MAX_LENGTH]*/
                                                                             /* MSB first in numbers */
const uba_page_item_key_value_criteria_t  uba_page_item_key_value_criterias[] =
{
  {/* UBA_KVC_1 */ UBS_MSG_KEY_SUBTYPE,       UBS_VALUE_TYPE_UINT32, 1, {'m',0}                                     },
  {/* UBA_KVC_2 */ UBS_MSG_KEY_SUBTYPE,       UBS_VALUE_TYPE_UINT32, 1, {'n',0}                                     },
  {/* UBA_KVC_3 */ UBS_MSG_KEY_SUBTYPE,       UBS_VALUE_TYPE_UINT32, 1, {'q',0}                                     },
  {/* UBA_KVC_4 */ UBS_MSG_KEY_SUBTYPE,       UBS_VALUE_TYPE_UINT32, 1, {'r',0}                                     },
  {/* UBA_KVC_5 */ UBS_MSG_KEY_SUBTYPE,       UBS_VALUE_TYPE_UINT32, 1, {'d',0}                                     },
  {/* UBA_KVC_6 */ UBS_MSG_KEY_SUBTYPE,       UBS_VALUE_TYPE_UINT32, 1, {'s',0}                                     },
  {/* UBA_KVC_7 */ UBS_MSG_KEY_SUBTYPE,       UBS_VALUE_TYPE_UINT32, 1, {'e',0}                                     },
  {/* UBA_KVC_8 */ UBS_MSG_KEY_SUBTYPE,       UBS_VALUE_TYPE_UINT32, 1, {'v',0}                                     },
  {/* UBA_KVC_9 */ UBS_MSG_KEY_READ,          UBS_VALUE_TYPE_BOOL,   1, {TRUE,0}                                    },
  {/* UBA_KVC_10*/ UBS_MSG_KEY_READ,          UBS_VALUE_TYPE_BOOL,   1, {FALSE,0}                                   },
  {/* UBA_KVC_11*/ UBS_MSG_KEY_PRIORITY,      UBS_VALUE_TYPE_UINT32, 1, {UBS_MSG_KEY_VALUE_PRIORITY_NORMAL,0}       },
  {/* UBA_KVC_12*/ UBS_MSG_KEY_PRIORITY,      UBS_VALUE_TYPE_UINT32, 1, {UBS_MSG_KEY_VALUE_PRIORITY_HIGH,0}         },
  {/* UBA_KVC_13*/ UBS_MSG_KEY_PRIORITY,      UBS_VALUE_TYPE_UINT32, 1, {UBS_MSG_KEY_VALUE_PRIORITY_LOW,0}          },
  {/* UBA_KVC_14*/ UBS_MSG_KEY_FOLDER,        UBS_VALUE_TYPE_UTF8,   2, {'t', 0x00}                                 },
  {/* UBA_KVC_15*/ UBS_MSG_KEY_FOLDER,        UBS_VALUE_TYPE_UTF8,   2, {'d', 0x00}                                 },
  {/* UBA_KVC_16*/ UBS_MSG_KEY_FOLDER,        UBS_VALUE_TYPE_UTF8,   2, {'o', 0x00}                                 },
  {/* UBA_KVC_17*/ UBS_MSG_KEY_FOLDER,        UBS_VALUE_TYPE_UTF8,   2, {'s', 0x00}                                 },
  {/* UBA_KVC_18*/ UBS_MSG_KEY_FORWARD_LOCK,  UBS_VALUE_TYPE_BOOL,   1, {TRUE,0}                                    },
  {/* UBA_KVC_19*/ UBS_MSG_KEY_FORWARD_LOCK,  UBS_VALUE_TYPE_BOOL,   1, {FALSE,0}                                   },
  {/* UBA_KVC_20*/ UBS_MSG_KEY_MSG_STATUS,    UBS_VALUE_TYPE_UINT32, 1, {UBS_MSG_KEY_VALUE_MSG_STATUS_INCOMPLETE,0} },
  /*TR 17904*/
  {/* UBA_KVC_21*/ UBS_MSG_KEY_CLASS,         UBS_VALUE_TYPE_BOOL,  1, {TRUE,0}                                     },
  {/* UBA_KVC_22*/ UBS_MSG_KEY_CLASS,         UBS_VALUE_TYPE_BOOL,  1, {FALSE,0}                                    },
  {                UBA_END_KEY,               UBA_END_TYPE,          0, {0x00,0x00}                                 }
};


/*
 *  Define helpers to make it easier to understand what a key value
 *  criteria msans
 */
#define SUB_TYPE_M    UBA_KVC_1
#define SUB_TYPE_N    UBA_KVC_2
#define SUB_TYPE_Q    UBA_KVC_3
#define SUB_TYPE_R    UBA_KVC_4
#define SUB_TYPE_D    UBA_KVC_5
#define SUB_TYPE_S    UBA_KVC_6
#define SUB_TYPE_E    UBA_KVC_7
#define SUB_TYPE_V    UBA_KVC_8
#define READ_TRUE     UBA_KVC_9
#define READ_FALSE    UBA_KVC_10
#define PRIO_NORM     UBA_KVC_11
#define PRIO_HIGH     UBA_KVC_12
#define PRIO_LOW      UBA_KVC_13
#define FOLD_T        UBA_KVC_14
#define FOLD_D        UBA_KVC_15
#define FOLD_O        UBA_KVC_16
#define FOLD_S        UBA_KVC_17
#define LOCK_TRUE     UBA_KVC_18
#define LOCK_FALSE    UBA_KVC_19
#define STATUS_INCOMP UBA_KVC_20
/*TR 17904*/
#define REPORT_TRUE   UBA_KVC_21
#define REPORT_FALSE  UBA_KVC_22



/*
 * What icon and menu a msg i connected to
 * The parser works top down and quits after first full match
 * A match is when the message type is correct and all the key value criterias is correct
 * After a match the resource engine can look up the menu, icon to use it gets also
 * The text to present in a page
 *
 * Title property mask     The title propertys that must match,
 * Message Type            The message type must match
 * Menu WID                 Use this menu if match
 * Icon WID                 Use this icon if match
 * Primary text property   Use this UTF8 property as text
 * Secondary text property if not primary text exsists use this utf8 property as text
 *
 */
/*        Title Property Mask,            Message Type,          Menu Id,                          Icon Id                Primary text property       Secondary text property   */
const uba_page_item_icon_menu_t uba_page_item_icon_menu_map[] =
{
#ifdef WE_MODID_MMS
  /* MMS subtype m, received message */
  {SUB_TYPE_M | READ_TRUE | LOCK_TRUE | PRIO_HIGH,  UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_MMS_LOCK_H_R,  UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_TRUE | LOCK_TRUE | PRIO_LOW,   UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_MMS_LOCK_L_R,  UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_TRUE | LOCK_TRUE,              UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_MMS_LOCK_R,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_TRUE | PRIO_HIGH,              UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_MSG_ID,          UBA_IMG_ID_MMS_H_R,       UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_TRUE | PRIO_LOW,               UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_MSG_ID,          UBA_IMG_ID_MMS_L_R,       UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_TRUE,                          UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_MSG_ID,          UBA_IMG_ID_MMS_R,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_FALSE | LOCK_TRUE | PRIO_HIGH, UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_MMS_LOCK_H,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_FALSE | LOCK_TRUE | PRIO_LOW,  UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_MMS_LOCK_L,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_FALSE | LOCK_TRUE,             UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_MMS_LOCK,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_FALSE | PRIO_HIGH,             UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_MSG_ID,          UBA_IMG_ID_MMS_H,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_FALSE | PRIO_LOW,              UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_MSG_ID,          UBA_IMG_ID_MMS_L,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | READ_FALSE,                         UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_MSG_ID,          UBA_IMG_ID_MMS,           UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M | LOCK_TRUE,                          UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_MMS_LOCK,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_M,                                      UBS_MSG_TYPE_MMS, UBA_GUI_MENU_INBOX_MMS_MSG_ID,          UBA_IMG_ID_MMS,           UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* MMS subtype q, notification */
  {SUB_TYPE_Q | READ_TRUE,              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_NOTIF_ID,  UBA_IMG_ID_MMS_N,       UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_Q | READ_FALSE,             UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_NOTIF_ID,  UBA_IMG_ID_MMS_N,       UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_Q,                          UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_NOTIF_ID,  UBA_IMG_ID_MMS_N,       UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* MMS subtype n, notification */
  {SUB_TYPE_N | READ_TRUE,              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_NOTIF_ID,  UBA_IMG_ID_MMS_N,       UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_N | READ_FALSE,             UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_NOTIF_ID,  UBA_IMG_ID_MMS_N,       UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_N,                          UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_NOTIF_ID,  UBA_IMG_ID_MMS_N,       UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* MMS subtype r, read report */
  {SUB_TYPE_R | READ_TRUE,              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_RR_ID,     UBA_IMG_ID_MMS_RR_R,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_R | READ_FALSE,             UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_RR_ID,     UBA_IMG_ID_MMS_RR,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_R,                          UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_RR_ID,     UBA_IMG_ID_MMS_RR,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* MMS subtype d, delivery report*/
  {SUB_TYPE_D | READ_TRUE,              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_DR_ID,     UBA_IMG_ID_MMS_DR_R,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_D | READ_FALSE,             UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_DR_ID,     UBA_IMG_ID_MMS_DR,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_D,                          UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_INBOX_MMS_DR_ID,     UBA_IMG_ID_MMS_DR,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* MMS report status TRUE, read report in outbox TR 17904*/
  {FOLD_O | REPORT_TRUE,              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_OUTBOX_MMS_RR_ID,     UBA_IMG_ID_MMS_RR_R,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {FOLD_D | REPORT_TRUE,              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_OUTBOX_MMS_RR_ID,     UBA_IMG_ID_MMS_RR,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* MMS subtype s */
  {FOLD_T,                              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_TEMPLATES_MMS_ID,    UBA_IMG_ID_MMS,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_TO      },
  {FOLD_D,                              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_DRAFTS_MMS_MSG_ID,   UBA_IMG_ID_MMS,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_TO      },
  {FOLD_O,                              UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_OUTBOX_MMS_MSG_ID,   UBA_IMG_ID_MMS_OUTBOX,  UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_TO      },
  {SUB_TYPE_S,                          UBS_MSG_TYPE_MMS,     UBA_GUI_MENU_SENT_MMS_MSG_ID,     UBA_IMG_ID_MMS,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_TO      },
#endif
#ifdef WE_MODID_EMS
  /* EMS/SMS subtype 'v', voice mail */
  {SUB_TYPE_V | READ_TRUE,              UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_INBOX_EMS_VM_ID,     UBA_IMG_ID_EMS_VM_R,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_V | READ_FALSE,             UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_INBOX_EMS_VM_ID,     UBA_IMG_ID_EMS_VM,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_V,                          UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_INBOX_EMS_VM_ID,     UBA_IMG_ID_EMS_VM,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* EMS/SMS subtype 'd', discard voice mail */
  {SUB_TYPE_D,                          UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_INBOX_EMS_DVM_ID,    UBA_IMG_ID_EMS_DVM_R,   UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* EMS/SMS subtype 's', status report */
  {SUB_TYPE_S | READ_TRUE,              UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_INBOX_EMS_SR_ID,     UBA_IMG_ID_EMS_SR_R,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_S | READ_FALSE,             UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_INBOX_EMS_SR_ID,     UBA_IMG_ID_EMS_SR,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_S,                          UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_INBOX_EMS_SR_ID,     UBA_IMG_ID_EMS_SR,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* EMS/SMS subtype 'e', ems/sms message */
  {SUB_TYPE_E | FOLD_T,                 UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_TEMPLATES_EMS_ID,    UBA_IMG_ID_EMS,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | FOLD_D,                 UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_DRAFTS_EMS_MSG_ID,   UBA_IMG_ID_EMS,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | FOLD_O,                 UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_OUTBOX_EMS_MSG_ID,   UBA_IMG_ID_EMS_OUTBOX,  UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | FOLD_S,                 UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_SENT_EMS_MSG_ID,     UBA_IMG_ID_EMS,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | STATUS_INCOMP,          UBS_MSG_TYPE_EMS,     UBA_GUI_MENU_INBOX_EMS_INCOMP_MSG_ID, UBA_IMG_ID_EMS_INCOMP, UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM },
  {SUB_TYPE_E | READ_TRUE | LOCK_TRUE | PRIO_HIGH, UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_EMS_LOCK_H_R,   UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_TRUE | LOCK_TRUE | PRIO_LOW,  UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_EMS_LOCK_L_R,   UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_TRUE | LOCK_TRUE,             UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_EMS_LOCK_R,     UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_TRUE | PRIO_HIGH,             UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_MSG_ID,          UBA_IMG_ID_EMS_H_R,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_TRUE | PRIO_LOW,              UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_MSG_ID,          UBA_IMG_ID_EMS_L_R,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_TRUE,                         UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_MSG_ID,          UBA_IMG_ID_EMS_R,          UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_FALSE | LOCK_TRUE | PRIO_HIGH, UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_EMS_LOCK_H,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_FALSE | LOCK_TRUE | PRIO_LOW,  UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_EMS_LOCK_L,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_FALSE | LOCK_TRUE,             UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_EMS_LOCK,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_FALSE | PRIO_HIGH,             UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_MSG_ID,          UBA_IMG_ID_EMS_H,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_FALSE | PRIO_LOW,              UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_MSG_ID,          UBA_IMG_ID_EMS_L,         UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | READ_FALSE,                         UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_MSG_ID,          UBA_IMG_ID_EMS,           UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E | LOCK_TRUE,                          UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID, UBA_IMG_ID_EMS_LOCK,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {SUB_TYPE_E,                                      UBS_MSG_TYPE_EMS, UBA_GUI_MENU_INBOX_EMS_MSG_ID,          UBA_IMG_ID_EMS,           UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
#endif
#ifdef WE_MODID_PUS
  /* PUSH SI */
  {READ_TRUE  | PRIO_NORM,              UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH_R,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_TRUE  | PRIO_HIGH,              UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH_H_R,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_TRUE  | PRIO_LOW,               UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH_R,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_TRUE,                           UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH_R,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_FALSE | PRIO_NORM,              UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_FALSE | PRIO_LOW,               UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_FALSE | PRIO_HIGH,              UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH_H,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_FALSE,                          UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {0,                                   UBS_MSG_TYPE_PUSH_SI, UBA_GUI_MENU_INBOX_PUSH_SI_ID,    UBA_IMG_ID_PUSH,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  /* PUSH SL */
  {READ_TRUE  | PRIO_NORM,              UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH_R,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_TRUE  | PRIO_HIGH,              UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH_H_R,    UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_TRUE  | PRIO_LOW,               UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH_R,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_TRUE,                           UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH_R,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_FALSE | PRIO_NORM,              UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_FALSE | PRIO_LOW,               UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_FALSE | PRIO_HIGH,              UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH_H,      UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {READ_FALSE,                          UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
  {0,                                   UBS_MSG_TYPE_PUSH_SL, UBA_GUI_MENU_INBOX_PUSH_SL_ID,    UBA_IMG_ID_PUSH,        UBS_MSG_KEY_SUBJECT , UBS_MSG_KEY_FROM    },
#endif
  /* PLACE mappings above this line*/
  {UBA_END_KVC_MASK,                    UBA_END_MSG_TYPE,     0,                                UBA_IMG_ID_EMPTY,       0,                    0                   }
};

