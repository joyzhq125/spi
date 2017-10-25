/*==================================================================================================
    FILE NAME :
        we_file.c
    MODULE NAME :
        WEA
    GENERAL DESCRIPTION
        This file implements WeFile API.
    TECHFAITH Software Confidential Proprietary(c)
        Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
    Modification Tracking
    Date               Author               Number      Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-10-24    HouGuanhua        none         Initialization
   
==================================================================================================*/


/*==================================================================================================
    Include File Section
==================================================================================================*/

#include "we_cfg.h"
#include "we_def.h"
#include "oem_seclog.h"

#include "oem_sechandle.h"
#include "we_mem.h"
#include "oem_secfile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


       
//#include <errno.h>
//#include <dirent.h> /* readdir */

/*==================================================================================================
* Macro Define Section
==================================================================================================*/
#define WE_FILE_PATH "../shared/"
//#define WE_FILE_PATH "file:/root/workspace/We_File/Debug/shared/we/"
//


/*==================================================================================================
    FUNCTION: 
            DbgPrintFileOpenError
    DESCRIPTION:
            functionName:the function where is is called
            iErrorCode: error code
    RETURN VALUE:    
    REMARK:
==================================================================================================*/
#if 0
#ifdef WE_FILE_LOG_OPEN
static void DbgPrintFileOpenError(WE_INT8 *functionName,WE_INT32  iErrorCode)
{  
    switch( iErrorCode )
    {
    case SUCCESS:
        WE_LOG_MSG("%s, SUCCESS \n",functionName);
        break;
        
    case EFAILED:
        WE_LOG_MSG("%s, EFAILED \n",functionName);
        break;
        
    case EFILEEXISTS:
        WE_LOG_MSG("%s, EFILEEXISTS \n",functionName);
        break;
        
    case EFILENOEXISTS:
        WE_LOG_MSG("%s, EFILENOEXISTS \n",functionName);
        break;
        
    case EDIRNOTEMPTY:
        WE_LOG_MSG("%s, EDIRNOTEMPTY \n",functionName);
        break;
        
    case EBADFILENAME:
        WE_LOG_MSG("%s, EBADFILENAME \n",functionName);
        break;
        
    case EBADSEEKPOS:
        WE_LOG_MSG("%s, EBADSEEKPOS \n",functionName);
        break;
        
    case EFILEEOF:
        WE_LOG_MSG("%s, EFILEEOF \n",functionName);
        break;
        
    case EFSFULL:
        WE_LOG_MSG("%s, EFSFULL \n",functionName);
        break;
        
    case EFILEOPEN:
        WE_LOG_MSG("%s, EFILEOPEN \n",functionName);
        break;
        
    case EOUTOFNODES:
        WE_LOG_MSG("%s, EOUTOFNODES \n",functionName);
        break;
        
    case EDIRNOEXISTS:
        WE_LOG_MSG("%s, EDIRNOEXISTS \n",functionName);
        break;
        
    case EINVALIDOPERATION:
        WE_LOG_MSG("%s, EINVALIDOPERATION \n",functionName);
        break;
        
    case ENOMEDIA:
        WE_LOG_MSG("%s, ENOMEDIA \n",functionName);
        break;
        
    default:
        WE_LOG_MSG("%s, unknown error\n ",functionName);
        break;
    }
    
    WE_LOG_MSG("ErrorCode=%x \n",iErrorCode);
}
#endif
#endif
/*==================================================================================================
    FUNCTION: 
            WeFile_ConvertPathWeToBrew
    DESCRIPTION:
        Add WE and BREW prefix string, and convert to lower case.
    RETURN VALUE:
        Zero terminated WE_CHAR string. 
        NULL, if error.     
    REMARK:
        The parameter pcFileName should be like "file", "/file", "dir/file", "/dir/file", etc.
        The string returned must be freed by user.
==================================================================================================*/
WE_CHAR * WeFile_ConvertPath
(
 const WE_CHAR *pcWeFileName
 )
{
    WE_UINT8 ucFileNameLen = 0;
    WE_CHAR *pcWeName = NULL;
    WE_CHAR *pcEntireName = NULL;
    
    if ( NULL == pcWeFileName )
    {
        return NULL;
    }
    pcWeName = (WE_CHAR*)pcWeFileName;      /* save name pointer */
    
    if ('/' == pcWeName[0])
    {
        pcWeName++;     /*skip "/" */
        
    }
    if ('\0' == pcWeName[0])
    {
        ucFileNameLen = WE_STRLEN(WE_FILE_PATH);
    }
    else 
    {
        ucFileNameLen = (WE_UINT8)( WE_STRLEN(WE_FILE_PATH)+WE_STRLEN(pcWeName) );
    }
    
    pcEntireName = (WE_CHAR *)WE_MALLOC(ucFileNameLen+1);
    if(NULL == pcEntireName)
    {
        return NULL;
    }
    WE_MEMSET(pcEntireName, 0, ucFileNameLen+1);
    WE_STRCPY(pcEntireName, WE_FILE_PATH);
//    if ('\0' != pcWeName[0])
    {
        WE_STRCAT( pcEntireName, pcWeName );
    }
    
    return pcEntireName;
} 



/*==================================================================================================
    FUNCTION: 
        static FileSeekType WeFile_ConvertSeekMode(E_WE_FILE_SEEK_MODE iSeekMode)
    DESCRIPTION:
    RETURN VALUE:
    REMARK:
==================================================================================================*/
static WE_INT32 WeFile_ConvertSeekMode
(
 E_WE_FILE_SEEK_MODE eSeekMode
)
{
    WE_INT32 iSeekType = SEEK_SET;
    
    switch(eSeekMode)
    {
    case WE_FILE_SEEK_SET:
        iSeekType = SEEK_SET;
        break;
    case WE_FILE_SEEK_CUR:
        iSeekType = SEEK_CUR;
        break;
    case WE_FILE_SEEK_END:
        iSeekType = SEEK_END;
        break;
    default:
        break;
    }
    return iSeekType;
}



/*==================================================================================================
    FUNCTION: 
        static E_WE_ERROR WeFile_ConvertResult(WE_INT32 iResult)
    DESCRIPTION:
    RETURN VALUE:
    REMARK:
==================================================================================================*/
static E_WE_ERROR WeFile_ConvertResult
(
 WE_INT32 iResult
)
{
    E_WE_ERROR eReturn = (E_WE_ERROR)-1;
    
    switch(iResult)
    {
    case EACCES: /*The requested access to the file is not allowed, or 
                 search permission is denied for one of the directories in the path 
                 prefix of pathname, or the file did not exist yet and write access 
        to the parent directory is not allowed.*/
    case EROFS: /*pathname refers to a file on a read-only filesystem 
            and write access was requested.*/
    case ETXTBSY: /*pathname refers to an executable image which is currently being executed 
                and write access was requested.*/                                  
        eReturn = E_WE_FILE_ERROR_ACCESS;
        break;
                
    case EEXIST: /*pathname already exists and O_CREAT and O_EXCL were used*/
        
        eReturn = E_WE_FILE_ERROR_EXIST;
        break;
        
    case ENAMETOOLONG: /*pathname was too long.*/
    case EFAULT: /*pathname points outside your accessible address space.*/
    case EBADF: /*fd isn't a valid open file descriptor*/
        //        case EINTR: /*The close() call was interrupted by a signal.*/
    case EIO: /*An I/O error occurred.*/
    case EINVAL: /*invalid option parameter*/
    case EPERM: /*The filesystem does not allow unlinking of files.*/
        eReturn = E_WE_FILE_ERROR_INVALID;
    break;

    case ENFILE: /*The process already has the maximum number of files open*/
    case ENOMEM: /*Insufficient kernel memory was available.*/
    case ELOOP: /*Too many symbolic links were encountered in resolving pathname, 
        or O_NOFOLLOW was specified but pathname was a symbolic link.*/
    case ENOSPC: /*pathname was to be created 
                         but the device containing pathname has no room for the new file.*/   
    case EFBIG: /*An attempt was made to write a file that exceeds the implementation-defined maximum file size 
                 or the process' file size limit, or to write at a position past than the maximum allowed offset.*/
    case EOVERFLOW: /*The resulting file offset cannot be represented in an off_t.*/                              
         eReturn = E_WE_FILE_ERROR_FULL;
         break; 
                             
    case ENOENT: /*O_CREAT is not set and the named file does not exist. 
        Or, a directory component in pathname does not exist or is a dangling symbolic link.*/
    case ENXIO: /*the file is a device special file and no corresponding device exists.*/  
    case EISDIR: /*fd refers to a directory.*/  
    case ENOTDIR: /*A component used as a directory in pathname is not, in fact, a directory.*/        
        eReturn = E_WE_FILE_ERROR_PATH;
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
    DESCRIPTION:
        Creates and/or opens a file with the name pszFileName. 
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
    REMARK:
        The open mode must be one E_WE_FILE_MODE type value.
        The file be opened or created is in the WE's directory, its' name should be like
        "file", "/file", "dir/file", "/dir/file", etc, and the name will be converted to lower case.
==================================================================================================*/
E_WE_ERROR  
WeFile_Open
(
 WE_HANDLE hWeHandle,
 WE_CHAR *pszWeFileName,
 E_WE_FILE_MODE eMode,
 WE_HANDLE *phWeFileHandle
 )
{
    St_WeFileHandle *pstFileHandle = NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    const WE_CHAR *pszFileName = NULL;
    
    WE_CHAR *pcName = NULL;
    WE_CHAR *pcPath = NULL;
    
    WE_INT32 i=0;
    WE_INT32 iError = 0;
    
    
    if((NULL == hWeHandle)
        ||(NULL == pszWeFileName)
        ||(NULL == phWeFileHandle))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    /*find an empty file handle */
    for(i=0; i<OPEN_FILE_MAX_NUM;i++)
    {
        if(NULL == ((St_WeHandle*) hWeHandle)->aphFileList[i])
        {
            break;
        }
    }
    if(OPEN_FILE_MAX_NUM == i)
    {
        return E_WE_FILE_ERROR_FULL;
    }
    
    pstFileHandle = (St_WeFileHandle*)WE_MALLOC(sizeof(St_WeFileHandle));
    if(NULL == pstFileHandle)
    {
        return E_WE_ERROR_RESOURCE_LIMIT;
    }
    pstFileHandle->iMagicNum = WE_FILE_HANDLE_MAGIC_NUM;
    pstFileHandle->hWeHandle = hWeHandle;
    pstFileHandle->iFileHandleID = i;
    pstFileHandle->iFd = -1;
    
    /* convert file name */
    pszFileName = WeFile_ConvertPath(pszWeFileName);
    if(NULL == pszFileName)
    {
        WE_FREE(pstFileHandle);
        return E_WE_ERROR_RESOURCE_LIMIT;
    }
    
    
    
    switch(eMode)
    {
    case WE_FILE_SET_RDONLY:
        pstFileHandle->iFd = (WE_INT32)open( pszFileName, O_RDONLY );
        break;
        
    case WE_FILE_SET_WRONLY:
        pstFileHandle->iFd = (WE_INT32)open( pszFileName, O_WRONLY );
        break;
        
    case WE_FILE_SET_RDWR:
        pstFileHandle->iFd = (WE_INT32)open( pszFileName, O_RDWR );
        break;
        
    case WE_FILE_SET_CREATE:
       #if 0 
        pcName = WE_STRRCHR( pszFileName, '/' );
        if (NULL == pcName)
        {
            return E_WE_FILE_ERROR_PATH;
        }
        pcName++;
        pcPath = (WE_CHAR *)WE_MALLOC( pcName - pszFileName + 1 );
        if (NULL == pcPath)
        {
            return E_WE_ERROR_RESOURCE_LIMIT;
        }
        WE_MEMCPY( pcPath, pszFileName, pcName - pszFileName );
        pcPath[pcName - pszFileName] = '\0';
        eResult = WeFile_MakeDir( hWeHandle, pszWeFileName );
        WE_FREE( pcPath );
        if (E_WE_OK != eResult && eResult != E_WE_FILE_ERROR_EXIST)
        {
            return eResult;
        }
        #endif
        pstFileHandle->iFd = (WE_INT32)open( pszFileName, O_CREAT | O_RDWR, 0777 );
        /*
        if (pstFileHandle->iFd != -1)
        {
        iRet = fchmod(pstFileHandle->iFd, 0777);
        if (-1 == iRet)
        {
        close(pstFileHandle->iFd);
        unlink(pszFileName);
        pstFileHandle->iFd = -1;                                     
        }
    }*/
        break;
        
    case WE_FILE_SET_APPEND:
        pstFileHandle->iFd = (WE_INT32)open( pszFileName, O_APPEND );
        break;
        
    default:
        pstFileHandle->iFd = (WE_INT32)NULL;
        break;
    }
    
    if (-1 == pstFileHandle->iFd)
    {
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
        WE_FREE(pstFileHandle);
    }
    else
    {
        /* save WeFileHandle in WeHandle */
        ((St_WeHandle*) hWeHandle)->aphFileList[i] = pstFileHandle;
        *phWeFileHandle = pstFileHandle;
        eResult = E_WE_OK;
    }
    
    return eResult ;
}



/*==================================================================================================
FUNCTION:
WeFile_Close
DESCRIPTION:
This function closes the file.
RETURN VALUE:
Possible error codes are:
E_WE_OK
successful
E_WE_ERROR_BAD_HANDLE
hWeFileHandle is a disable handle.
E_WE_ERROR_INVALID_PARAM
hWeFileHandle is not a valid handle, or name is not a valid path name for a file.
REMARK:
==================================================================================================*/
E_WE_ERROR
WeFile_Close
(
 WE_HANDLE hWeFileHandle
 )
{
    St_WeHandle* hWeHandle = NULL;
    WE_INT32 iFileHandleID = 0;
    WE_INT32 iFd = (WE_INT32)NULL;
    WE_INT32 iError = 0;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iRet = 0;
    
    if(NULL == hWeFileHandle)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    hWeHandle = (St_WeHandle*)((St_WeFileHandle*)hWeFileHandle)->hWeHandle;
    iFileHandleID = ((St_WeFileHandle*)hWeFileHandle)->iFileHandleID;
    iFd = ((St_WeFileHandle*)hWeFileHandle)->iFd;
    
    ((St_WeFileHandle*)hWeFileHandle)->iMagicNum = 0;
    ((St_WeFileHandle*)hWeFileHandle)->hWeHandle = NULL;
    ((St_WeFileHandle*)hWeFileHandle)->iFileHandleID = -1;
    
    if (0 != iFd)
    {
        iRet = close( iFd );
        if (0 == iRet)
        {
            eResult = E_WE_OK;
        }
        else
        {
            iError = errno;
            eResult = WeFile_ConvertResult( iError );
        }
    }
    WE_FREE( hWeFileHandle );
    hWeFileHandle = NULL;
    /* clear WeFileHandle record in WeHandle */
    hWeHandle->aphFileList[iFileHandleID] = NULL;
    
    return eResult;
}



/*==================================================================================================
    FUNCTION:
        WeFile_Write
    DESCRIPTION:
        This function attempts to write size bytes from the buffer.
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
    REMARK:
        This function writes the specified number of bytes to an open file, the actual writen size
        is return by piWriteNum.
==================================================================================================*/
E_WE_ERROR
WeFile_Write
(
 WE_HANDLE hWeFileHandle,
 void *pvDataBuf,
 WE_INT32 iBufSize,
 WE_INT32 *piWriteNum
 )
{
    WE_INT32 iFd = (WE_INT32)NULL;
    WE_INT32 iError = 0;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;    
    
    if((NULL == hWeFileHandle)
        ||(NULL == pvDataBuf)
        ||(iBufSize<= 0 )
        ||(NULL ==piWriteNum))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    
    if (WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    iFd = ((St_WeFileHandle*)hWeFileHandle)->iFd;
    
    *piWriteNum = (WE_INT32)write( iFd, pvDataBuf, (size_t)iBufSize );
    if (-1 == (*piWriteNum))
    {    
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
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
    DESCRIPTION:
        This function attempts to read a specified number of bytes from an open file.
    RETURN VALUE:
        Possible error codes are:
        E_WE_OK
        successful
        E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file.
    REMARK:
        The actual read size is return by piReadNum.

==================================================================================================*/
E_WE_ERROR
WeFile_Read
(
 WE_HANDLE hWeFileHandle,
 void *pvDataBuf,
 WE_INT32 iBufSize,
 WE_INT32 *piReadNum
 )
{
    WE_INT32 iFd = (WE_INT32)NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iError = 0;
    
    if((NULL == hWeFileHandle)
        ||(NULL == pvDataBuf)
        ||(iBufSize <= 0 )
        ||(NULL == piReadNum))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    iFd = ((St_WeFileHandle*)hWeFileHandle)->iFd;
    
    *piReadNum = read( iFd, pvDataBuf, iBufSize );
    if( -1 == *piReadNum )
    {
        iError = errno;
        eResult = WeFile_ConvertResult( iError );      
    }
    else if (0 == *piReadNum)
    {
        eResult =  E_WE_FILE_ERROR_EOF;
    }
    else
    {
        eResult =  E_WE_OK;
    }
    
    return eResult;
}



/*==================================================================================================
    FUNCTION:
        WeFile_Seek
    DESCRIPTION:
        Moves the file pointer of an open file, allowing random access read and write operations.
        RETURN VALUE:
    Possible error codes are:
        E_WE_OK
        successful
        E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file.
    REMARK:
        Moves the file pointer of an open file, allowing random access read and write operations.
        If the seek operation exceeds the limits of the file an error is returned. Offset may be negative.
==================================================================================================*/
E_WE_ERROR
WeFile_Seek
(
 WE_HANDLE hWeFileHandle,
 WE_INT32 iOffset,
 E_WE_FILE_SEEK_MODE eSeekMode,
 WE_INT32 *piNewPos
 )
{
    WE_INT32 iFd = (WE_INT32)NULL;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iSeekType = 0;
    WE_INT32 iRet = 0;
    WE_INT32 iError = 0;
    
    if ((NULL == hWeFileHandle)
        ||(NULL == piNewPos))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    iFd = ((St_WeFileHandle*)hWeFileHandle)->iFd;
    iSeekType = WeFile_ConvertSeekMode(eSeekMode);
    
    iRet = lseek( iFd, iOffset, SEEK_SET );
    if( -1 == iRet )
    {
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
    }
    else        /* eResult equal to SUCCESS or file pointer position*/
    {
        eResult = E_WE_OK;      
        *piNewPos = iRet;     
    }
    return eResult;
}



/*==================================================================================================
    FUNCTION:
        WeFile_Remove
    DESCRIPTION:
        This function removes the file identified by fileName.
    RETURN VALUE:
        Possible error codes are:
        E_WE_OK
        successful
        E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file.
    REMARK:
        This function removes the file identified by fileName.
==================================================================================================*/
E_WE_ERROR
WeFile_Remove
(
 WE_HANDLE hWeHandle,
 const WE_CHAR *pszWeFileName
 )
{
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT iRet = 0;
    WE_INT32 iError = 0;
    WE_CHAR *pszBrewDirName = NULL;
    
    if ((NULL == hWeHandle)
        ||(NULL == pszWeFileName))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    pszBrewDirName = WeFile_ConvertPath(pszWeFileName);
    iRet = unlink(pszBrewDirName);
    if(0 == iRet)
    {
        eResult = E_WE_OK;
    }
    else
    {
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
    }
    
    return eResult;
}



/*==================================================================================================
    FUNCTION:
        WeFile_MakeDir
    DESCRIPTION:
        This function create a directory name by dirName.
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
      
     REMARK:
         This function creates a directory named by dirName.
==================================================================================================*/
E_WE_ERROR
WeFile_MakeDir
(
 WE_HANDLE hWeHandle,
 const WE_CHAR *pszWeDirName
 )
{
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iRet = 0;
    WE_INT32 iError = 0;
    const WE_CHAR *pcTail = NULL;
    const WE_CHAR *pcTmpDir = NULL;
    WE_CHAR acTmpDir[200] = {0};
    WE_CHAR *pszBrewDirName = NULL;
    if ((NULL == hWeHandle)
        ||(NULL == pszWeDirName))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    pszBrewDirName = WeFile_ConvertPath(pszWeDirName);
    pcTail = pcTmpDir = '/' == pszBrewDirName[0] ? pszBrewDirName + 1 : pszBrewDirName;

    while ((pcTail = WE_STRCHR( pcTail, '/' )) != NULL)
    {
        pcTail++;
        WE_MEMCPY( acTmpDir, pcTmpDir, pcTail - pcTmpDir );
        acTmpDir[pcTail - pcTmpDir] = '\0';
        iRet = mkdir( acTmpDir, 0777); /* mode ?*/
        WE_LOG_MSG((0,0,"acTmpDir is %s iRet is %d\n",acTmpDir,iRet));
        if (-1 == iRet)    
        {
            iError = errno;
            eResult = WeFile_ConvertResult( iError );
            if (eResult != E_WE_FILE_ERROR_EXIST)
            {
                return eResult;
            }
        }
    } 
    /*added by Bird 070323*/
    iRet = mkdir( pcTmpDir, 0777); /* mode ?*/
    WE_LOG_MSG((0,0,"mkdir path is %s ,ret is %d\n",pcTmpDir,iRet));
    if (-1 == iRet)    
    {
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
        if (eResult != E_WE_FILE_ERROR_EXIST)
        {
            return eResult;
        }
    }

    eResult = E_WE_OK;
    return eResult;
}


/*==================================================================================================
    FUNCTION:
        WeFile_ReadDir
    DESCRIPTION:
        This function reads from a directory.
    RETURN VALUE:
        Possible error codes are:
        E_WE_OK
        successful
        E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file.
        E_WE_FILE_ERROR_PATH
    REMARK:
        This function reads from a directory and extracts information about an entry,
        which could be a file or a directory. This function is not recursive, it doesn't read
        the subdirectory of input directory.
        The "iPos" parameter value starts from 0.
==================================================================================================*/
E_WE_ERROR
WeFile_ReadDir
(
 WE_HANDLE hWeHandle,
 const WE_CHAR *pszWeDirName,
 WE_INT32 iPos,
 WE_CHAR *pszNameBuf,
 WE_INT32 pszNameBufLength,
 E_WE_FILE_TYPE* eType, /*not used*/
 WE_INT32 *piSize /*not used*/
 )
{
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    //WE_CHAR *pcName = NULL;
    WE_INT32 iError = 0;
    DIR *pDir = NULL;
    struct dirent *pstDirent = NULL;
    WE_CHAR *pcEntireDirName = NULL;
    off_t pos = 0;
    WE_INT32 i = 0;
    
    if ((NULL == hWeHandle)
        ||(NULL == pszWeDirName)
        ||(NULL == pszNameBuf)
        ||(pszNameBufLength <= 0)
        ||(NULL == eType)
        ||(NULL == piSize))
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    
    pcEntireDirName = WeFile_ConvertPath( pszWeDirName );
    /*enumerate dirs*/
    pDir = opendir( pcEntireDirName );
    if(NULL == pDir)
    {       
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
        return eResult;
    }
    
    for (i = 0; i <= iPos + 2; i++) /* for dir "./" and "../" */
    {
        pstDirent = readdir( pDir );
        pos = telldir(pDir);   
    }
    
    seekdir( pDir, pos );    
    pstDirent = readdir( pDir );  /* need not free? */
    
    if( NULL != pstDirent )
    {
        WE_MEMSET( pszNameBuf, 0, pszNameBufLength );
        sprintf(pszNameBuf, "%s", pstDirent->d_name);
        //WE_MEMCPY(pszNameBuf, pstDirent->d_name , pstDirent->d_reclen );
        closedir(pDir);
        return E_WE_OK;
    }
    else
    {
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
        WE_LOG_MSG((0,0,"WeFile_ReadDir()_____open dir error, errno = %d", eResult));
        closedir(pDir);
        return eResult;
    }
}




/*==================================================================================================
    FUNCTION:
        WeFile_SetSize
    DESCRIPTION:
        This function sets the file size.
    RETURN VALUE:
        Possible error codes are:
        E_WE_OK
        successful
        E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file.
        E_WE_FILE_ERROR_PATH
    REMARK:
        This function sets the size of the file associated with hWeFileHandle to "size", it can increase
        or decrease the file size.
==================================================================================================*/
E_WE_ERROR
WeFile_SetSize
(
 WE_HANDLE hWeFileHandle,
 WE_INT32 iSize
 )
{
    //    St_WeHandle* hWeHandle = NULL;
    WE_INT32 iFd = 0;
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iError = 0;
    WE_INT32 iRet = 0;
    struct stat stStat = {0};
    
    
    if(NULL == hWeFileHandle)
    {
        return E_WE_ERROR_INVALID_PARAM;
    }
    if(WE_FILE_HANDLE_MAGIC_NUM  != ((St_WeFileHandle*)hWeFileHandle)->iMagicNum)
    {
        return E_WE_ERROR_BAD_HANDLE;
    }
    
    //    hWeHandle = ((St_WeFileHandle*)hWeFileHandle)->hWeHandle;
    iFd = ((St_WeFileHandle*)hWeFileHandle)->iFd;
    
    iRet = fstat( iFd, &stStat);
    if (0 == iRet)
    {
        if (stStat.st_size <= iSize)
        {
            iRet = lseek( iFd, iSize, SEEK_SET );
            if (-1 == iRet)
            {
                iError = errno;
                eResult = WeFile_ConvertResult( iError );
            }
            else
            {
                eResult = E_WE_OK;
            }
        }
        else
        {
            iRet = ftruncate( iFd, iSize );
            if (0 == iRet)
            {
                eResult = E_WE_OK;
            }
            else
            {
                iError = errno;
                eResult = WeFile_ConvertResult( iError );
            }
        }
    }
    else
    {
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
    }
    
    return eResult;
}



/*==================================================================================================
    FUNCTION: 
        WeFile_GetSize
    DESCRIPTION:
        This function gets the file size.
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
        successful
        E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_PATH 
    REMARK:
==================================================================================================*/
E_WE_ERROR
WeFile_GetSize 
(
 WE_HANDLE hWeHandle, 
 const WE_CHAR *pszFileName,
 WE_INT32* piSize
 )
{
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iError = 0;
    WE_INT32 iRet = 0;
    struct stat stStat = {0};
    WE_CHAR *pszBrewDirName = NULL;
    
    if ((NULL == hWeHandle)
        ||(NULL == pszFileName)
        ||(NULL == piSize))
    {
        return eResult;
    }
    
    if (WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    /* convert file name */
    pszBrewDirName = WeFile_ConvertPath(pszFileName);
    iRet = stat( pszBrewDirName, &stStat );
    if (0 == iRet)
    {
        *piSize = stStat.st_size;
        eResult = E_WE_OK;
    }
    else
    {
        iError = errno;
        eResult = WeFile_ConvertResult( iError );
    }
    
    return eResult;
}



/*==================================================================================================
    FUNCTION: 
        WeFile_CloseAll
    DESCRIPTION:
        This function closes all files associated with hWeHandle.
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
        successful
        E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_PATH 
    REMARK:
==================================================================================================*/
E_WE_ERROR
WeFile_CloseAll 
(
 WE_HANDLE hWeHandle
 )
{
    St_WeFileHandle* hWeFileHandle = NULL;
    WE_INT32 i;
    
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
        hWeFileHandle = (St_WeFileHandle*)((St_WeHandle*)hWeHandle)->aphFileList[i];
        WeFile_Close(hWeFileHandle);
    }
    
    return E_WE_OK;
}

E_WE_ERROR
WeFile_Select 
(
 WE_HANDLE hWeFileHandle, 
 E_WE_EVENT eEventType
 )
{
    return E_WE_OK;
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
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iRes = 0;
    WE_CHAR *pcEntireDirName = NULL;
    DIR *dp = NULL;
    struct dirent *pstEntry = NULL;
    WE_UINT32 uiEntryCnt = 0;
    struct stat statBuf;
    
    if((NULL == pszDirName)||(NULL == piSize||NULL == hWeHandle))
    {
        return eResult;
    }
    
    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    pcEntireDirName = WeFile_ConvertPath( pszDirName );
    dp = opendir(pcEntireDirName);
    WE_FREE(pcEntireDirName);
    pcEntireDirName = NULL;
    if(NULL == dp)
    {
        iRes = errno;
        eResult = WeFile_ConvertResult( iRes );
        return eResult;
    }   
    chdir(pcEntireDirName);
    while((pstEntry = readdir(dp))!= NULL )
    {
        lstat(pstEntry->d_name,&statBuf);
        if(S_ISDIR(statBuf.st_mode))
        {
            if((strcmp(".",pstEntry->d_name)==0)||(strcmp("..",pstEntry->d_name)==0))
            {
                continue;
            }   
            else
            {
                uiEntryCnt++ ;
            }
        }
        else
        {
            uiEntryCnt++ ;
        }
    }
    *piSize = uiEntryCnt;
    closedir(dp);
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
    E_WE_ERROR eResult = E_WE_ERROR_INVALID_PARAM;
    WE_INT32 iRes = 0;
    WE_CHAR *pcEntireFileName = NULL;
    struct stat statBuf;
    
    if((NULL == pszFileName)||NULL == hWeHandle)
    {
        return eResult;
    }
    
    if(WE_HANDLE_MAGIC_NUM != ((St_WeHandle*)hWeHandle)->iMagicNum)
    {
        eResult = E_WE_ERROR_BAD_HANDLE;
        return eResult;
    }
    pcEntireFileName = WeFile_ConvertPath( pszFileName );
    iRes = stat(pcEntireFileName,&statBuf);
    WE_FREE(pcEntireFileName);
    if(E_WE_OK != iRes)
    {
        eResult = WeFile_ConvertResult( iRes );
        return eResult;
    }
    
    return E_WE_OK; 
    
}


