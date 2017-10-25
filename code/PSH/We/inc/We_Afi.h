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
 * We_Afi.h 
 *
 * Attribute File Interface
 * This package allows file attributes to be saved. The file_name must be
 * encoded using utf-8.
 *
 * Created by Tomas Mandorf
 *
 * Revision  history:
 */

#ifndef _we_afi_h
#define _we_afi_h

#ifndef _we_cfg_h
#include "We_Cfg.h"
#endif

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
#define WE_AFI_OK               0
#define WE_AFI_ERROR_ACCESS    -1
#define WE_AFI_ERROR_DELAYED   -2
#define WE_AFI_ERROR_PATH      -3
#define WE_AFI_ERROR_INVALID   -4
#define WE_AFI_ERROR_SIZE      -5
#define WE_AFI_ERROR_FULL      -6
#define WE_AFI_ERROR_EOF       -7 
#define WE_AFI_ERROR_EXIST     -8

#define WE_AFI_ERROR_INTERNAL  -9  /* This should be the last error code */

/* Flags to be used with open file operation.
 * It is not possible to combine flags.
 */
#define WE_AFI_RDONLY            1 /* Open file in read only mode */
#define WE_AFI_WRONLY            2 /* Open file in write only mode. Creates
                                       new file if path does not exist. */
#define WE_AFI_RDWR              3 /* Open file in read/write mode. Creates
                                       new file if path does not exist. */

/* Flags to be used with save file operation. */
#define WE_AFI_OVERWRITE         0x1

#define WE_AFI_MAX_FILE_READ_SIZE  512

/* operation types */
#define WE_AFI_OPEN              1
#define WE_AFI_CLOSE             2
#define WE_AFI_READ              3
#define WE_AFI_WRITE             4
#define WE_AFI_SAVE              5
#define WE_AFI_REMOVE            6
#define WE_AFI_RENAME            7
#define WE_AFI_SET_ATTR          8
#define WE_AFI_GET_ATTR          9
#define WE_AFI_GET_DIR           10
#define WE_AFI_MKDIR             11
#define WE_AFI_DELETE            12
#define WE_AFI_EXIST             13

/**********************************************************************
 * Types
 **********************************************************************/
typedef struct {
  WE_INT32 result;     /* Result code for the operation:
                            WE_AFI_OK
                            WE_AFI_ERROR_EOF
                            WE_AFI_ERROR_ACCESS
                            WE_AFI_ERROR_DELAYED
                            WE_AFI_ERROR_INVALID
                            WE_AFI_ERROR_EXIST*/
  int       file_handle; /* use in subsequent read/write etc. */
} we_afi_result_open_t;

typedef struct {
  WE_INT32  result;     /* Result code for the operation:
                              WE_AFI_OK
                              WE_AFI_ERROR_EOF
                              WE_AFI_ERROR_ACCESS 
                              WE_AFI_ERROR_DELAYED 
                              WE_AFI_ERROR_INVALID */
  void      *data;       /* Pointer to data buffer supplied in 
                            the we_afi_read call*/
  long       data_len;   /* Actual number of bytes read.
                            If result equals WE_AFI_OK, this
                            value is the same as the input
                            parameter. If the result is 
                            WE_AFI_ERROR_EOF the number of 
                            bytes read might be less than the 
                            requested. In all other cases this
                            parameter is set to zero*/
} we_afi_result_read_t;

typedef struct {
  long bytes_saved;
} we_afi_additional_data_save_t;

typedef struct {
  WE_INT32            result;     /* Result code for the operation*/
  we_pck_attr_list_t *attr_list;  /* The attributes */
} we_afi_result_get_attributes_t ;

typedef struct {
  WE_BOOL             is_dir;    /* TRUE if Directory, else FALSE */
  char                *name;      /* Directory/File name */
  we_pck_attr_list_t *attr_list; /* Attributes associated with the file.*/
} we_afi_file_t;

typedef struct {
  WE_INT32       result;         /* When the result is WE_AFI_OK then */
  int             num_entries;    /* the remainging struct members are set.*/
  we_afi_file_t  entry_list[1];  /* List of files and directories.*/
} we_afi_result_dir_entries_t;


/**********************************************************************
 * Global functions
 **********************************************************************/

/*
 * Initialise the AFI package.
 * Returns a handle if success, otherwise returns NULL.
 */
we_pck_handle_t *
we_afi_init (WE_UINT8 modid);


/*
 * Runs the AFI package. When the module receives a signal defined by
 * WE, the module must call this function since the AFI package might
 * be the receiver.
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
we_afi_handle_signal (we_pck_handle_t *handle,
                       WE_UINT16        signal,
                       void             *p,
                       WE_INT32        *wid);


/*
 * Set private data connected to a specific AFI identifier. The private data
 * can be used, for instance, by the module to indicate which sub module that
 * invoked the operation.
 * Returns TRUE if success, otherwise FALSE.
 */
int
we_afi_set_private_data (we_pck_handle_t *handle, WE_INT32 wid, void *p);


/*
 * Get private data connected to a specific AFI identifier. The private data
 * can be used, for instance by the module to indicate where to continue
 * execution.
 * If success returns the private data, otherwise returns NULL.
 */
void *
we_afi_get_private_data (we_pck_handle_t *handle, WE_INT32 wid);


/*
 * Terminates the AFI package, release all resources allocated by
 * this package. If there are any open files or other operations,
 * these will be deleted. Any private data must be released by the
 * module itself before this function is called.
 */
int
we_afi_terminate (we_pck_handle_t *handle);


/**********************************************************************
 * Result
 **********************************************************************/
/*
 * Get the result from an operation
 *
 * handle The handle to the struct that holds the file package
 *        instance data.
 * wid     The wid to the operation instance
 * result The function initializes this parameter with current result type
 *        containing status code and optionally result data of the operation.
 *        All functions may return WE_AFI_ERROR_INTERNAL status code.
 *        Asynchronous functions may return WE_AFI_ERROR_DELAYED.
 *        All functions may return WE_AFI_OK status code except we_afi_exist.
 *
 *        Function/result type      Possible status codes
 *        we_afi_open              WE_AFI_ERROR_ACCESS  WE_AFI_ERROR_PATH
 *                                  WE_AFI_ERROR_PATH    WE_AFI_ERROR_SIZE
 *                                  WE_AFI_ERROR_FULL    WE_AFI_ERROR_EXIST
 *          we_afi_result_open_t
 *
 *        we_afi_close             WE_AFI_ERROR_INVALID
 *          WE_INT32
 *
 *        we_afi_read              WE_AFI_ERROR_ACCESS  WE_AFI_ERROR_INVALID
 *                                  WE_AFI_ERROR_EOF
 *          we_afi_result_read_t
 *
 *        we_afi_write             WE_AFI_ERROR_ACCESS  WE_AFI_ERROR_INVALID
 *                                  WE_AFI_ERROR_SIZE    WE_AFI_ERROR_FULL
 *          WE_INT32
 *
 *        we_afi_save              WE_AFI_ERROR_ACCESS  WE_AFI_ERROR_INVALID
 *                                  WE_AFI_ERROR_SIZE    WE_AFI_ERROR_FULL
 *          WE_INT32
 *
 *        we_afi_remove            WE_FILE_ERROR_ACCESS WE_AFI_ERROR_PATH
 *                                  WE_AFI_ERROR_INVALID
 *          WE_INT32
 *
 *        we_afi_rename            WE_AFI_ERROR_ACCESS  WE_AFI_ERROR_PATH
 *                                  WE_AFI_ERROR_EXIST   WE_AFI_ERROR_INVALID
 *          WE_INT32
 *
 *        we_afi_set_attributes    WE_AFI_ERROR_PATH    WE_AFI_ERROR_INVALID
 *          WE_INT32
 *
 *        we_afi_get_attributes    WE_AFI_ERROR_PATH
 *          we_afi_result_get_attributes_t *
 *
 *        we_afi_get_dir_entries   WE_AFI_ERROR_PATH    WE_AFI_ERROR_INVALID
 *          we_afi_result_dir_entries_t *
 *
 *        we_afi_delete            WE_AFI_ERROR_INVALID
 *          WE_INT32
 *
 *        we_afi_mkdir             WE_AFI_ERROR_ACCESS  WE_AFI_ERROR_PATH
 *                                  WE_AFI_ERROR_FULL    WE_AFI_ERROR_EXIST
 *          WE_INT32
 *
 *        we_afi_exist             WE_AFI_ERROR_PATH  WE_AFI_ERROR_EXIST
 *                                  WE_AFI_ERROR_INVALID
 *          WE_INT32
 *
 *        function                  additional data is a pointer to:
 *        we_afi_save              we_afi_additional_data_save_t
 *
 * Returns:
 *        WE_PACKAGE_ERROR on failure
 *        WE_PACKAGE_COMPLETED if operation has finished
 *        WE_PACKAGE_BUSY if operation has not finished yet
 */
int
we_afi_get_result (we_pck_handle_t *handle,
                    WE_INT32         wid,
                    we_pck_result_t *result);


/*
 * Free the storage allocated in the get result function.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * result     Result data to be freed.
 *
 * Returns:   TRUE if success, otherwise FALSE.
 */
int
we_afi_result_free (we_pck_handle_t *handle, we_pck_result_t *result);

/**********************************************************************
 * ASYNCHRONOUS FUNCTIONS
 **********************************************************************/
/* The functions below are asynchronous. That is the operation a function
 * implement may be delayed. Therefore the we_afi_get_result function
 * must be called using an operation wid to determine if the operation
 * is finished.
 */

/**********************************************************************
 * Save
 **********************************************************************/
/* Save a pipe, file, or buffer in a file with a specified file name.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the unique name of the file
 *            to save.
 * data_type  Tells if the data to save comes from a pipe, file or buffer.
 *            The value must be one of the following:
 *                 WE_PCK_DATA_TYPE_FILE,
 *                 WE_PCK_DATA_TYPE_PIPE,
 *                 WE_PCK_DATA_TYPE_BUFFER
 * data_len   The size of the data.
 *            Used only when the data source is a buffer.
 * data       The data that shall be written to file.
 *            Used only when the data source is a buffer.
 * path_name  The name of the pipe or file. This parameter is only used when
 *            data comes from a pipe or file.
 * file_attr  File attributes.
 * flags      Save flags. Possible values are:
 *              WE_AFI_OVERWRITE
 *
 * Returns:   An wid to the save operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_save (we_pck_handle_t          *handle,
              const char                *file_name,
              int                        data_type,
              long                       data_len,
              const char                *data,
              const char                *path_name,
              const we_pck_attr_list_t *file_attr);

WE_INT32
we_afi_save_ext (we_pck_handle_t          *handle,
                  const char                *file_name,
                  int                        data_type,
                  long                       data_len,
                  const char                *data,
                  const char                *path_name,
                  const we_pck_attr_list_t *file_attr,
                  WE_UINT32                 flags);

/**********************************************************************
 * Delete
 **********************************************************************/
/* Deletes an operation. Private data is not released by this function.
 * The module calls this function when the module, for some reason must
 * delete the dialog before completion.
 * NOTE! Delete works only on the Save operation when saving from a pipe.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * wid         The wid to the operation instance that is to be deleted
 *
 * Returns:   An wid to the delete operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_delete (we_pck_handle_t *handle, WE_INT32 wid);


/**********************************************************************
 * Write
 **********************************************************************/
/* Writes the specified number of bytes to the file pointed at by
 * the file_handle parameter. The function does not return finished
 * until all bytes have been written. If all bytes cannot be written the 
 * function returns an error
 *
 * NOTE! It is not possible to make more than one (1) simultaneous write 
 *       operation on the same file handle.
 *
 * handle       The handle to the struct that holds the file package
 *              instance data.
 * file_handle  File handle returned from the we_afi_open operation.
 * data         The data to be written.
 * data_len     The size in bytes of the data parameter.
 *
 * Returns:     An wid for operation or WE_PACKAGE_ERROR if failure.
 */
WE_INT32
we_afi_write (we_pck_handle_t *handle,
               WE_INT32         file_handle,
               void             *data,
               long              data_len);


/**********************************************************************
 * Read
 **********************************************************************/
/* Reads the specified number of bytes from the file associated with
 * the file_handle parameter. The function does not return finished
 * until all bytes have been read. If it is not possible to read the
 * specified number of bytes the number of actually read bytes will
 * be returned in the we_afi_get_result call.
 *
 * handle       The handle to the struct that holds the file package
 *              instance data.
 * file_handle  File handle returned from the we_afi_open operation.
 * data         Data buffer.
 * data_len     Number of bytes to read into the data buffer.
 *
 * Returns:     An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_read (we_pck_handle_t *handle,
              WE_INT32         file_handle,
              void             *data,
              long              data_len);


/**********************************************************************
 * Remove
 **********************************************************************/
/* Remove a file or an empty directory.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * item       Null terminated string containing the unique name of the file
 *            or directory to delete.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_remove (we_pck_handle_t *handle, const char *item);


/**********************************************************************
 * Set attributes
 **********************************************************************/
/* Set file attributes for a specific file. The operation will remove
 * old attributes. File attributes are thus modified by:
 * 1) Get all attributes for the file
 * 2) Modify the data in memory
 * 3) Set the attributes for the file
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the unique name of the file
 *            for which the attributes will be set.
 * attr_list  A list of attributes.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_set_attributes (we_pck_handle_t          *handle,
                        const char                *file_name,
                        const we_pck_attr_list_t *attr_list);


/**********************************************************************
 * Get attributes
 **********************************************************************/
/* Get attributes associated with a specific file.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the unique name of the file
 *            that has the attributes that shall be returned.
 * bit_mask   NOTE: NOT IMPLEMENTED.
 *            Bit mask to specify which attributes to return. If this flag
 *            is set to 0 all attributes will be returned.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR,
 */
WE_INT32
we_afi_get_attributes (we_pck_handle_t *handle,
                        const char       *file_name,
                        WE_UINT32        bit_mask);


/**********************************************************************
 * Get dir entries
 **********************************************************************/
/* Retrieves a list of all files and subdirectories for a given directory.
 * When the result member of the we_afi_result_dir_entries_t struct is
 * WE_AFI_OK then the directory result data is set in entry_list.
 * Thus, there are no partial directory data to be read.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * dir_name   Null terminated string containing the unique name of the
 *            directory that shall be read.
 * bit_mask   NOTE: NOT IMPLEMENTED
 *            Bit mask to specify which attributes for each file and
 *            folder to include in the result.
 *            If this flag is set to 0 all attributes will be returned.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR,
 */
WE_INT32
we_afi_get_dir_entries (we_pck_handle_t *handle,
                         const char       *dir_name,
                         WE_UINT32        bit_mask);


/**********************************************************************
 * Rename
 **********************************************************************/
/* Rename the file or directory specified by old_name to the name
 * specified by new_name.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * old_name   Null terminated string with the old file or directory name.
 * new_name   Null terminated string with the new file or directory name.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR,
 */
WE_INT32
we_afi_rename (we_pck_handle_t *handle,
                const char       *old_name,
                const char       *new_name);


/**********************************************************************
 * Exist
 **********************************************************************/
/* Check if a file exist with attributes.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the name of the file
 *            that should be checked for existence.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_exist (we_pck_handle_t *handle, const char *file_name);


/**********************************************************************/
/* The functions we_afi_open, we_afi_mkdir and we_afi_close are
 * synchronously implemented. However the asynchronous interface is
 * kept for compatibility purpose.
 */

/**********************************************************************
 * Open
 **********************************************************************/
/* Initialise the specified file for read/write operations. It it not
 * necessary to open a file before its attributes are set/retrieved.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * file_name  Null terminated string containing the unique name of
 *            the file to open. The filename cannot end with a '/'.
 * mode       The open mode to be used. Possible modes are:
 *              WE_AFI_RDONLY
 *              WE_AFI_WRONLY
 *              WE_AFI_RDWR
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_open (we_pck_handle_t *handle, const char *file_name, int mode);


/**********************************************************************
 * Make Directory
 **********************************************************************/
/* Create a new directory. If any intermediate directory does not exist
 * it will be created.
 * 
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * dir_name   Null terminated string with the name of the directory to create.
 *
 * Returns:   An wid for the operation or WE_PACKAGE_ERROR.
 */
WE_INT32
we_afi_mkdir (we_pck_handle_t *handle, const char *dir_name);


/**********************************************************************
 * Close
 **********************************************************************/
/* Close a file using the file handle returned from we_afi_open.
 *
 * handle       The handle to the struct that holds the file package
 *              instance data.
 * file_handle  File handle returned from the we_afi_open operation.
 *
 * Returns:     An wid or WE_PACKAGE_ERROR
 */
WE_INT32
we_afi_close (we_pck_handle_t *handle, WE_INT32 file_handle);


/**********************************************************************
 * SYNCHRONOUS FUNCTIONS
 **********************************************************************/
/* The functions listed below are synchronous. Hence they do not
 * implement any get result function.
 */

/**********************************************************************
 * Get Size of Directory
 **********************************************************************/
/* Dtermine the total size of the directory including subdirectories.
 * NOTE: only user root folders and user folders are applicable.
 *
 * handle     The handle to the struct that holds the file package
 *            instance data.
 * dir_name   Null terminated string with the path name of the directory.
 *
 * Returns:   The size of the directory or NULL if dir_name is invalid.
 */
long
we_afi_get_dir_size (const we_pck_handle_t *handle, const char *dir_name);


/**********************************************************************
 * Seek
 **********************************************************************/
/* Move the file pointer of an open file. If the seek operation exceeds
 * the limits of the file an error is returned. Offset may be negative.
 *
 * handle      The handle to the struct that holds the file package
 *             instance data.
 * file_handle File handle returned from we_afi_open.
 * offset      Distance in bytes to move.
 * seek_mode   Offset start position.
 *             TPI_FILE_SEEK_SET offset from start of file
 *             TPI_FILE_SEEK_CUR relative offset from current position
 *             TPI_FILE_SEEK_END offset from end of file
 *
 * Returns:    The new position of the file pointer or a negative value
 *             if an error occurred. Error codes:
 *
 *             WE_AFI_ERROR_ACCESS The file was opened in
 *                TPI_FILE_SET_APPEND mode.
 *             WE_AFI_ERROR_INVALID e.g. the fileHandle argument is not
 *                an open file descriptor or the requested new cursor
 *                position exceeds the limits of the file.
 */
WE_INT32
we_afi_seek (const we_pck_handle_t *handle,
              WE_INT32               file_handle,
              long                    offset,
              int                     seek_mode);

#endif /*_we_afi_h*/
