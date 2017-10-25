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
 * We_Nap.c
 *
 * network account package:
 * 
 *
 * Created by Philippe Burlion, 2004-01-27.
 *
 * Revision  history:
 *
 * 
 */

#include "We_Nap.h"
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Core.h"

#include "We_Cmmn.h"

#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
#else
#include "We_Neta.h"
#endif



 /**********************************************************************
 * Defines 
 **********************************************************************/
/* Package status codes */
#define WE_NAP_STATUS_UNINITIALIZED  0
#define WE_NAP_STATUS_INITIALIZED    1


/* Package operation types */
#define WE_NAP_OP_UNDEFINED   -1
#define WE_NAP_OP_GET_SETTINGS 1




/**********************************************************************
 * Typedefs
 **********************************************************************/



typedef struct we_nap_op_st {
  struct we_nap_op_st    *next;
  WE_INT32               wid;          /* operation identifier */
  int                     state;
  void                    *priv_data;
  WE_INT8                type;      /* which operation */
} we_nap_op_t;

typedef struct we_nap_handle_st {
  WE_UINT8      modid;
  WE_UINT8      status;             
  we_nap_op_t   *op;
} we_nap_handle_t;



/**********************************************************************
 * Local functions
 **********************************************************************/

static we_nap_op_t *
find_op_by_id (we_nap_handle_t *handle, WE_INT32 wid)
{
  we_nap_op_t *op = handle->op;

  while (op) {
    if (op->wid == wid)
      return op;
    op = op->next;
  }
  return NULL;
}



static we_nap_op_t *
get_new_nap_op (we_nap_handle_t *handle)
{
  static int we_nap_uniq_id = 1;

  we_nap_op_t *op = WE_MEM_ALLOCTYPE (handle->modid, we_nap_op_t);

  if (!op)
    return NULL;

  op->wid = we_nap_uniq_id++;
  op->state = 0;
  op->priv_data = NULL;
  op->type = WE_NAP_OP_UNDEFINED;

  /* insert new operation first in list */
  op->next = handle->op;
  handle->op = op;

  return op;
}

static void
free_nap_op(we_nap_handle_t *handle, we_nap_op_t *op)
{
  we_nap_op_t *temp_op = handle->op;
  we_nap_op_t *prev_op = NULL;

  /*Remove op*/

  while (temp_op != op) {
    prev_op = temp_op;
    temp_op = temp_op->next;
  }
  if (prev_op)
    prev_op->next = op->next;
  else
    handle->op = op->next;
  
  /*Free op*/


  
 
  WE_MEM_FREE(handle->modid, op);
  op = NULL;
}

#ifndef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS  /* NOTE: if NOT defined! */

static int
translate_napi_error_code(int napiError)
{
  int ret_val;

  switch (napiError) {
  case WE_NETWORK_ACCOUNT_ERROR_BAD_ID:
    {
      ret_val = WE_NAP_ERROR_BAD_ID;
    }
    break;
  case WE_NETWORK_ACCOUNT_ERROR_NO_MORE_ID:
    {
      ret_val = WE_NAP_ERROR_NO_MORE_ID;
    }
    break;
  case WE_NETWORK_ACCOUNT_ERROR_BEARER_NOT_FOUND:
    {
      ret_val = WE_NAP_ERROR_BEARER_NOT_FOUND;
    }
    break;
  default:
    {
      ret_val = WE_NAP_ERROR_INTERNAL;
    }
  }
  return ret_val;

}
#endif


/**********************************************************************
 * Global functions
 **********************************************************************/

/*
 * Initialise the network account package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t*
we_nap_init (WE_UINT8 modid)
{
  we_nap_handle_t *h = WE_MEM_ALLOCTYPE (modid, we_nap_handle_t);

  if (h) {
    h->modid = modid;
    h->op = NULL;
    h->status = WE_NAP_STATUS_UNINITIALIZED;
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
    /* Add additional initializers here */
#endif
    
  }

  return (we_pck_handle_t*)h;
}

/*
 * Runs the network account package. When the module receives a signal that is
 * defined by WE, the module must call this function because the
 * network account package might be the receiver.
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
we_nap_handle_signal (we_pck_handle_t* handle, WE_UINT16 signal, void* p,
                       WE_INT32* wid)
{
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  int dispatched = WE_PACKAGE_SIGNAL_NOT_HANDLED;

  p=p; /* removes warning */

  if (!handle /*|| !p*/ || !wid)
    return WE_PACKAGE_SIGNAL_NOT_HANDLED;

  if (!(nap_handle->op))
    return WE_PACKAGE_SIGNAL_NOT_HANDLED;

  switch (signal) {
  case WE_SIG_PIPE_NOTIFY:
    {
      we_nap_op_t* op = nap_handle->op;
      WE_UINT8 op_found = FALSE;

      while (op && !op_found) {
        if (op->type == WE_NAP_OP_GET_SETTINGS) {
          op->state = WE_NAP_OK;
          op_found = TRUE;
          *wid = op->wid;
          dispatched = WE_PACKAGE_OPERATION_COMPLETE;
        }
        else
          op = op->next;
        
      }
       
      
    }
    break;
  }

#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
  



  
#endif
  
  return dispatched;
} 

/*
 * Set private data connected to a specific operation. The private data
 * might be used by the module to indicate which sub module that
 * created the operation. This function may be called after a
 * operation function has been called and an operation wid is available.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_nap_set_private_data (we_pck_handle_t* handle, WE_INT32 wid, void* p)
{
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  we_nap_op_t * op;

  if (!handle)
    return FALSE;

  op = find_op_by_id (nap_handle, wid);
  
  if (!op)
    return FALSE;
  op->priv_data = p;
  return TRUE;
}

/*
 * Get private data connected to a specific operation. The private data
 * might be used by the module to indicate which sub module that
 * created the operation. This function may be called after
 * we_nap_handle_signal has returned WE_PACKAGE_OPERATION_COMPLETE.
 * If success returns the private data, otherwise returns NULL.
 */
void*
we_nap_get_private_data (we_pck_handle_t* handle, WE_INT32 wid)
{
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  we_nap_op_t* op;
  
  if (!handle)
    return NULL;
    
  op = find_op_by_id (nap_handle, wid);
  if (!op)
    return NULL;
  return op->priv_data;
}


/*
 * Terminates the network account package, release all resources allocated by
 * this package. If there are any unfinished operations, these operations are
 * deleted. Any private data must be released by the module itself.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_nap_terminate (we_pck_handle_t* handle)
{
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;

  if (!nap_handle)
    return FALSE;

  while (nap_handle->op){
    switch (nap_handle->op->state) {
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
      /* 
       * States that denies the package to be terminated in goes here.
       * These should result in a return FALSE;
       */
#endif      
      default : break;
    }
    /* Otherwise, delete the operation */
    free_nap_op(nap_handle, nap_handle->op);
  }

#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
  /* Free the any other members in the handle */
#endif
  
  /* Free the handle itself */
  WE_MEM_FREE(nap_handle->modid, nap_handle);

  
  return TRUE;
}

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
we_nap_delete (we_pck_handle_t* handle, WE_INT32 wid)
{
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  we_nap_op_t *op = find_op_by_id (nap_handle, wid);
  
  if (!op)
    return FALSE;

  switch (op->state) {
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
    /* 
       * States that denies the package to be terminated in goes here.
       * These should result in a return FALSE here;
       */
#endif
    default : break;
  }

  free_nap_op(nap_handle, op);
  return TRUE;
}
/**********************************************************************
 * Result
 **********************************************************************/

/*
 * Get the result of a completed operation. The caller must
 * call this function when we_nap_handle_signal has returned
 * WE_PACKAGE_OPERATION_COMPLETE for this specific operation. This
 * function also releases all allocated resources connected to the
 * specific operation, but only if the function is called when the
 * operation is finished. Any private data associated with this 
 * operation must be freed by the using module before this function
 * is called.
 *
 * Arguments:
 * handle       The handle to the struct that holds network account package information.
 * wid           The wid to the operation instance
 * result       The function initializes this parameter with current result
 *              type containing status code and optionally result
 *              data of the operation.
 *
 *              
 *
 *
 * Returns:     WE_PACKAGE_ERROR on failure
 *              WE_PACKAGE_COMPLETED if operation has finished
 *              WE_PACKAGE_BUSY if operation has not finished yet
 */

int
we_nap_get_result(we_pck_handle_t* handle, WE_INT32 wid, we_pck_result_t* result)
{
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  we_nap_op_t *op;
  int ret_val = WE_PACKAGE_ERROR;
  
  if (!handle || !result)
    return WE_PACKAGE_ERROR;
  
  result->type = 0;
  result->_u.i_val = 0;
  result->additional_data = NULL;
  
  op = find_op_by_id (nap_handle, wid);
  if (op == NULL)
    return WE_PACKAGE_ERROR;
  
  result->type = op->type;

  switch (op->type){
  
  case WE_NAP_OP_GET_SETTINGS:
    {
      if (op->state == WE_NAP_OK)
      {
      
        ret_val = WE_PACKAGE_COMPLETED;
        free_nap_op(nap_handle, op);
      }
      else 
        ret_val = WE_PACKAGE_BUSY;
    }
    break;
  case WE_NAP_OP_UNDEFINED:
    {
    }
    break;
    
  default: break;
  }
  
  return ret_val;
}

/*
 * Free the storage allocated in the get result function
 *
 * Argument:
 * handle       The handle to the struct that holds network account package information.
 * result       Result data to be freed.
 *
 * Returns:     TRUE if success, otherwise FALSE. 
 */

int
we_nap_result_free(we_pck_handle_t* handle, we_pck_result_t* result)
{
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;

  if (!nap_handle || !result)
    return FALSE;

  switch (result->type){
  case WE_NAP_OP_GET_SETTINGS:
    {
      if(result->additional_data) {
        WE_MEM_FREE(nap_handle->modid, (void*) result->additional_data);
      }

    }
    break;
  default: return FALSE;
  }

  return TRUE;
}

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
we_nap_get_settings(we_pck_handle_t* handle)
{
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
  return WE_PACKAGE_ERROR;
#else
  
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  
  /* Fundamental error checks */
  if (!nap_handle)
    return WE_PACKAGE_ERROR;
  
  if (nap_handle->status != WE_NAP_STATUS_INITIALIZED)
    nap_handle->status = WE_NAP_STATUS_INITIALIZED;

  nap_handle->op = get_new_nap_op(nap_handle);
  if (!nap_handle->op)
    return WE_PACKAGE_ERROR;

  nap_handle->op->type = WE_NAP_OP_GET_SETTINGS;
  nap_handle->op->state = WE_NAP_OK;

  return nap_handle->op->wid; 

#endif
}


/*
 *  This function should return the actual bearer used (like GSM/CSD etc.)
 *  for a specific network account.
 *
 *  Argument: 
 *  handle              The handle to the struct that holds network account 
 *                      package information.
 *  networkAccountId    The wid of the data account
 *
 *  Returns:            Returns the bearer (TPI_SOCKET_BEARER_GSM_GPRS etc.)
 *                      or negative value indicating an error.
 */

int
we_nap_get_bearer(we_pck_handle_t* handle, WE_INT32 networkAccountId)
{
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
  return WE_NAP_ERROR_NOT_IMPLEMENTED;
#else
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  int result;

  /* Fundamental error checks */
  if (!nap_handle)
    return WE_NAP_ERROR_INVALID;
  if (nap_handle->status != WE_NAP_STATUS_INITIALIZED)
    return WE_NAP_ERROR_NO_SETTINGS;


  result = WE_NETWORK_ACCOUNT_GET_BEARER(networkAccountId);

  if (result < 0) 
   result = translate_napi_error_code(result); 
    
  return result;
#endif
}


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
                 char* buf, int buflen)
{
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
  return WE_NAP_ERROR_NOT_IMPLEMENTED;
#else
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  int result;

  /* Fundamental error checks */
  if (!nap_handle || !buf || (buflen <= 0))
    return WE_NAP_ERROR_INVALID;
  if (nap_handle->status != WE_NAP_STATUS_INITIALIZED)
    return WE_NAP_ERROR_NO_SETTINGS;

  result = WE_NETWORK_ACCOUNT_GET_NAME(networkAccountId, buf, buflen);

  if (result < 0) 
   result = translate_napi_error_code(result); 
  
  return result;
#endif
}


/*
 *  This function should return a valid network account wid, which have 
 *  the specified bearer (like GSM/CSD etc.).
 *
 *  Argument:
 *  handle              The handle to the struct that holds network account 
 *                      package information.
 *  bearer              The bearer likes GSM/CSD etc.
 *
 *  Returns:            Returns a valid network account wid or a negative value 
 *                      in case of an error.
 */

int
we_nap_get_id(we_pck_handle_t* handle, int bearer)
{
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
  return WE_NAP_ERROR_NOT_IMPLEMENTED;
#else
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  int result;

  /* Fundamental error checks */
  if (!nap_handle)
    return WE_NAP_ERROR_INVALID;
  if (nap_handle->status != WE_NAP_STATUS_INITIALIZED)
    return WE_NAP_ERROR_NO_SETTINGS;

  result = WE_NETWORK_ACCOUNT_GET_ID(bearer);

  if (result < 0)
    result = translate_napi_error_code(result);

  return result;
#endif
}

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
we_nap_get_first(we_pck_handle_t* handle)
{
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
  return WE_NAP_ERROR_NOT_IMPLEMENTED;
#else
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  int result;

  /* Fundamental error checks */
  if (!nap_handle)
    return WE_NAP_ERROR_INVALID;
  if (nap_handle->status != WE_NAP_STATUS_INITIALIZED)
    return WE_NAP_ERROR_NO_SETTINGS;

  result = WE_NETWORK_ACCOUNT_GET_FIRST();

  if (result < 0)
    result = translate_napi_error_code(result);

  return result;
#endif
}


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
we_nap_get_next(we_pck_handle_t* handle, WE_INT32 networkAccountId)
{
#ifdef WE_REGISTRY_CACHE_NETWORK_ACCOUNTS
  return WE_NAP_ERROR_NOT_IMPLEMENTED;
#else
  we_nap_handle_t* nap_handle = (we_nap_handle_t*)handle;
  int result;

  /* Fundamental error checks */
  if (!nap_handle)
    return WE_NAP_ERROR_INVALID;
  if (nap_handle->status != WE_NAP_STATUS_INITIALIZED)
    return WE_NAP_ERROR_NO_SETTINGS;

  result = WE_NETWORK_ACCOUNT_GET_NEXT(networkAccountId);

  if (result < 0)
    result = translate_napi_error_code(result);

  return result;
#endif
}
