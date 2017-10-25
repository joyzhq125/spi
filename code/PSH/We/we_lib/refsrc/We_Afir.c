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
#include "We_Afir.h"
#ifdef WE_CFG_AFI_REDIRECT_INDEX
#include "We_Lib.h"
#include "We_Cmmn.h"
#include "We_File.h"
#include "We_Mem.h"


/**********************************************************************
 * Local functions
 **********************************************************************/

/* Create directories recursive */
static int
makeRecursiveDir (char *p) {

  char *stop;
  int   res = 0;
  int   len = strlen (p);

  if (p[len - 1] == '/') {
    p[len - 1] = '\0';
  }
  stop = p;

  do {
    stop  = strchr (stop + 1, '/');
    if (stop != NULL) {
      *stop = '\0';
    }
    res = TPIa_fileMkDir (p);
    if (!((res == WE_FILE_ERROR_EXIST) || (res == WE_FILE_OK))) {
      return res;
    }
    if (stop != NULL) {
      *stop = '/';
    }
  } while (stop != NULL);

  return res;
}

/* Lowercase version of strstr */
char *
we_afir_strstr_lc(WE_UINT8 modId, const char *s1, const char *s2)
{

  char *dst1        = NULL;
  char *dst2        = NULL;
  char *result_dst1 = NULL;
  char *result_s1   = NULL;
  
  if ((s1 != NULL) && (s2 != NULL)) {
    dst1 = (char*)WE_MEM_ALLOC (modId, strlen(s1)+1);
    dst2 = (char*)WE_MEM_ALLOC (modId, strlen(s2)+1);

    we_cmmn_strcpy_lc (dst1, s1);
    we_cmmn_strcpy_lc (dst2, s2);

    result_dst1 = strstr (dst1, dst2);

    if(result_dst1 != NULL) {
      /* Pointer to first occurence in original string */
      result_s1 = ((char*)s1 + (result_dst1 - dst1));
    }
    
    WE_MEM_FREE(modId, dst1);
    WE_MEM_FREE(modId, dst2);
  }

  return result_s1;
}


/**********************************************************************
 * Global functions
 **********************************************************************/

char * 
we_afir_idx_redirect_path(WE_UINT8 modId, const char *path) {

  const char *append_path = "/index_data";
  const char *external    = "/external/c:usr_data";
  char       *index_path;
  int         result = TPI_FILE_OK;
  
  /* First check if path is NULL */
  if(path == NULL) {
    return NULL;
  }

  /* Check if memory card */
  if (we_afir_strstr_lc(modId, path, external) == path) {
    /* Just return path for memorycard */
    index_path = we_cmmn_strdup (modId, path);    
  }
  else {
  /* Concatenate to new path for index file storing */
    index_path = we_cmmn_strcat (modId, append_path, path);
  }

  /* Check if structure exist in file system otherwise */
  /* create it.                                        */
  if (WE_FILE_GETSIZE_DIR (index_path) != WE_FILE_OK) {
    result = makeRecursiveDir (index_path);
  }

  if (result != WE_FILE_OK && result != WE_FILE_ERROR_EXIST) {
    WE_MEM_FREE (modId, index_path);
    return NULL;
  }
  /* Append "/" for folder */
  return we_cmmn_strcat (modId, (const char*)index_path, "/");
}
#endif /* WE_CFG_AFI_REDIRECT_INDEX */
