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

#ifndef _MAPCKG_H_
#define _MAPCKG_H_


#ifndef _we_def_h
#include "We_Def.h"
#endif

#ifndef _we_hdr_h
#include "We_Hdr.h"
#endif

#ifndef _we_pck_h
#include "We_Pck.h"
#endif


/**********************************************************************
 * Defines 
 **********************************************************************/
/*Result codes*/

/**********************************************************************
 * Types
 **********************************************************************/

/*! \brief Internal structure for callback handling data.
 *
 */
typedef struct
{
    WE_INT32   sub_module;          /* Response receiver */
    WE_UINT32  response_action;     /* Response internal signal */
    void        *custom_data;        /* Receiver custom data */
    WE_BOOL    operation_active;    /* Has the operation been canceled? */
} msa_pckg_data_t;


typedef void        msa_pkg_handle_t;   /* This is dummy package handle type */
typedef WE_UINT8   msa_pkg_id_t;       /* Package encapsulation internal package wid */

/* Function pointer type for package handle signal */
typedef int (*msa_pckg_handle_signal_t)(
                       msa_pkg_handle_t *pkg_inst,
                       WE_UINT16        signal,
                       void             *sig_data,
                       WE_INT32        *wid);

/* Function pointer type for package instance allocation */
typedef msa_pkg_handle_t *(*msa_pckg_init_t)(WE_UINT8 mod_id);

/* Function pointer type for package get private data */
typedef void *(*msa_pckg_get_private_data_t)(
                       msa_pkg_handle_t *pkg_inst,
                       WE_INT32         wid);

/* Function pointer type for package get private data */
typedef int (*msa_pckg_set_private_data_t)(
                                           msa_pkg_handle_t *pkg_inst,
                                           WE_INT32         wid,
                                           void             *p);

/* Function pointer type for package terminate */
typedef int (*msa_pckg_terminate_t)(
                       msa_pkg_handle_t *pkg_inst);

/* Function pointer type for package delete */
typedef int (*msa_pckg_delete_t)(
                       msa_pkg_handle_t *pkg_inst,
                       WE_INT32         wid);

/* Function pointer type for package get result */
typedef int (*msa_pckg_get_result_t)(
                       msa_pkg_handle_t *pkg_inst, 
                       WE_INT32         wid,
                       we_pck_result_t *result);

/* Function pointer type for package free result */
typedef int (*msa_pckg_free_result_t)(
                       msa_pkg_handle_t *pkg_inst,
                       we_pck_result_t *result);

/*! \brief Internal structure for simplifying free.
 *
 */
typedef struct {
    we_pck_result_t       oper_result;           /* Operation result */
    msa_pkg_handle_t       *pkg_handle;           /* package handle */
    msa_pckg_free_result_t free_fnc;              /* Free function ptr */    
    WE_INT8               module;                /* Module memory ref */    
} msa_pckg_result_t;


/* Function pointer type for custom user module operation response handler */
typedef int (*msa_pckg_response_handler_t)(
                       WE_INT32            sub_module, /* Module internal fsm or similar (optional) */
                       WE_INT32            action,     /* What action to take at operation completion (optional) */
                       msa_pkg_id_t         pckg_id,    /* Package registration Id */
                       msa_pkg_handle_t     *pkg_inst,  /* Package instance handle */
                       WE_INT32            wid,         /* Package operation Id */
                       msa_pckg_result_t    *result,    /* Operation result data */
                       void                 *data);     /* Private_data, (optional) */

typedef struct msa_pckg_fnc_list_t
{
    msa_pckg_init_t             init_fnc;               /* Package instance allocation */
    msa_pckg_handle_signal_t    handle_signal_fnc;      /* Handle signal function ptr */
    msa_pckg_get_private_data_t get_private_data_fnc;   /* Get private data function ptr */
    msa_pckg_set_private_data_t set_private_data_fnc;   /* Get private data function ptr */
    msa_pckg_terminate_t        terminate_fnc;          /* Terminate handle function ptr */
    msa_pckg_delete_t           delete_fnc;             /* Delete operation function ptr */
    msa_pckg_get_result_t       get_result_fnc;         /* Get result function ptr */
    msa_pckg_free_result_t      free_result_fnc;        /* Free result function ptr */
} msa_pckg_fnc_list_t;


typedef struct msa_pckg_pkg_list_t
{
    msa_pkg_id_t                pckg_id;
    const msa_pckg_fnc_list_t  *package_fncs;               /* Package function pointers */
    
    struct msa_pckg_pkg_list_t *next;
} msa_pckg_pkg_list_t;


typedef struct msa_pckg_inst_list_t
{
    msa_pckg_pkg_list_t         *pkg_data;
    msa_pkg_handle_t            *pkg_inst;
    WE_UINT32                  active_operations;
    WE_BOOL                    terminate_requested;
    
    struct msa_pckg_inst_list_t *next;
} msa_pckg_inst_list_t;

typedef struct
{
    WE_UINT8                    mod_id;          /* Usage context, for memory allocation */
    msa_pkg_id_t                 pkg_count;       /* Counter for package WID */
    msa_pckg_pkg_list_t         *reg_pkg_list;    /* List of registered packages */
    msa_pckg_inst_list_t        *reg_inst_list;   /* List of registered package instances */
    WE_BOOL                     do_free_result;  /* Shal encapsulation auto free data ? */
    
    msa_pckg_response_handler_t  reg_handler;     /* Registered internal operation handler. */
} msa_pckg_handle_t;


/**********************************************************************
 * Global functions
 **********************************************************************/

/*!
 * \brief  Initialise the package dispatcher.
 * \return Returns a handle if success, otherwise returns NULL.
 */
msa_pckg_handle_t* msa_pckg_create(WE_UINT8 mod_id);

/*! /brief Closes down all encapsulation activities.
 *
 * /param handle Encapsulation handle.
 */
void msa_pckg_destroy(msa_pckg_handle_t *handle);

/*!
 * \brief This function encapsulates the package operation dispatching.
 *
 * This function should be called by the user module at reception
 * of an external signal.
 *
 * This functions scans through all registered package instances,
 * to ask if the received signal is to be consumed by the package
 * instance. If consumed the get_result function of the package is
 * called to see if the operation is completed.
 *
 * If an operation turns out to be completed a call is made to
 * the registered custom response handler.
 *
 * \param handle, package encapsulator handle.
 * \param signal, The external signal Id.
 * \param sigData, The signal data supplied with the external signal.
 *
 * \return The function returns wheather the signal was consumed or not.
 *
 * Returns:
 *    WE_PACKAGE_SIGNAL_NOT_HANDLED
 *      The signal was not handled by the package, the caller
 *      must process the signal by itself.
 *    WE_PACKAGE_SIGNAL_HANDLED
 *      The signal was handled by the package.
 *    WE_PACKAGE_OPERATION_COMPLETE
 *      The signal was handled by the package and the dialog is
 *      finished. In this case the wid parameter indicates which dialog
 *      that has finished.  
 */
int msa_pckg_handle_signal( 
             msa_pckg_handle_t *handle,
             WE_UINT16 signal, 
             void *sig_data
             );

/*!
 * \brief  Add definition of package functions.
 * \return Returns internal WID of the package.
 */
msa_pkg_id_t msa_pckg_reg_pckg(msa_pckg_handle_t* handle, const msa_pckg_fnc_list_t *fncs);

/*!
 * \brief  Add definition of package functions.
 * \return Returns TRUE if successful (fails if instances remains)
 */
int msa_pckg_unreg_pckg(msa_pckg_handle_t* handle, msa_pkg_id_t pckg_id);

/*!
 * \brief  Creates an instance of a package.
 * \param  pckgId, returned by msa_pckg_reg_pckg().
 * \return Returns a handle to the package.
*/
msa_pkg_handle_t *msa_pckg_create_inst(msa_pckg_handle_t* handle, msa_pkg_id_t pckg_id);

/*!
 * \brief  Terminates an instance of a package.
 *
 * Terminates the "pckgId" package, release all resources allocated by
 * this package. If there is any open dialogs, these dialog are
 * deleted. Any private data must be released before.
 * Returns TRUE  if success, otherwise FALSE.
 *
 * \param  pckgId, returned by msa_pckg_reg_pckg().
 * \param  panic, Terminate immediately (no garbage collection).
 * \return 
 */
int msa_pckg_terminate(msa_pckg_handle_t* handle, void *pck_inst, WE_BOOL panic);

/*
 * \brief Deletes/cancels an operation.
 *
 * Deletes a created operation. Any private data is not released by 
 * this function. The module call this function when the module of any
 * reason must delete the operation before the finished state has been
 * reached.
 *
 * \param Handle, package instance handle
 * \param wid, package operation wid
 * \param private_data, returns pointer to any private_data, this must be
 *        deallocated by the caller.
 *
 * \return Returns TRUE  if success, otherwise FALSE.
 */
int msa_pckg_cancel(msa_pckg_handle_t* handle, void* pck_inst, WE_INT32 wid, void **private_data);

/*!
 * \brief Sets operation response function for package encapsulator.
 *
 * The supplied response function function is called whenever an operation
 * is completed.
 * It is up to the user module to decide what actions to take on the
 * completion of the operation. The dispacher MUST handle the private_data
 * supplied in the dispatcher function call.
 *
 * \param handle, The Package encapsulator handle.
 * \param response_handler, function pointer to user module response function 
 *
 * \return Returns TRUE is successful.
 */
int msa_pckg_set_response_handler(msa_pckg_handle_t* handle, msa_pckg_response_handler_t response_handler);

/*!
 * \brief Registers an started package operation and registers response action.
 *
 * This function is called to start a package operation.
 * Whenever the operation is completed the response function set earlier
 * is called with the sub_module, action and custom data parameters passed
 * to this function as well as the operation Id.
 *
 * \param handle, The Package encapsulator handle.
 * \param pck_inst, Handle to the package instance.
 * \param oper_id, The wid of the started operation.
 * \param sub_module, Data to the response handler.
 * \param action, Data to the response handler.
 * \param custom_data, custom data pointer to the response handler.
 *
 * \return Returns TRUE is successful.
 */
int msa_pckg_reg_operation(msa_pckg_handle_t *handle,
    msa_pkg_handle_t *pck_inst, WE_UINT32 oper_id, WE_INT32 sub_module,
    WE_INT32 action, void *custom_data);

/*!
 * \brief  Free data retrived by get_result from an operation.
 */

void msa_pckg_free_result(msa_pckg_result_t *result);

/*!
 *
 */
WE_INT32 msa_pckg_get_result_int(msa_pckg_result_t *result);

/*!
 *
 */
void *msa_pckg_get_result_data(msa_pckg_result_t *result);

/*!
 *
 */
void *msa_pckg_get_result_additional_data(msa_pckg_result_t *result);

#endif /* _MAPACK_H_ */
