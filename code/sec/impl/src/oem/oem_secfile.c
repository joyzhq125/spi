/*==============================================================================
    FILE NAME : oem_secfile.c
    MODULE NAME : WEA
    
    GENERAL DESCRIPTION
        This file implements WeFile API.
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2006 by TECHFAITH Software. All Rights Reserved.
================================================================================
    Revision History
    
    Modification Tracking
    Date               Author               Number      Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-06-08    HouGuanhua        none         Initialization
    2006-06-22    HouGuanhua        none         modify WeFile_ConvertPathWeToBrew
    2006-06-23    HouGuanhua        none         modify WeFile_ConvertPathWeToBrew,  other module can use it
    2006-1-15     bluefire          none         remove pc_lint warning
    
==============================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include "we_def.h"
#include "oem_secerr.h"
#include "oem_secevent.h"
#include "oem_sechandle.h"
#include "oem_secfile.h"

/*******************************************************************************
* Macro Define Section
*******************************************************************************/
#define WE_FILE_PATH "fs:/shared/"


/*==================================================================================================
FUNCTION: 
    WeFile_ConvertPathWeToBrew
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Add WE and BREW prefix string, and convert to lower case.
ARGUMENTS PASSED:
    WE_CHAR *pcFileName: File name
RETURN VALUE:
    Zero terminated WE_CHAR string. 
    NULL, if error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    The parameter pcFileName should be like "file", "/file", "dir/file", 
    "/dir/file", etc. The string returned must be freed by user.
==================================================================================================*/
WE_CHAR * WeFile_ConvertPathWeToBrew
(
    const WE_CHAR *pcFileName
)
{
    WE_CHAR acTemp[5] ={0};
    WE_UINT8 ucFileNameLen = 0;
    WE_CHAR *pcWeName = NULL;
    WE_CHAR *pcBrewName = NULL;
    WE_BOOL bAddPreFix = TRUE;

    if ( NULL == pcFileName )
    {
        return NULL;
    }
    pcWeName = (WE_CHAR*)pcFileName;      /* save name pointer */

    /* save pcFileName's first 4 char and convert to lower case*/
    (void)STRNCPY(acTemp, pcWeName, 4);

    if(STRBEGINS(AEEFS_ROOT_DIR, acTemp))
    {
        bAddPreFix = FALSE;
        ucFileNameLen = (WE_UCHAR)STRLEN(pcWeName);
    }
    else if(STRBEGINS("/", acTemp))
    {
        pcWeName++;     /*skip "/" */
        ucFileNameLen = (WE_UCHAR)(STRLEN(WE_FILE_PATH)+STRLEN(pcWeName));
    }
    else
    {
        ucFileNameLen = (WE_UCHAR)(STRLEN(WE_FILE_PATH)+STRLEN(pcWeName));
    }
    
    pcBrewName = MALLOC(ucFileNameLen+1);
    if(NULL == pcBrewName)
    {
        return NULL;
    }
    (void)MEMSET(pcBrewName, 0, ucFileNameLen+1);
    if(bAddPreFix)
    {
        (void)STRCPY(pcBrewName, WE_FILE_PATH);
    }
    (void)STRCAT(pcBrewName, pcWeName);


    return pcBrewName;
#if 0
    /**If pathname start with "/external/", then remove the "/external"    */
    uiStrLen = STRLEN(pcFileName);

    if(uiStrLen>10)
    {
        /*Convert to lower case*/
        STRNCPY((WE_CHAR *)&acTemp, pcFileName, 10);
        for(i=0; i<10; i++)
        {
            if((acTemp[i]>=0x41)&&(acTemp[i]<=0x5A))
            {
                acTemp[i] += 0x20;
            }        
        }
            /*Compare*/
        if(STRNCMP(acTemp, (WE_CHAR *)"/external/", 10) == 0)
        {
            pcName = (WE_CHAR *)(pcFileName + 10);
        }
        else
        {
            pcName = (WE_CHAR *)pcFileName;
        }        
    }
    else
    {
        pcName = (WE_CHAR *)pcFileName;
    }
    return pcName;
#endif
} 

/*==================================================================================================
FUNCTION: 
    WeFile_ConvertSeekModeWeToBrew
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Convert WE seek mode to brew seek mode.
ARGUMENTS PASSED:
    E_WE_FILE_SEEK_MODE eSeekMode: Seek mode
RETURN VALUE:
    Seek type.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static FileSeekType WeFile_ConvertSeekModeWeToBrew(E_WE_FILE_SEEK_MODE eSeekMode)
{
    FileSeekType iSeekType = _SEEK_START;

    switch(eSeekMode)
    {
        case WE_FILE_SEEK_SET:
            iSeekType = _SEEK_START;
            break;
        case WE_FILE_SEEK_CUR:
            iSeekType = _SEEK_CURRENT;
            break;
        case WE_FILE_SEEK_END:
            iSeekType = _SEEK_END;
            break;
        default:
            break;
    }
    return iSeekType;
}


/*==================================================================================================
FUNCTION: 
    WeFile_ConvertOpenModeWeToBrew
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Convert WE open mode to brew open mode.
ARGUMENTS PASSED:
    E_WE_FILE_MODE eMode: Open mode
RETURN VALUE:
    Open type.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static OpenFileMode WeFile_ConvertOpenModeWeToBrew(E_WE_FILE_MODE eMode)
{
    OpenFileMode iOpenMode = 0;
    
    if(WE_FILE_SET_RDONLY == eMode)
    {
        iOpenMode = _OFM_READ;
    }
    else if(WE_FILE_SET_CREATE == eMode)
    {
        iOpenMode = _OFM_CREATE;
    }        
    else if((WE_FILE_SET_RDWR == eMode)||(WE_FILE_SET_WRONLY == eMode))
    {
        iOpenMode = _OFM_READWRITE;
    }    
    else if( WE_FILE_SET_APPEND == eMode)
    {
        iOpenMode = _OFM_APPEND;
    }
    
    return iOpenMode;
}


/*==================================================================================================
FUNCTION: 
    WeFile_ConvertResultBrewToWe
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Convert brew result to WE mode.
ARGUMENTS PASSED:
    WE_INT32 iResult: result
RETURN VALUE:
    result.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static E_WE_ERROR WeFile_ConvertResultBrewToWe(WE_INT32 iResult)
{
    E_WE_ERROR eReturn = (E_WE_ERROR)-1;
    
    switch(iResult)
    {
        case EFILEEXISTS:// File exist
            eReturn = E_WE_FILE_ERROR_EXIST;
            break;
        case EDIRNOEXISTS:// Directory does not exist
        case EBADFILENAME:// Bad file name
        case EFILENOEXISTS:// File does not exist
            eReturn = E_WE_FILE_ERROR_PATH;
            break;
        case EBADSEEKPOS: // Bad seek position
            eReturn = E_WE_FILE_ERROR_BADSEEKPOS;
            break;
        case EINVALIDOPERATION:
        case EBADPARM:
        case EOUTOFNODES:
            eReturn = E_WE_FILE_ERROR_INVALID;
            break;
        case EFILEOPEN:// File already open
        case EDIRNOTEMPTY:// Directory not empty
            eReturn = E_WE_FILE_ERROR_ACCESS;
            break;
        case EFILEEOF:// End of file
            eReturn = E_WE_FILE_ERROR_EOF;
            break;
        case EFSFULL: // File system full
        case ENOMEDIA:// Storage media not present
            eReturn = E_WE_FILE_ERROR_FULL;
            break;
        default:
            eReturn = E_WE_ERROR_OTHER;
            break;
    }
    
    return eReturn;
}


/*==================================================================================================
FUNCTION: 
    WeFile_Open
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Creates and/or opens a file with the name pszFileName.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    WE_CHAR  *pszFileName: Null terminated string containing the file name
    E_WE_FILE_MODE eMode: File open mode
    WE_HANDLE *phWeFileHandle: Point to the IFile Interface object of the file opened If successful
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeHandle is not a valid handle, or name is not a valid path name for a file. 
        or Errror in parameters e.g. mode specifies both WE_FILE_SET_RDONLY 
        and WE_FILE_SET_WRONLY. 
    E_WE_FILE_ERROR_PATH
        The named file does not exist. For example, the path is null or a component 
        of path does not exist, or the file itself does not exist and WE_FILE_SET_CREATE 
        is not set. 
    E_WE_FILE_ERROR_SIZE 
    E_WE_FILE_ERROR_FULL 
    E_WE_FILE_ERROR_EXIST 
        E_WE_FILE_SET_EXCL and E_WE_FILE_SET_CREATE are set and the named file exists. 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR  
WeFile_Open
(
    WE_HANDLE hWeHandle,
    WE_CHAR *pszFileName,
    E_WE_FILE_MODE eMode,
    WE_HANDLE *phWeFileHandle
)
{
    St_WeFileHandle *ot_pStFileHandle = NULL;
    IFileMgr* pIFileMgr = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    OpenFileMode OpenMode;
    WE_CHAR *pszBrewFileName = NULL;
    WE_INT32 i = 0;
    WE_INT32 iRes = 0;

    if((NULL == hWeHandle)
        ||(NULL == pszFileName)
        ||(NULL == phWeFileHandle))
    {
        return eResult;
    }

    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    
    /*find an empty file handle */
    for(i=0; i<OPEN_FILE_MAX_NUM;i++)
    {
        if(NULL == ((St_WeHandle*) hWeHandle)->aphFileList[i])
            break;
    }
    if(OPEN_FILE_MAX_NUM == i)
    {
        eResult = E_WE_FILE_ERROR_FULL;
        return eResult;
    }

    pIFileMgr = ((St_WeHandle*)hWeHandle)->pIFileMgr;
    
    ot_pStFileHandle = MALLOC(sizeof(St_WeFileHandle))  ;
    if(NULL == ot_pStFileHandle)
    {
        eResult = E_WE_ERROR_RESOURCE_LIMIT;
        return eResult;
    }
    ot_pStFileHandle->iMagicNum = WE_FILE_HANDLE_MAGIC_NUM;
    ot_pStFileHandle->hWeHandle = hWeHandle;
    ot_pStFileHandle->iFileHandleID = (WE_INT8)i;
    ot_pStFileHandle->pIFile = NULL;

    pszBrewFileName = WeFile_ConvertPathWeToBrew(pszFileName);
    OpenMode = WeFile_ConvertOpenModeWeToBrew(eMode);            

        if(NULL == pszBrewFileName)
        {
            FREE(ot_pStFileHandle);
            return E_WE_ERROR_RESOURCE_LIMIT;
        }
    ot_pStFileHandle->pIFile = IFILEMGR_OpenFile(pIFileMgr, pszBrewFileName, OpenMode);
    if(NULL == (ot_pStFileHandle->pIFile))
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
        FREE(ot_pStFileHandle);
    }
    else
    {
        /* save WeFileHandle in WeHandle */
        ((St_WeHandle*) hWeHandle)->aphFileList[i] = ot_pStFileHandle;
        *phWeFileHandle = ot_pStFileHandle;
        eResult = E_WE_OK;
    }
        
    FREE(pszBrewFileName);
    return eResult ;
}


/*==================================================================================================
FUNCTION: 
    WeFile_Close
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function closes the file.
ARGUMENTS PASSED:
    WE_HANDLE hWeFileHandle: File handle
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    This function closes the file, i.e. deallocates the file handle/descriptor indicated by fileHandle. 
    To deallocate basically means to make the file handle available for return by subsequent calls to 
    WeFile_Open or other functions that allocate file handles. 
==================================================================================================*/
E_WE_ERROR 
WeFile_Close 
(
    WE_HANDLE hWeFileHandle
)
{
    St_WeHandle* hWeHandle = NULL;    
    WE_INT32 iFileHandleID = 0;
    IFile *pIFile = NULL;
    
    if(NULL == hWeFileHandle)
    {
        return E_WE_OK;
    }
    
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }

    hWeHandle = ((St_WeFileHandle*)hWeFileHandle)->hWeHandle;
    iFileHandleID = ((St_WeFileHandle*)hWeFileHandle)->iFileHandleID;
    pIFile = ((St_WeFileHandle*)hWeFileHandle)->pIFile;

    ((St_WeFileHandle*)hWeFileHandle)->iMagicNum = 0;
    ((St_WeFileHandle*)hWeFileHandle)->hWeHandle = NULL;
    ((St_WeFileHandle*)hWeFileHandle)->iFileHandleID = (WE_CHAR)(-1);
    if(NULL != pIFile )
    {
        (void)IFILE_Release(pIFile);
    }
    
    FREE(hWeFileHandle);
    hWeFileHandle = NULL;

    /* clear WeFileHandle record in WeHandle */
    hWeHandle->aphFileList[iFileHandleID] = NULL;
    
    return E_WE_OK;
}


/*==================================================================================================
FUNCTION: 
    WeFile_Write
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function attempts to write size bytes from the buffer.
ARGUMENTS PASSED:
    WE_HANDLE hWeFileHandle: File handle
    void *pvDataBuf: Point to the data
    WE_LONG lBufSize: Length of data
    WE_LONG *plWriteNum: Length of write 
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_ACCESS 
        E.g. attempt to write in a file opened as read only. 
    E_WE_ FILE_ERROR_SIZE 
    E_WE_ FILE_ERROR_FULL 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_Write 
(
    WE_HANDLE hWeFileHandle,
    void *pvDataBuf, 
    WE_LONG lBufSize,
    WE_LONG *plWriteNum
)
{
    St_WeHandle* hWeHandle = NULL;    
    IFileMgr *pIFileMgr = NULL;
    IFile *pIFile = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iRes = 0;

    if((NULL == hWeFileHandle)
        ||(NULL == pvDataBuf) 
        ||(lBufSize<= 0 )
        ||(NULL ==plWriteNum))
    {
        return eResult;
    }
    
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    hWeHandle = ((St_WeFileHandle*)hWeFileHandle)->hWeHandle;
    pIFileMgr = hWeHandle->pIFileMgr;
    pIFile = ((St_WeFileHandle*)hWeFileHandle)->pIFile;

    *plWriteNum = (WE_LONG)IFILE_Write(pIFile, pvDataBuf, (WE_UINT32)lBufSize);
    if(0 == (*plWriteNum))
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
    }
    else
    {
        eResult = E_WE_OK;
    }
    
    return eResult;
}


/*==================================================================================================
FUNCTION: 
    WeFile_Read
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function attempts to read size bytes from the buffer. 
ARGUMENTS PASSED:
    WE_HANDLE hWeFileHandle: File handle
    void *pvDataBuf: buffer
    WE_LONG lBufSize: Length of buffer
    WE_LONG *plReadNum: Length of read
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_Read 
(
    WE_HANDLE hWeFileHandle,
    void *pvDataBuf, 
    WE_LONG lBufSize,
    WE_LONG *plReadNum
)
{
    St_WeHandle* hWeHandle = NULL;    
    IFileMgr *pIFileMgr = NULL;
    IFile *pIFile = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iRes = 0;

    if((NULL == hWeFileHandle)
        ||(NULL == pvDataBuf) 
        ||(lBufSize<= 0 )
        ||(NULL == plReadNum))
    {
        return eResult;
    }

    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    hWeHandle = ((St_WeFileHandle*)hWeFileHandle)->hWeHandle;
    pIFileMgr = hWeHandle->pIFileMgr;
    pIFile = ((St_WeFileHandle*)hWeFileHandle)->pIFile;

    *plReadNum = (WE_LONG)IFILE_Read(pIFile, pvDataBuf, (WE_UINT32)lBufSize);
    if(0 == (*plReadNum))
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        if(SUCCESS == iRes)
        {
            eResult = E_WE_ERROR_DELAYED;
        }
        else
        {
            eResult = WeFile_ConvertResultBrewToWe(iRes);
        }
    }
    else
    {
        eResult = E_WE_OK;
    }
    
    return eResult;
}


/*==================================================================================================
FUNCTION: 
    WeFile_Seek
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    Moves the file pointer of an open file, allowing random access read and write operations.  
ARGUMENTS PASSED:
    WE_HANDLE hWeFileHandle: File handle
    WE_LONG lOffset: Offset
    E_WE_FILE_SEEK_MODE eSeekMode: Seek mode
    WE_INT32 *piNewPos: New position
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_Seek 
(
    WE_HANDLE hWeFileHandle, 
    WE_LONG lOffset, 
    E_WE_FILE_SEEK_MODE eSeekMode,
    WE_INT32 *piNewPos
)
{
    St_WeHandle* hWeHandle = NULL;    
    IFileMgr *pIFileMgr = NULL;
    IFile *pIFile = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    FileSeekType SeekType;
    WE_INT32 iRes = 0;
    WE_INT32 iResult = 0;

    if((NULL == hWeFileHandle) || (NULL == piNewPos))
    {
        return eResult;
    }
    
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }

    hWeHandle = ((St_WeFileHandle*)hWeFileHandle)->hWeHandle;
    pIFileMgr = hWeHandle->pIFileMgr;
    pIFile = ((St_WeFileHandle*)hWeFileHandle)->pIFile;
    SeekType = WeFile_ConvertSeekModeWeToBrew(eSeekMode);
    
    iResult = IFILE_Seek(pIFile, SeekType, (WE_INT32)lOffset);
    if(EFAILED == iResult)
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
    }
    else        /* eResult equal to SUCCESS or file pointer position*/
    {
        *piNewPos = IFILE_Seek(pIFile, _SEEK_CURRENT, 0);
        eResult = E_WE_OK;
    }

    return eResult;
}



/*==================================================================================================
FUNCTION: 
    WeFile_Remove
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function removes the file identified by pszFileName.   
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    const WE_CHAR *pszFileName: File name
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_Remove 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszFileName
)
{
    IFileMgr *pIFileMgr = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_CHAR *pszBrewPathName = NULL;
    WE_INT32 iRes = 0;

    if((NULL == hWeHandle)
        ||(NULL == pszFileName) )
    {
        return eResult;
    }

    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }

    pIFileMgr = ((St_WeHandle*)hWeHandle)->pIFileMgr;
    pszBrewPathName = WeFile_ConvertPathWeToBrew(pszFileName);
        if(NULL == pszBrewPathName)
        {
            return E_WE_ERROR_RESOURCE_LIMIT;
        }
    
    iRes = IFILEMGR_Remove(pIFileMgr, pszBrewPathName);
    if(SUCCESS == iRes)
    {
        eResult = E_WE_OK;
    }
    else
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
    }

    FREE(pszBrewPathName);
    return eResult;
}



/*==================================================================================================
FUNCTION: 
    WeFile_Rename
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function removes the file identified by pszFileName.   
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    const WE_CHAR *pszSrcName: Source file to be renamed
    const WE_CHAR *pszDstName: Destination file
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_Rename 
(
    WE_HANDLE hWeHandle,
    const WE_CHAR *pszSrcName,
    const WE_CHAR *pszDstName
)
{
    IFileMgr *pIFileMgr = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_CHAR *pszBrewSrcName = NULL;
    WE_CHAR *pszBrewDstName = NULL;
    WE_INT32 iRes = 0;

    if((NULL == hWeHandle)
        ||(NULL == pszSrcName)
        ||(NULL == pszDstName))
    {
        return eResult;
    }

    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }

    pIFileMgr = ((St_WeHandle*)hWeHandle)->pIFileMgr;
    pszBrewSrcName = WeFile_ConvertPathWeToBrew(pszSrcName);
        if(NULL == pszBrewSrcName)
        {
            return E_WE_ERROR_RESOURCE_LIMIT;
        }
    pszBrewDstName = WeFile_ConvertPathWeToBrew(pszDstName);
        if(NULL == pszBrewDstName)
        {
            FREE(pszBrewSrcName);
            return E_WE_ERROR_RESOURCE_LIMIT;
        }
    
    iRes = IFILEMGR_Rename(pIFileMgr,pszBrewSrcName,pszBrewDstName);    
    if(SUCCESS == iRes)
    {
        eResult = E_WE_OK;
    }
    else
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
    }

    FREE(pszBrewSrcName);
    FREE(pszBrewDstName);
    return eResult;
}


/*==================================================================================================
FUNCTION: 
    WeFile_MakeDir
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function create a directory name by dirName.   
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    const WE_CHAR *pszDirName: Directory name
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_FULL 
    E_WE_FILE_ERROR_PATH 
    E_WE_FILE_ERROR_EXIST
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_MakeDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName
)
{
    IFileMgr *pIFileMgr = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_CHAR *pszBrewDirName = NULL;
    WE_INT32 iRes = 0;

    if((NULL == hWeHandle)
        ||(NULL == pszDirName))
    {
        return eResult;
    }

    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }

    pIFileMgr = ((St_WeHandle*)hWeHandle)->pIFileMgr;
    pszBrewDirName = WeFile_ConvertPathWeToBrew(pszDirName);
        if(NULL == pszBrewDirName)
        {
            return E_WE_ERROR_RESOURCE_LIMIT;
        }

    iRes = IFILEMGR_MkDir(pIFileMgr, pszBrewDirName);
    if(SUCCESS == iRes)
    {
        eResult = E_WE_OK;
    }
    else
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
    }

    FREE(pszBrewDirName);
    return eResult;
}




/*==================================================================================================
FUNCTION: 
    WeFile_RemoveDir
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function removes a directory named by pathName.   
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    const WE_CHAR *pszDirName: Directory name
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_ FILE_ERROR_ACCESS 
    E_WE_FILE_ERROR_PATH 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_RemoveDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName
)
{
    IFileMgr *pIFileMgr = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_CHAR *pszBrewDirName = NULL;
    WE_INT32 iRes = 0;

    if((NULL == hWeHandle)
        ||(NULL == pszDirName))
    {
        return eResult;
    }
    
    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    
    pIFileMgr = ((St_WeHandle*)hWeHandle)->pIFileMgr;
    pszBrewDirName = WeFile_ConvertPathWeToBrew(pszDirName);
        if(NULL == pszBrewDirName)
        {
            return E_WE_ERROR_RESOURCE_LIMIT;
        }

    iRes = IFILEMGR_RmDir(pIFileMgr, pszBrewDirName);
    if(SUCCESS == iRes)
    {
        eResult = E_WE_OK;
    }
    else
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
    }

    FREE(pszBrewDirName);
    return eResult;
}



/*==================================================================================================
FUNCTION: 
    WeFile_GetDirSize
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function get the entry count in the directory.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    const WE_CHAR *pszDirName: Directory name
    WE_INT32* piSize: Number of file and directory within pszDirName
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_PATH 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_GetDirSize
(
    WE_HANDLE hWeHandle,
    const WE_CHAR *pszDirName,
    WE_INT32* piSize
)
{
    IFileMgr *pIFileMgr = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_CHAR  *pszBrewDirName = NULL;
    FileInfo stFileInfo = {0};
    WE_UINT32 uiEntryCnt = 0;
    WE_INT32 iRes = 0;

    if((NULL == hWeHandle)
        ||(NULL == pszDirName)
        ||(NULL == piSize))
    {
        return eResult;
    }

    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    
    pIFileMgr = ((St_WeHandle*)hWeHandle)->pIFileMgr;
    pszBrewDirName = WeFile_ConvertPathWeToBrew(pszDirName);
        if(NULL == pszBrewDirName)
        {
            return E_WE_ERROR_RESOURCE_LIMIT;
        }
        
    /*Make the aim directory as the current root dir*/
    iRes = IFILEMGR_EnumInit(pIFileMgr, pszBrewDirName, TRUE);/*TRUE means only enum directory, and FALSE is only for file*/
    if(SUCCESS == iRes)
    {
        /*Get the information of directory in the current dir*/
        while(IFILEMGR_EnumNext(pIFileMgr, &stFileInfo))
        {
            uiEntryCnt ++;
        }
        eResult = E_WE_OK;
    }
    else
    {
           FREE(pszBrewDirName);
        return E_WE_FILE_ERROR_PATH;
    }
    
    /*Make the aim directory as the current root dir*/
    iRes = IFILEMGR_EnumInit(pIFileMgr, pszBrewDirName, FALSE);/*TRUE means only enum directory, and FALSE is only for file*/
    if(SUCCESS == iRes)
    {
        /*Get the information of files in the current dir*/
        while(IFILEMGR_EnumNext(pIFileMgr, &stFileInfo))
        {
            uiEntryCnt ++;
        }
        *piSize = (WE_INT32)uiEntryCnt;
        eResult = E_WE_OK;
    }
    else
    {
        eResult = E_WE_FILE_ERROR_PATH;
    }
    
    FREE(pszBrewDirName);
    return eResult;
}




/*==================================================================================================
FUNCTION: 
    WeFile_ReadDir
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function get the entry count in the directory.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    const WE_CHAR *pszDirName: Directorie name
    WE_INT32 iPos: position
    WE_CHAR *pszNameBuf: File name
    WE_INT32 pszNameBufLength: Length of pszNameBuf
    E_WE_FILE_TYPE* eType: File type
    WE_LONG *plSize: File size
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_PATH 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR 
WeFile_ReadDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName, 
    WE_INT32 iPos, 
    WE_CHAR *pszNameBuf, 
    WE_INT32 pszNameBufLength, 
    E_WE_FILE_TYPE* eType, 
    WE_LONG *plSize
)
{
    IFileMgr *pIFileMgr = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 i = 0;
    WE_CHAR *pszBrewDirName = NULL;
    FileInfo stInfo = {0};
    WE_INT32 iRes = 0;
    
    if((NULL == hWeHandle)
        ||(NULL == pszDirName)
        ||(NULL == pszNameBuf)
        ||(pszNameBufLength <= 0)
        ||(NULL == eType)
        ||(NULL == plSize))
    {
        return eResult;
    }

    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    
    pIFileMgr = ((St_WeHandle*)hWeHandle)->pIFileMgr;
    pszBrewDirName = WeFile_ConvertPathWeToBrew(pszDirName);
        if(NULL == pszBrewDirName)
        {
            return E_WE_ERROR_RESOURCE_LIMIT;
        }
    
    /*enumerate dirs*/
    iRes = IFILEMGR_EnumInit(pIFileMgr, pszBrewDirName, TRUE);/*TRUE means only enum directory*/
    if(SUCCESS == iRes)
    {
        /*Get the information of dirs in the current dir*/
        while( IFILEMGR_EnumNext(pIFileMgr, &stInfo))
        {
            if(i == iPos)
            {
                /*Get the information of directory in the current dir*/
                WE_CHAR * pName = NULL;
                (void)MEMSET(pszNameBuf, 0, (WE_UINT32)pszNameBufLength);
                pName = STRRCHR((WE_CHAR*)stInfo.szName,'/');
                (void)STRNCPY(pszNameBuf, pName+1 , pszNameBufLength - 1);
                
                *eType = WE_FILE_DIRTYPE;
                *plSize = (WE_LONG)(stInfo.dwSize);
                eResult = E_WE_OK;
                FREE(pszBrewDirName);
                return eResult;
            }
            i ++;
        }
            
    }

    /*enumerate files*/
    iRes = IFILEMGR_EnumInit(pIFileMgr, pszBrewDirName, FALSE);/*FALSE is only for file*/
    if(SUCCESS == iRes)
    {
        /*Get the information of files in the current dir*/
        while(IFILEMGR_EnumNext(pIFileMgr, &stInfo))
        {
            if(i == iPos)
            {
                /*Get the information of directory in the current dir*/
                WE_CHAR * pName = NULL;
                (void)MEMSET(pszNameBuf, 0, (WE_UINT32)pszNameBufLength);
                pName = STRRCHR((WE_CHAR*)stInfo.szName,'/');
                (void)STRNCPY(pszNameBuf, pName+1 , pszNameBufLength - 1);
                
                *eType = WE_FILE_FILETYPE;
                *plSize = (WE_LONG)(stInfo.dwSize);
                eResult = E_WE_OK;              
                FREE(pszBrewDirName);
                return eResult;
            }
            i ++;
        }
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
              FREE(pszBrewDirName);
        return eResult;
    }
    else
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);

              FREE(pszBrewDirName);
        return eResult;
    }

}



/*==================================================================================================
FUNCTION: 
    WeFile_Notify
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    callback.
ARGUMENTS PASSED:
    WE_HANDLE hWeFileHandle: File handle
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void
WeFile_Notify (WE_HANDLE hWeFileHandle)
{
    St_WeHandle* hWeHandle = NULL;    
    Fn_WeCallback cbWeCallback = NULL;    
    void* pCbData = NULL;
    
    hWeHandle = ((St_WeFileHandle*)hWeFileHandle)->hWeHandle;
    cbWeCallback = hWeHandle->cbWeCallback;
    pCbData = hWeHandle->pvPrivData;

    if(NULL != cbWeCallback)
    {
        cbWeCallback(E_WE_EVENT_FILE_READABLE, pCbData, hWeHandle, hWeFileHandle);    
    }
    
    return;
}



/*==================================================================================================
FUNCTION: 
    WeFile_Select
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function is used for registering a event  notify when the file is readable.
ARGUMENTS PASSED:
    WE_HANDLE hWeFileHandle: File handle
    E_WE_EVENT eEventType: File event type
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_PATH 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR
WeFile_Select
(
    WE_HANDLE hWeFileHandle, 
    E_WE_EVENT eEventType
)
{
    IFile* pIFile = NULL;
    
    if((NULL == hWeFileHandle )
        ||(E_WE_EVENT_FILE_READABLE != eEventType))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }    
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }

    pIFile = ((St_WeFileHandle*)hWeFileHandle)->pIFile;
    
    IFILE_Readable(pIFile, WeFile_Notify, hWeFileHandle);
    
    return E_WE_OK;
}



/*==================================================================================================
FUNCTION: 
    WeFile_SetSize
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function sets the file size.
ARGUMENTS PASSED:
    WE_HANDLE hWeFileHandle: File handle
    WE_LONG lSize: Size of file
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_PATH 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR
WeFile_SetSize 
(
    WE_HANDLE hWeFileHandle, 
    WE_LONG lSize
)
{
    St_WeHandle* hWeHandle = NULL;    
    IFileMgr* pIFileMgr = NULL;
    IFile *pIFile = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    FileInfo stFileInfo = {0};
    WE_INT32 iRes = 0;
    
    if(NULL == hWeFileHandle)
    {
        return eResult;
    }
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    hWeHandle = ((St_WeFileHandle*)hWeFileHandle)->hWeHandle;
    pIFileMgr = hWeHandle->pIFileMgr;
    pIFile = ((St_WeFileHandle*)hWeFileHandle)->pIFile;
    
    iRes = IFILE_GetInfo(pIFile, &stFileInfo);
    if(SUCCESS != iRes)
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
        return eResult;
    }

    if(stFileInfo.dwSize > (WE_UINT32)lSize)
    {
        iRes = IFILE_Truncate(pIFile, (WE_UINT32)lSize);
    }
    else
    {
        iRes = IFILE_Seek(pIFile,_SEEK_START ,lSize);
    }
    
    if(SUCCESS == iRes)/*Operate successfully*/
    {
        eResult = E_WE_OK; 
    }
    else
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
    }

    return eResult;
}




/*==================================================================================================
FUNCTION: 
    WeFile_GetSize
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function gets the file size.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    const WE_CHAR *pszFileName: File name
    WE_LONG* plsize: Size of file
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_PATH 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR
WeFile_GetSize 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszFileName,
    WE_LONG* plsize
)
{
    IFileMgr *pIFileMgr = NULL;
    WE_INT32 iRes = 0;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_CHAR *pszBrewDirName = NULL;
    FileInfo stInfo ={0};

    if((NULL == hWeHandle)
        ||(NULL == pszFileName)
        ||(NULL == plsize))
    {
        return eResult;
    }

    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    
    pIFileMgr = ((St_WeHandle*)hWeHandle)->pIFileMgr;
    pszBrewDirName = WeFile_ConvertPathWeToBrew(pszFileName);
        if(NULL == pszBrewDirName)
        {
            return E_WE_ERROR_RESOURCE_LIMIT;
        }
    
    iRes = IFILEMGR_GetInfo (pIFileMgr, pszBrewDirName, &stInfo);
    if(SUCCESS == iRes)
    {
        *plsize = (WE_LONG)(stInfo.dwSize);
        eResult = E_WE_OK;
    }
    else
    {
        iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
    }

    FREE(pszBrewDirName);
    return eResult;
}


/*==================================================================================================
FUNCTION: 
    WeFile_CloseAll
CREATE DATE:
    2006-07-22
AUTHOR:
    Tang
DESCRIPTION: 
    This function closes all files associated with hWeHandle.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_PATH 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
E_WE_ERROR
WeFile_CloseAll 
(
    WE_HANDLE hWeHandle
)
{
    St_WeFileHandle* hWeFileHandle = NULL;
    WE_INT32 i = 0;
    E_WE_ERROR eRes = E_WE_ERROR_INVALID_PARAM;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    
    if(NULL == hWeHandle)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    if(WE_HANDLE_MAGIC_NUM  != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }

    for(i = 0; i < OPEN_FILE_MAX_NUM; i++)
    {
        hWeFileHandle = ((St_WeHandle*)hWeHandle)->aphFileList[i];
        eRes = WeFile_Close(hWeFileHandle);
        if (eRes != E_WE_OK)
        {
            eResult = WeFile_ConvertResultBrewToWe((WE_INT32)eRes);
            return eResult;
        }
    }
    
    return E_WE_OK;
}


/*==================================================================================================
FUNCTION: 
    WeFile_CheckFileExist
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird
DESCRIPTION: 
    This function check that the file is exist or not.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    WE_CHAR *pszFileName: file name
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_PATH 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

E_WE_ERROR
WeFile_CheckFileExist(WE_HANDLE hWeHandle, 
                                const WE_CHAR *pszFileName)
{
    IFileMgr* pIFileMgr = NULL;
    WE_CHAR *pszBrewFileName=NULL;
    E_WE_ERROR eResult = E_WE_OK;
    WE_INT32 iRes = 0;

    
    if((NULL == hWeHandle)||(NULL == pszFileName))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    
    pszBrewFileName = WeFile_ConvertPathWeToBrew(pszFileName);
    pIFileMgr=((St_WeHandle*)hWeHandle)->pIFileMgr;    
    
    iRes = IFILEMGR_Test (pIFileMgr, pszBrewFileName);  
    FREE(pszBrewFileName);
    if(SUCCESS != iRes)
    {
         iRes = IFILEMGR_GetLastError(pIFileMgr);
        eResult = WeFile_ConvertResultBrewToWe(iRes);
        return eResult;
    }
    return E_WE_OK;

    
}


