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

#ifndef _we_sas_h
#define _we_sas_h

#ifndef _we_def_h
#include "We_Def.h"
#endif

#ifndef _we_int_h
#include "We_Int.h"
#endif

#ifndef _we_pck_h
#include "We_Pck.h"
#endif


/**********************************************************************
 * Defines 
 **********************************************************************/

#define WE_SAS_RETURN_OK                0
#define WE_SAS_RETURN_CANCELLED        -1
#define WE_SAS_RETURN_NOT_AVAILABLE    -2
#define WE_SAS_RETURN_ERROR            -3

#define WE_SAS_ERROR_ACCESS            -4
#define WE_SAS_ERROR_DELAYED           -5
#define WE_SAS_ERROR_PATH              -6
#define WE_SAS_ERROR_INVALID           -7
#define WE_SAS_ERROR_SIZE              -8
#define WE_SAS_ERROR_FULL              -9
#define WE_SAS_ERROR_EOF               -10 
#define WE_SAS_ERROR_EXIST             -11
#define WE_SAS_ERROR_INTERNAL          -12 /* This should be the last error code */

/* operation types */
#define WE_SAS_TYPE                     1

/* bit flags */
#define WE_SAS_ALLOW_OVERWRITE         0x0001 /* Must be 0x0001 to allow backward compatibility*/
#define FRW_SAS_NO_EXTERNAL_MEMORY      0x0002 /*This flag is used to prevent object to be stored on external memory.*/
#define WE_SAS_DO_NOT_SHOW_DIALOG      0x0004

/* Default filename for files saved without dialogs shown */
#define WE_SAS_CFG_DEFAULT_FILENAME WE_CFG_TEMPORARY_FOLDER_PATH##"/temp.dat"

/* Number of steps in which to update the progressbar */
#define WE_SAS_CFG_PROGRESS_STEPS      100

/* If file size is unknown the progressbar will increase to this value (in bytes) and then restart */
#define WE_SAS_CFG_BAR_DEFAULT_MAX_VALUE   20000

/* Defines if the save as file browser dialog should not be shown */
/* #define WE_SAS_CFG_DO_NOT_SHOW_DIALOG */

/**********************************************************************
 * Global functions
 **********************************************************************/

/*
 * Initialise the save as package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t*
we_sas_init (WE_UINT8 modId);

/*
 * Runs the save as package. When the module receives a signal that is
 * defined by WE, the module must call this function because the
 * dialog package might be the receiver.
 *
 * Returns:
 *    WE_PACKAGE_SIGNAL_NOT_HANDLED
 *      The signal was not handled by the package, the calling module
 *      must process the signal by itself.
 *    WE_PACKAGE_SIGNAL_HANDLED
 *      The signal was handled by the package.
 *    WE_PACKAGE_OPERATION_COMPLETE
 *      The signal was handled by the package and the save as operation is 
 *      finished. In this case the wid parameter indicates which operation that 
 *      is finished. 
 */
int
we_sas_handle_signal (we_pck_handle_t* handle, WE_UINT16 signal, void* p,
                       WE_INT32* wid);

/*
 * Set private data connected to a specific save as operation. The private 
 * data might be used by the module to indicate which sub module that started 
 * the save as operation. This function may be called after a
 * we_sas_create_xxx function has been called.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_sas_set_private_data (we_pck_handle_t* handle, WE_INT32 wid, void* p);

/*
 * Get private data connected to a specific save as operation. The private 
 * data might be used by the module to indicate which sub module that has
 * started the operation. This function may be called after 
 * we_sas_handle_signal returned WE_PACKAGE_OPERATION_COMPLETE.
 * If success returns the private data, otherwise returns WE_PACKAGE_ERROR.
 */
void*
we_sas_get_private_data (we_pck_handle_t* handle, WE_INT32 wid);


/*
 * Terminates the save as package, release all resources allocated by
 * this package. If there is any open dialogs, these dialog are
 * deleted. Any private data must be released by the module itself.
 * Returns TRUE  if success, otherwise FALSE.
 */
int
we_sas_terminate (we_pck_handle_t* handle);

/**********************************************************************
 * Delete 
 **********************************************************************/

/*
 * Deletes a created save as operation. Any private data is not released by 
 * this function. The module call this function when the module of any
 * reason must delete the operation before the finished state has been
 * reached.
 * Returns TRUE  if success, otherwise FALSE.
 */
int
we_sas_delete (we_pck_handle_t* handle, WE_INT32 wid);

/**********************************************************************
 * Result
 **********************************************************************/

/*
 * Get the result of a completed operation. The caller must
 * call this function when we_dlg_handle_signal has returned
 * WE_PACKAGE_OPERATION_COMPLETE for this specific dialog. This
 * function also releases all allocated resources connected to the
 * specific dialog , but only if the function is called when the
 * operation is finished. Any private data associated with this 
 * operation must be freed by the using module before this function
 * is called.
 * Returns TRUE if success, otherwise FALSE.
 */

int
we_sas_get_result(we_pck_handle_t* handle, WE_INT32 wid, we_pck_result_t* result);

int
we_sas_get_result_ext(we_pck_handle_t* handle, WE_INT32 wid, we_pck_result_t* result);

/*
 * Free the storage allocated in the get result function
 *
 * Argument:
 * result       Result data to be freed.
 *
 * Returns:     TRUE if success, otherwise FALSE. 
 */

int
we_sas_result_free(we_pck_handle_t* handle, we_pck_result_t* result);


/**********************************************************************
 * Save as Operation
 **********************************************************************/

/*
 * Create a save as operation.
 *
 * Save as parameters:
 *  screen                The screen that should be used for the dialog.
 *  title                 Tile of the dialog. If 0 a default string will be used.
 *  file_name             Suggested user friendly name of the file.
 *  mime_type             The mime type of file to save.
 *  save_here_str         "save here" string. If 0 a default string will be used.
 *  data_type             Tells if the data to save comes from a pipe, file or
 *                        buffer. value must be one of the following constants:
 *                          WE_PCK_DATA_TYPE_FILE,
 *                          WE_PCK_DATA_TYPE_PIPE,
 *                          WE_PCK_DATA_TYPE_BUFFER
 *  data                  The data that shall be written to file. This 
 *                        parameter is only used when data comes from a 
 *                        buffer. Note: This data must be available until the
 *                        operation is complete.      
 *  data_len              The size of the data. This parameter is only used 
 *                        when data comes from a buffer.
 *  pathname              The name of the pipe or file. This parameter is only
 *                        used when data comes from a pipe or a file.
 *  file_attrs            the file attributes that should be stored with the
 *                        specified file.
 *  bit_flag              WE_SAS_ALLOW_OVERWRITE   -  if bit is set, the package will prompt
 *                          the user about the replace and possibly replace the existing file.
 *                        WE_SAS_DO_NOT_SHOW_DIALOG   -  if bit is set, the package will not 
 *                          allow the user to select position or name of the file to save, but
 *                          the file will be saved to a temporary default position with a default
 *                          name.
 *                        FRW_SAS_NO_EXTERNAL_MEMORY - If this bit is set no folders on
 *                          external memory will be possible to select as save destination.
 * 
 * Returns positive number denoting an wid if ok and WE_PACKAGE_ERROR otherwise.
 */
typedef struct we_sas_save_as_st {
  we_pck_handle_t          *handle;
  WeScreenHandle            screen;
  WeStringHandle            title;
  const char                *file_name;
  const char                *mime_type;
  WeStringHandle            save_here_str;
  int                        data_type;
  void                      *data;
  long                       data_len;
  const char                *pathname;
  const we_pck_attr_list_t *file_attrs;
  WE_INT32                  bit_flag;
} we_sas_save_as_t;


typedef struct {
  int   result;              /* WE_SAS_RETURN_OK if success, 
                                WE_SAS_RETURN_CANCELLED if operation 
                                    was canceled by the user 
                                WE_SAS_RETURN_NOT_AVAILABLE if result
                                    function is called when operation is 
                                    not finished
                                WE_SAS_RETURN_ERROR if the operation was 
                                    aborted due to an unexpected error, 
                                    e.g. file error*/
  char  *file_name;          /* If result was WE_SAS_RETURN_OK, there is a 
                                file name in this parameter, otherwise NULL.*/
} we_sas_result_save_as_t;


WE_INT32
we_sas_create_save_as (const we_sas_save_as_t* save_as_data);

/*
 * The get result function of a finished save as operation returnes the following values
 * The module must call this function when we_sas_handle_signal has returned 
 * WE_PACKAGE_OPERATION_COMPLETE for this specific operation. This function also releases 
 * all allocated  resources connected to the specific dialog, but only if the function 
 * is called when the operation is finished.  * Returns TRUE if success, otherwise FALSE.
 *
 * Parameter cast type: we_sas_result_save_as_t
 *
 * Parameters:
 * 
 * result                       WE_SAS_RETURN_OK if success, 
 *                              WE_SAS_RETURN_CANCELLED if operation 
 *                                  was canceled by the user 
 *                              WE_SAS_RETURN_NOT_AVAILABLE if result
 *                                  function is called when operation is 
 *                                  not finished
 *                                  aborted due to an unexpected error, 
 *                                  e.g. file error
 * char  *file_name;            If result was WE_SAS_RETURN_OK, there is a 
 *                              file name in this parameter, otherwise NULL. 
 *
 */



#endif
