/*
 * Copyright (C) Techfaith, 2002-2005.
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

/*--- Include files ---*/
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Cfg.h"    /* WE: Module definition */
#include "We_Def.h"    /* WE: Global definitions */ 
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Mem.h"    /* WE: Memory handling */
#include "We_Pck.h"    /* WE: WE File API */ 
#include "Sapckg.h"

/*!
 * \brief  Initialise the package dispatcher.
 * \return Returns a handle if success, otherwise returns NULL.
 */
Sia_pckg_handle_t* Sia_pckg_create(WE_UINT8 mod_id)
{
    Sia_pckg_handle_t *pckg = 0;

    pckg = (Sia_pckg_handle_t*) WE_MEM_ALLOC(mod_id, sizeof(Sia_pckg_handle_t));
    if (pckg == NULL)
    {
        return NULL;
    }

    pckg->mod_id = mod_id;
    pckg->reg_pkg_list = NULL;
    pckg->reg_inst_list = NULL;
    pckg->reg_handler = NULL;
    pckg->pkg_count = 0;
    pckg->do_free_result = TRUE;
    return pckg;
}

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
 *      finished. In this case the id parameter indicates which dialog
 *      that has finished. 
 */

int Sia_pckg_handle_signal( 
                       Sia_pckg_handle_t *handle,
                       WE_UINT16 signal, 
                       void *sig_data)
{
    int                   result = WE_PACKAGE_SIGNAL_NOT_HANDLED;
    Sia_pckg_inst_list_t *list = 0;
    Sia_pckg_inst_list_t *inst = 0;
    Sia_pckg_data_t      *private_data;
    WE_INT32             oper_id = 0;
    we_pck_result_t      oper_result;
    Sia_pckg_result_t    *int_result = NULL;

    if (handle == NULL)
    {
        return result;
    }

    list = handle->reg_inst_list;

    while (result == WE_PACKAGE_SIGNAL_NOT_HANDLED && list != NULL)
    {
        result = list->pkg_data->package_fncs->handle_signal_fnc(list->pkg_inst, signal, sig_data, &oper_id);
        if (result == WE_PACKAGE_OPERATION_COMPLETE)
        {
            /*
             *	Increment operation counter.
             */
            --list->active_operations;

            /*
             *	Get private data from operation:
             */
            private_data = list->pkg_data->package_fncs->get_private_data_fnc(list->pkg_inst, oper_id);

            /*
             *	Get result data from operation:
             */
            list->pkg_data->package_fncs->get_result_fnc(list->pkg_inst, oper_id, &oper_result);


            if (TRUE == list->terminate_requested)
            {                
                /*
                 *	Free encapsulation data
                 */
                WE_MEM_FREE(handle->mod_id, private_data);

                /*
                 *	Free package data and terminate
                 */
                list->pkg_data->package_fncs->free_result_fnc(list->pkg_inst, &oper_result);

                if (list->active_operations == 0)
                {

                    (void)Sia_pckg_terminate(handle, list->pkg_inst, FALSE);
                }
                
                return WE_PACKAGE_OPERATION_COMPLETE;
            }

            if (FALSE == private_data->operation_active)
            {                            
                /*
                 *	Free encapsulation data
                 */
                WE_MEM_FREE(handle->mod_id, private_data);
                list->pkg_data->package_fncs->free_result_fnc(list->pkg_inst, &oper_result);

                return WE_PACKAGE_OPERATION_COMPLETE;
            }

            int_result = WE_MEM_ALLOC(handle->mod_id, sizeof(Sia_pckg_result_t));
            if (NULL == int_result)
            {
                list->pkg_data->package_fncs->free_result_fnc(list->pkg_inst, &oper_result);
                
                return FALSE;
            }
            
            int_result->pkg_handle = list->pkg_inst;
            int_result->free_fnc = list->pkg_data->package_fncs->free_result_fnc;
            int_result->module = handle->mod_id;
            int_result->oper_result._u.data = oper_result._u.data;
            int_result->oper_result.type = oper_result.type;
            int_result->oper_result.additional_data = oper_result.additional_data;
            
            /*
             *	Call response handler with operation data:
             */
            ++list->active_operations;
            (*handle->reg_handler)(
                private_data->sub_module,
                private_data->response_action,
                list->pkg_data->pckg_id,
                list->pkg_inst,
                oper_id,
                int_result,
                private_data->custom_data);
            --list->active_operations;
            
            /*
             *	Free encapsulation data
             */
            WE_MEM_FREE(handle->mod_id, private_data);

            /*
             *	Free result data from operation:
             */
            if (TRUE == handle->do_free_result)
            {
                Sia_pckg_free_result(int_result);
            }

            return result;
        }
        else
        {
            inst = list;
            list = list->next;
            
            if (TRUE == inst->terminate_requested &&
                inst->active_operations == 0)
            {                
                (void)Sia_pckg_terminate(handle, inst->pkg_inst, FALSE);
            }
        }        
    }

    return result;
} /* Sia_pckg_handle_signal() */


WE_INT32 Sia_pckg_get_result_int(Sia_pckg_result_t *result)
{
    return result->oper_result._u.i_val;
}

void *Sia_pckg_get_result_data(Sia_pckg_result_t *result)
{
    return result->oper_result._u.data;
}

void *Sia_pckg_get_result_additional_data(Sia_pckg_result_t *result)
{
    return result->oper_result.additional_data;
}

/*!
 * \brief  Add definition of package functions.
 * \return Returns internal ID of the package.
 */
Sia_pkg_id_t Sia_pckg_reg_pckg(Sia_pckg_handle_t* handle, const Sia_pckg_fnc_list_t *fncs)
{
    Sia_pckg_pkg_list_t *list = 0;

    if (handle == NULL || fncs == NULL)
    {
        return 0;
    }

    /*
     *	Allocate and initiate into list:
     */
    list = (Sia_pckg_pkg_list_t*) WE_MEM_ALLOC(handle->mod_id, sizeof(Sia_pckg_pkg_list_t));
    if (list == NULL)
    {
        return 0;
    }

    list->next = handle->reg_pkg_list;
    handle->reg_pkg_list = list;
    handle->pkg_count++;
    list->pckg_id = handle->pkg_count;
    list->package_fncs = fncs;

    return list->pckg_id;
} /* Sia_pckg_reg_pckg */

/*!
 * \brief  Add definition of package functions.
 * \return Returns TRUE if successful (fails if instances remains)
 */
int Sia_pckg_unreg_pckg(Sia_pckg_handle_t* handle, Sia_pkg_id_t pckg_id)
{
    Sia_pckg_pkg_list_t *list = 0;
    Sia_pckg_pkg_list_t *prev = NULL;
    Sia_pckg_inst_list_t *inst = NULL;
    
    if (handle == NULL)
    {
        return FALSE;
    }


    list = handle->reg_pkg_list;
    while (list != NULL && list->pckg_id != pckg_id)
    {
        prev = list;
        list = list->next;
    }

    if (list == NULL)
    {
        /*
         *	Package not found.
         */
        return FALSE;
    }

    /*
     *	Verify that no instance is active:
     */
    inst = handle->reg_inst_list;
    while (inst != NULL && inst->pkg_data != list)
    {
        inst = inst->next;
    }
    if (inst != NULL)
    {
        /*
         *	Package instanse still active:
         */
        return FALSE;
    }

    /*
     *	Unlink and free:
     */
    if (prev == NULL)
    {
        handle->reg_pkg_list = list->next;
    }
    else
    {
        prev->next = list->next;
    }
    list->next = NULL;
    list->package_fncs = NULL;
    WE_MEM_FREE(handle->mod_id, list);

    return TRUE;
} /* Sia_pckg_unreg_pckg() */

/*!
 * \brief  Creates an instance of a package.
 * \param  pckgId, returned by Sia_pckg_reg_pckg().
 * \return Returns a handle to the package.
 */
Sia_pkg_handle_t *Sia_pckg_create_inst(Sia_pckg_handle_t* handle, Sia_pkg_id_t pckg_id)
{
    Sia_pckg_pkg_list_t   *list = 0;
    Sia_pckg_inst_list_t  *inst = NULL;
    Sia_pkg_handle_t      *p = NULL;
    
    
    if (handle == NULL)
    {
        return NULL;
    }

    /*
     *	Locate package:
     */
    list = handle->reg_pkg_list;
    while (list != NULL && list->pckg_id != pckg_id)
    {
        list = list->next;
    }

    if (list == NULL)
    {
        /*
         *	Package not found.
         */
        return NULL;
    }

    /*
     *	Create new package instance:
     */
    p = list->package_fncs->init_fnc(handle->mod_id);
    if (p == NULL)
    {
        return NULL;
    }

    /*
     *	Allocate and link:
     */
    inst = (Sia_pckg_inst_list_t*) WE_MEM_ALLOC(handle->mod_id, sizeof(Sia_pckg_inst_list_t));
    if (inst == NULL)
    {
        /*
         *	Memory allocation failure:
         */
        return NULL;
    }

    inst->pkg_data = list;
    inst->pkg_inst = p;
    inst->next = handle->reg_inst_list;
    inst->terminate_requested = FALSE;
    inst->active_operations = 0;
    
    handle->reg_inst_list = inst;

    return p;
} /* Sia_pckg_create_inst() */

/*!
 * \brief  Terminates an instance of a package.
 *
 * Terminates the "pckgId" package, release all resources allocated by
 * this package. If there is any open dialogs, these dialog are
 * deleted. Any private data must be released before.
 * Returns TRUE  if success, otherwise FALSE.
 *
 * \param  pckgId, returned by Sia_pckg_reg_pckg().
 * \return 
 */
int Sia_pckg_terminate(Sia_pckg_handle_t* handle, void *pck_inst, WE_BOOL panic)
{
    Sia_pckg_inst_list_t  *inst = NULL;
    Sia_pckg_inst_list_t  *prev = NULL;
    
    if (handle == NULL)
    {
        return FALSE;
    }

    /*
     *	Locate package instance:
     */
    inst = handle->reg_inst_list;
    while (inst != NULL && inst->pkg_inst != pck_inst)
    {
        prev = inst;
        inst = inst->next;
    }

    if (inst == NULL)
    {
        /*
         *	Instance not found:
         */
        return FALSE;
    }

    inst->terminate_requested = TRUE;
    
    if (FALSE == panic && inst->active_operations > 0)
    {
        return TRUE;
    }
    
    /*
     *	Terminate the package instance:
     */
    if (WE_PACKAGE_COMPLETED != inst->pkg_data->package_fncs->terminate_fnc(inst->pkg_inst))
    {
        /*
         * ToDo: When packages behave, this will return.
         */
        /* return FALSE; */
    }
    
    /*
     *	Unlink and free:
     */
    if (prev == NULL)
    {
        handle->reg_inst_list = inst->next;
    }
    else
    {
        prev->next = inst->next;
    }
    inst->next = NULL;
    inst->pkg_data = NULL;
    inst->pkg_inst = NULL;
    WE_MEM_FREE(handle->mod_id, inst);

    return TRUE;
} /* Sia_pckg_terminate() */

/*
 * \brief Deletes/cancels an operation.
 *
 * Deletes a created operation. Any private data is not released by 
 * this function. The module call this function when the module of any
 * reason must delete the operation before the finished state has been
 * reached.
 *
 * \param Handle, package instance handle
 * \param id, package operation id
 * \param private_data, returns pointer to any private_data, this must be
 *        deallocated by the caller.
 *
 * \return Returns TRUE  if success, otherwise FALSE.
 */
int Sia_pckg_cancel(Sia_pckg_handle_t* handle, void* pkg_inst, WE_INT32 id, void **private_data)
{
    Sia_pckg_inst_list_t  *inst = NULL;
    Sia_pckg_inst_list_t  *prev = NULL;
    Sia_pckg_data_t       *int_data;
 
    (*private_data) = NULL; 

    if (handle == NULL)
    {
        return FALSE;
    }

    /*
     *	Locate package instance:
     */
    inst = handle->reg_inst_list;
    while (inst != NULL && inst->pkg_inst != pkg_inst)
    {
        prev = inst;
        inst = inst->next;
    }

    if (inst == NULL)
    {
        /*
         *	Instance not found:
         */
        return FALSE;
    }

    /*
     *	First get any private data connected to the operation:
     */
    int_data = inst->pkg_data->package_fncs->get_private_data_fnc(inst->pkg_inst, id);
    if (FALSE == int_data)
    {
        return FALSE;
    }

    /*
     *	Return private data to free:
     */
    (*private_data) = int_data->custom_data;
    int_data->custom_data = NULL;
    int_data->operation_active = FALSE;
    
    /*
     *	Deletes the package operation:
     */
    if (NULL != inst->pkg_data->package_fncs->delete_fnc)
    {
        if (FALSE == inst->pkg_data->package_fncs->delete_fnc(inst->pkg_inst, id))
        {
            return TRUE;
        }

        /*
         *	Free insternal encapsulation data:
         */
        WE_MEM_FREE(handle->mod_id, int_data);
    }

    return TRUE;
} /* Sia_pckg_delete() */

/*!
 * \brief Sets dispatcher function for package handler.
 *
 * The supplied dispatcher function is called whenever an operation
 * is completed.
 * It is up to the user module to decide what actions to take on the
 * completion of the operation. The dispacher MUST handle the private_data
 * supplied in the dispatcher function call.
 *
 * \param handle, The Package encapsulator handle.
 * \param dispatcher, function pointer to user module dispatcher 
 *
 * \return Returns TRUE is successful.
 */
int Sia_pckg_set_response_handler(Sia_pckg_handle_t* handle, Sia_pckg_response_handler_t response_handler)
{
    if (handle == NULL)
    {
        return FALSE;
    }
    handle->reg_handler = response_handler;

    return TRUE;
} /* Sia_pckg_set_response_handler() */

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
 * \param oper_id, The id of the started operation.
 * \param sub_module, Data to the response handler.
 * \param action, Data to the response handler.
 * \param custom_data, custom data pointer to the response handler.
 *
 * \return Returns TRUE is successful.
 */
int Sia_pckg_reg_operation(
          Sia_pckg_handle_t *handle,
          Sia_pkg_handle_t  *pck_inst,
          WE_UINT32         oper_id,
          WE_INT32          sub_module,
          WE_INT32          action,
          void              *custom_data)
{
    we_pck_result_t     oper_result;
    Sia_pckg_data_t      *private_data = NULL;
    Sia_pckg_inst_list_t *inst = NULL;
    Sia_pckg_result_t    *result = NULL;
    int                   status;
 
    if (handle == NULL)
    {
        return FALSE;
    }

    /*
     *	Locate package instance:
     */
    inst = handle->reg_inst_list;
    while (inst != NULL && inst->pkg_inst != pck_inst)
    {
        inst = inst->next;
    }

    if (inst == NULL)
    {
        /*
         *	Instance not found:
         */
        return FALSE;
    }


    /*
     *	First check if operation is delayed:
     */
    status = inst->pkg_data->package_fncs->get_result_fnc(pck_inst, oper_id, &oper_result);
    
    if (WE_PACKAGE_COMPLETED == status)
    {
        result = WE_MEM_ALLOC(handle->mod_id, sizeof(Sia_pckg_result_t));
        if (NULL == result)
        {
            inst->pkg_data->package_fncs->free_result_fnc(inst->pkg_inst, &oper_result);
            
            return FALSE;
        }
        result->pkg_handle = inst->pkg_inst;
        result->free_fnc = inst->pkg_data->package_fncs->free_result_fnc;
        result->module = handle->mod_id;
        result->oper_result._u.data = oper_result._u.data;
        result->oper_result.type = oper_result.type;
        
        /*
         *	Operation completed, call response function.
         */
        ++inst->active_operations;
        (*handle->reg_handler)(
            sub_module,
            action,
            inst->pkg_data->pckg_id,
            pck_inst,
            oper_id,
            result,
            custom_data);
        --inst->active_operations;
        
        /*
         *	Free encapsulation data
         */
        WE_MEM_FREE(handle->mod_id, private_data);

        /*
         *	Free result data from operation:
         */
        if (TRUE == handle->do_free_result)
        {
            Sia_pckg_free_result(result);
        }

        return TRUE;
    }
    else if (WE_PACKAGE_ERROR == status)
    {
        return FALSE;
    }
    else /* operation delayed */
    {
        /*
         *	Some irrelevant data was returned....free...
         */
        inst->pkg_data->package_fncs->free_result_fnc(inst->pkg_inst, &oper_result);

        /*
         *	Set private data from operation:
         */
        private_data = WE_MEM_ALLOC(handle->mod_id, sizeof(Sia_pckg_data_t));
        if (private_data == NULL)
        {
            return FALSE;
        }
        private_data->custom_data = custom_data;
        private_data->response_action = action;
        private_data->sub_module = sub_module;
        private_data->operation_active = TRUE;

        /*
         *	Increment operation counter.
         */
        ++inst->active_operations;

        return inst->pkg_data->package_fncs->set_private_data_fnc(pck_inst, oper_id, private_data);
    }
} /* Sia_pckg_reg_operation() */

/*!
 * \brief  Free data retrived by get_result from an operation.
 */
void Sia_pckg_free_result(Sia_pckg_result_t *result)
{
    if (NULL != result)
    {
        result->free_fnc(result->pkg_handle, &(result->oper_result));
        WE_MEM_FREE(result->module, result);
    }
}

/*! /brief Closes down all encapsulation activities.
 *
 * /param handle Encapsulation handle.
 */
void Sia_pckg_destroy(Sia_pckg_handle_t *handle)
{
    Sia_pckg_inst_list_t *inst_list = NULL;
    Sia_pckg_pkg_list_t *pckg_list = NULL;
    
    inst_list = handle->reg_inst_list;
    while (NULL != inst_list)
    {
        Sia_pckg_terminate(handle, inst_list->pkg_inst, TRUE);
        inst_list = handle->reg_inst_list;
    }

    pckg_list = handle->reg_pkg_list;
    while (NULL != pckg_list)
    {
        Sia_pckg_unreg_pckg(handle, pckg_list->pckg_id);
        pckg_list = handle->reg_pkg_list;
    }

    WE_MEM_FREE(handle->mod_id, handle);
}
