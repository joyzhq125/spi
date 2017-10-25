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

#ifndef _we_act_h
#define _we_act_h

#ifndef _we_def_h
#include "We_Def.h"
#endif

#ifndef _we_pck_h
#include "We_Pck.h"
#endif

#ifndef _we_int_h
#include "We_Int.h"
#endif

#ifndef _we_core_h
#include "We_Core.h"
#endif


 /**********************************************************************
 * Defines 
 **********************************************************************/
/*Return codes*/
#define WE_ACT_OK               0
#define WE_ACT_CANCELLED       -1
#define WE_ACT_ERROR_ACCESS    -2
#define WE_ACT_ERROR_DELAYED   -3
#define WE_ACT_ERROR_PATH      -4
#define WE_ACT_ERROR_INVALID   -5

#define WE_ACT_ERROR_INTERNAL  -6  /*This should be the last error code*/

/* operation types */
#define WE_ACT_GET_ACTIONS      1
#define WE_ACT_OBJECT_ACTION    2

/* Defines the Action command used for internal packets */
#define WE_PACKET_OPERATION_ID_SAS      "save"

/* Blocked permissions */
#define WE_PERMISSION_NONE_BLOCKED      0x0000
#define WE_PERMISSION_FORWARD_BLOCKED   0x0002


 /**********************************************************************
 * Types
 **********************************************************************/
typedef struct {
  WE_INT32             action_id;
  WE_INT32             string_id;
  char*                 act_cmd;
} we_act_action_entry_t;

typedef struct {
  WE_INT32               result;
  WE_INT32               num_actions;
  we_act_action_entry_t  action_list[1];  /* List of actions. */
} we_act_result_get_actions_t;

typedef struct {
  WE_BOOL caller_owns_pipe; /* caller has pipe responsibility */
} we_act_error_result_object_action_t;

/**********************************************************************
 * Global functions
 **********************************************************************/

/*
 * Initialises the action package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t*
we_act_init (WE_UINT8 modId);

/*
 * Runs the action package. When the module receives a signal that is
 * defined by WE, the module must call this function because the
 * action package might be the receiver.
 *
 * Returns:
 *    WE_PACKAGE_SIGNAL_NOT_HANDLED
 *      The signal was not handled by the package, the caller
 *      must process the signal by itself.
 *    WE_PACKAGE_SIGNAL_HANDLED
 *      The signal was handled by the package.
 *    WE_PACKAGE_OPERATION_COMPLETE
 *      The signal was handled by the package and the operation is
 *      finished. In this case the wid parameter indicates which operation
 *      that has finished. 
 */
int
we_act_handle_signal (we_pck_handle_t* handle, WE_UINT16 signal,
                       void* p, WE_INT32* wid);

/*
 * Set private data connected to a specific operation. The private data
 * might be used by the module to indicate which sub module that
 * created the operation. This function may be called after a
 * operation function has been called and an operation wid is available.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_act_set_private_data (we_pck_handle_t* handle,
                          WE_INT32 wid, void* p);

/*
 * Get private data connected to a specific operation. The private data
 * might be used by the module to indicate which sub module that
 * created the operation. This function may be called after
 * we_act_handle_signal has returned WE_PACKAGE_OPERATION_COMPLETE.
 * If success returns the private data, otherwise returns NULL.
 */
void*
we_act_get_private_data (we_pck_handle_t* handle, WE_INT32 wid);

/*
 * Terminates the action package, release all resources allocated by
 * this package. If there are any unfinished operations, these operations are
 * deleted. Any private data must be released by the module itself.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_act_terminate (we_pck_handle_t* handle);

/**********************************************************************
 * Delete 
 **********************************************************************/

/*
 * Deletes an unfinished operation. Any private data is not released by this
 * function. The module call this function when the module of any
 * reason must delete the operation before the finished state has been
 * reached.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_act_delete (we_pck_handle_t* handle, WE_INT32 wid);

/**********************************************************************
 * Result
 **********************************************************************/

/*
 * Get the result of a completed operation. The caller must
 * call this function when we_act_handle_signal has returned
 * WE_PACKAGE_OPERATION_COMPLETE for this specific operation. This
 * function also releases all allocated resources connected to the
 * specific operation , but only if the function is called when the
 * operation is finished. Any private data associated with this 
 * operation must be freed by the using module before this function
 * is called.
 *
 * Arguments:
 * handle       The handle to the struct that holds action package information.
 * wid           The wid to the operation instance
 * result       The function initializes this parameter with current result
 *              type containing status code and optionally result
 *              data of the operation.
 *
 *              function                  result has a data that should be casted to:
 *              we_act_get_actions       we_act_result_get_actions_t *
 *              we_act_object_action     WE_INT32 (WE_ACT_ return codes)
 *
 *              function                  result has a additional_data that should be casted to:
 *              we_act_object_action     we_act_error_result_object_action_t in case of 
 *                                        WE_PACKAGE_ERROR or WE_ACT_CANCELLED
 *
 * Returns:     WE_PACKAGE_ERROR on failure
 *              WE_PACKAGE_COMPLETED if operation has finished
 *              WE_PACKAGE_BUSY if operation has not finished yet
 */
int
we_act_get_result(we_pck_handle_t* handle, WE_INT32 wid, we_pck_result_t* result);

/*
 * Free the storage allocated in the get result function
 *
 * Argument:
 * result       Result data to be freed.
 *
 * Returns:     TRUE if success, otherwise FALSE. 
 */

int
we_act_result_free(we_pck_handle_t* handle, we_pck_result_t* result);



/**********************************************************************
 * Get actions
 **********************************************************************/

/* This operation is used to recieve the actions available for a specific
 * mime type.
 *
 * If a non-NULL content_type is passed in, then capability checking
 * might be applied besides checking the mime_type. That is, only
 * those actions with an associated mime_type and with some capability
 * will be returned by this function.
 *
 * The content_type is typically the DRM type or for example some envelope
 * type.
 * 
 * Presently capability checking for content type is applied if the
 * content_type is non-NULL and is equal to either the DRM-types
 * "application/vnd.oma.drm.content" or "application/vnd.oma.drm.message".
 * For these content_types, it is checked whether the action has been configured
 * with DRM capability (see WE_ACTION_CAPABILITY_DRM in We_Cfg.h).
 *
 * The parameter data_type can also be used to check capabilities. However,
 * presenlty there are no capability-checking that is triggered by the data_type.
 *
 * handle                    The handle to the struct that holds all action package 
 *                           information.
 * mime_type                 The suitable actions for this mime type will be recieved
 * data_type                 Tells if the content is in a file, pipe or buffer.
 * content_type              Set to NULL if content type is same as mime_type.
 *                           Set also to NULL to turn of capability checking for
 *                           content type. Otherwise, set to DRM type or other 
 *                           envelope type.
 * check_blocked_permission  DRM permission of the file. Specifïes the
 *                           permission(s) to be checked wether blocked  
 *                           or not (requested permission).
 * excl_act_strings          An array of string pointers. Each string
 *                           (or command string) identifies an action
 *                           or a sub-action. All actions.and sub-actions
 *                           in this array will be excluded from the result.
 *                           That is, they are excluded when searching for
 *                           actions for the given mime type. NULL may be
 *                           passed. 
 * excl_act_strings_cnt      The number of strings in excl_act_strings.
 *
 * Returns:      An wid to this get action operation or WE_PACKAGE_ERROR
 *   
 */

WE_INT32
we_act_get_actions(we_pck_handle_t  *handle,
                    const char        *mime_type,
                    WeResourceType    data_type,
                    const char*        content_type,
                    WE_INT32          check_blocked_permission,
                    const char*        excl_act_strings[],
                    WE_INT32          excl_act_strings_cnt);

/**********************************************************************
 * Object action
 **********************************************************************/

/* This operation performs the action specified by the action_id. If the action
 * needs more specified information from the end user, these dialogs are handled by
 * the operation.
 *
 * In case of succcessful result, the responsibility of any pipe content passed in,
 * is passed on to the action. In case of cancel or error, a pipe content might have 
 * been deleted. Whether a pipe was deleted or not during cancel or error, is indicated 
 * in the we_act_error_result_object_action_t returned in we_act_get_result. In case 
 * of cancel or failure and the pipe was not deleted, the responsibilty remains by the
 * caller.
 *
 * Error handling
 * If object action returns WE_ACT_OK, the caller does not have to do any error handling.
 * However when WE_ACT_OK is returned for external and internal non-package actions, 
 * the receiver of the action may detect some error and in these cases it is the receiver 
 * that shall do error handling, if any.
 *
 * If object action returns an error, the act package will have made common error handling
 * that is applicable. Presently this is presenting an error dialog to the user.
 * Further, the only common error handling that presently might occur is during internal 
 * package actions (e.g. "save as" was cancelled or "save as" failed of other reasons).
 *
 * When object action returns an error, it is up to the caller to do other caller specific
 * error handling besides what has been done in the package.
 *
 * 
 * handle                    The handle to the struct that holds all action package 
 *                           information.
 * action_id                 The identifier to the action to perform
 * content                   The content data to perform an action on. All members 
 *                           except the data memeber is copied by the act package 
 *                           when starting the operation.
 *                           Note: If the content is of buffer type, the data member  
 *                           must be available until the operation is complete.
 *                           All members are passed on to the receiver of the object
 *                           action.
 * file_attrs                The file attributes of the file to perform the action on.
 * check_blocked_permission  DRM permission of the file. Specifïes the
 *                           permission(s) to be checked whether blocked  
 *                           or not (requested permission).
 * screen                    The screen that should be used for dialogs.
 * excl_act_strings          An array of string pointers. Each string
 *                           (or command string) identifies an action
 *                           or a sub-action. All sub-actions in this
 *                           array will be excluded in any dialogs to 
 *                           the end user. The same excl_act_strings
 *                           parameter as in we_act_get_actions should
 *                           be passed here.¨NULL may be passed.
 * excl_act_strings_cnt      The number of strings in excl_act_strings.
 *
 * Returns:     An wid to this object action operation or WE_PACKAGE_ERROR
 *   
 */
WE_INT32
we_act_object_action(we_pck_handle_t          *handle,
                      WE_INT32                  action_id,
                      const we_act_content_t   *content,
                      const we_pck_attr_list_t *file_attrs,
                      WE_INT32                  check_blocked_permission,
                      WeScreenHandle            screen,
                      const char*                excl_act_strings[],
                      WE_INT32                  excl_act_strings_cnt);



#endif
