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
 * We_Pipe.h
 *
 * Created by Ingmar Persson
 *
 * Revision  history:
 *   020905, AED: Added error codes
 *   021014, IPN: Added WE_PIPE_CLOSE_ALL
 *
 */
#ifndef _we_pipe_h
#define _we_pipe_h

#ifndef _we_int_h
#include "We_Int.h"
#endif

/****************************************
 * Named Pipes
 ****************************************/

/* Notification types */
#define WE_PIPE_EVENT_CLOSED                     TPI_PIPE_EVENT_CLOSED
#define WE_PIPE_EVENT_READ                       TPI_PIPE_EVENT_READ
#define WE_PIPE_EVENT_WRITE                      TPI_PIPE_EVENT_WRITE

#ifdef NEW_PIPE_ERROR_EVENT
#define WE_PIPE_EVENT_CLOSED_ERROR               TPI_PIPE_EVENT_CLOSED_ERROR
#endif

/*
 * Error codes
 */
#define WE_PIPE_ERROR_BAD_HANDLE                 TPI_PIPE_ERROR_BAD_HANDLE
#define WE_PIPE_ERROR_INVALID_PARAM              TPI_PIPE_ERROR_INVALID_PARAM
#define WE_PIPE_ERROR_EXISTS                     TPI_PIPE_ERROR_EXISTS
#define WE_PIPE_ERROR_IS_OPEN                    TPI_PIPE_ERROR_IS_OPEN
#define WE_PIPE_ERROR_DELAYED                    TPI_PIPE_ERROR_DELAYED
#define WE_PIPE_ERROR_RESOURCE_LIMIT             TPI_PIPE_ERROR_RESOURCE_LIMIT
#define WE_PIPE_ERROR_CLOSED                     TPI_PIPE_ERROR_CLOSED
#define WE_PIPE_ERROR_NOT_FOUND                  TPI_PIPE_ERROR_NOT_FOUND

/*
 * Functions
 */
#define WE_PIPE_CREATE                           TPIa_PipeCreate
#define WE_PIPE_OPEN                             TPIa_PipeOpen
#define WE_PIPE_CLOSE                            TPIa_PipeClose
#define WE_PIPE_DELETE                           TPIa_PipeDelete
#define WE_PIPE_READ                             TPIa_PipeRead
#define WE_PIPE_WRITE                            TPIa_PipeWrite
#define WE_PIPE_POLL                             TPIa_PipePoll
#define WE_PIPE_STATUS                           TPIa_PipeStatus
#define WE_PIPE_CLOSE_ALL                        TPIa_PipeCloseAll

#ifdef NEW_PIPE_ERROR_EVENT
#define WE_PIPE_CLOSE_ERROR                      TPIa_PipeCloseError
#endif

#ifdef NEW_PIPE_STATUS
#define WE_PIPE_STATUS_SET                       TPIa_pipeStatusSet
#define WE_PIPE_STATUS_GET                       TPIa_pipeStatusGet
#endif

#endif
