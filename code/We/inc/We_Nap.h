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

#ifndef _we_nap_h
#define _we_nap_h

#ifndef _we_def_h
#include "We_Def.h"
#endif

#ifndef _we_pck_h
#include "We_Pck.h"
#endif




 /**********************************************************************
 * Defines 
 **********************************************************************/
/*Return codes*/
#define WE_NAP_OK                      0
#define WE_NAP_BUSY                    -1
#define WE_NAP_ERROR_NO_SETTINGS       -2
#define WE_NAP_ERROR_INVALID           -3
#define WE_NAP_ERROR_BAD_ID            -4
#define WE_NAP_ERROR_NO_MORE_ID        -5
#define WE_NAP_ERROR_BEARER_NOT_FOUND  -6

#define WE_NAP_ERROR_NOT_IMPLEMENTED   -7  /* Temporary use until registry 
                                             * cache is implemented 
                                             */
#define WE_NAP_ERROR_INTERNAL          -8  /*This should be the last error code*/




 /**********************************************************************
 * Types
 **********************************************************************/


/**********************************************************************
 * Global functions
 **********************************************************************/

/*
 * Initialises the action package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t*
we_nap_init (WE_UINT8 modId);

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
we_nap_handle_signal (we_pck_handle_t* handle, WE_UINT16 signal,
                       void* p, WE_INT32* wid);

/*
 * Set private data connected to a specific operation. The private data
 * might be used by the module to indicate which sub module that
 * created the operation. This function may be called after a
 * operation function has been called and an operation wid is available.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_nap_set_private_data (we_pck_handle_t* handle,
                          WE_INT32 wid, void* p);

/*
 * Get private data connected to a specific operation. The private data
 * might be used by the module to indicate which sub module that
 * created the operation. This function may be called after
 * we_act_handle_signal has returned WE_PACKAGE_OPERATION_COMPLETE.
 * If success returns the private data, otherwise returns NULL.
 */
void*
we_nap_get_private_data (we_pck_handle_t* handle, WE_INT32 wid);

/*
 * Terminates the action package, release all resources allocated by
 * this package. If there are any unfinished operations, these operations are
 * deleted. Any private data must be released by the module itself.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_nap_terminate (we_pck_handle_t* handle);

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
we_nap_delete (we_pck_handle_t* handle, WE_INT32 wid);

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
 *              
 *
 * Returns:     WE_PACKAGE_ERROR on failure
 *              WE_PACKAGE_COMPLETED if operation has finished
 *              WE_PACKAGE_BUSY if operation has not finished yet
 */
int
we_nap_get_result(we_pck_handle_t* handle, WE_INT32 wid, we_pck_result_t* result);

/*
 * Free the storage allocated in the get result function
 *
 * Argument:
 * result       Result data to be freed.
 *
 * Returns:     TRUE if success, otherwise FALSE. 
 */

int
we_nap_result_free(we_pck_handle_t* handle, we_pck_result_t* result);


/**********************************************************************
 * Package specific function declarations
 **********************************************************************/

/*
 *  This function gets the current network account settings from the 
 *  registry in the registry cache when the package is configured to be 
 *  used in registry cache mode. In HDIa wrapper mode it will simply create
 *  an operation.
 *
 *  Argument: 
 *  handle              The handle to the struct that holds network account 
 *                      package information.
 *
 *  Returns:            WE_PACKAGE_ERROR on failure
 *                      op WID if successful
 */

int
we_nap_get_settings(we_pck_handle_t* handle);


/*
 *  This function should return the actual bearer used (like GSM/CSD etc.)
 *  for a specific network account.
 *
 *  Argument: 
 *  networkAccountId    The wid of the data account
 *
 *  Returns:            Returns the bearer (TPI_SOCKET_BEARER_GSM_GPRS etc.)
 *                      or negative value indicating an error.
 */

int
we_nap_get_bearer(we_pck_handle_t* handle, WE_INT32 networkAccountId);

/*
 *  This function should return any human readable identification of the 
 *  network account, if there is one present (for example the ISP or operator 
 *  name like "XXX Mobile 3G Access", or name assigned to the account by the 
 *  user like "My GPRS connection")
 *
 *  Argument:
 *  handle              The handle to the struct that holds network account 
 *                      package information.
 *  networkAccountId    The wid of the data account
 *  buf                 The buffer to put the response data in 
 *                      (null terminated string)
 *  buflen              The length of the response data buffer
 *
 *  Returns:            Returns a negative value in case of an error
 */

int
we_nap_get_name(we_pck_handle_t* handle, WE_INT32 networkAccountId,
                 char* buf, int buflen);


/*
 *  This function should return a valid network account wid, which have 
 *  the specified bearer (like GSM/CSD etc.).
 *
 *  Argument:
 *  handle              The handle to the struct that holds network account 
 *                      package information.
 *  bearer              The bearer likes GSM/CSD etc.
 *
 *  Returns:            Returns a valid network account wid or negative value 
 *                      in case of an error.
 */

int
we_nap_get_id(we_pck_handle_t* handle, int bearer);


/*
 *  Retrieves the network account WID for first network account present 
 *  in the device
 *
 *  Argument:
 *  handle              The handle to the struct that holds network account 
 *                      package information.
 *
 *  Returns:            Returns a the first network account wid or a negative value 
 *                      in case of an error.
 */

int
we_nap_get_first(we_pck_handle_t* handle);


/*
 *  Retrieves the network account WID for the next account after networkAccountId .
 *
 *  Argument:
 *  handle              The handle to the struct that holds network account 
 *                      package information.
 *  networkAccount      The wid of the current network account
 *
 *  Returns:            The WID of the next network account or negative value 
 *                      indicating an error
 */

int
we_nap_get_next(we_pck_handle_t* handle, WE_INT32 networkAccountId);

#endif
