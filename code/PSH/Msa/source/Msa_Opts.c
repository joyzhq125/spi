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

/*--- Include files ---*/
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Cfg.h"    /* WE: Module definition */
#include "We_Def.h"    /* WE: Global definitions */ 
#include "We_Mem.h"    /* WE: Memory handling */

#include "Msa_Opts.h"

static char *skip_white_space(char *str)
{
    if (str == NULL)
    {
        return NULL;
    }
    
    while((*str) != 0 && ((*str) == ' ' || (*str)== '\t'))
    {
        str++;
    }

    return str;
}

/*
 *	Scans through a token:
 *  - Decides if token is option
 *  - Finds token data start
 *  - Finds token data length
 *  \return the end of 
 */
static char *parseToken(WE_UINT8 modId, char *str, msa_opts_token **res_token)
{
    msa_opts_token  *token = 0;
    WE_BOOL        isString = FALSE;

    (*res_token) = (msa_opts_token*) WE_MEM_ALLOC( modId, sizeof(msa_opts_token));

    token = (*res_token);

    if (token == NULL)
    {
        /*
         *	Error; Memory allocation failed.
         */
        return str;
    }

    memset( token, 0, sizeof(msa_opts_token));
    
    if ((*str) == '-')
    {
        token->isOption = TRUE;
        str++;
    }

    if ((*str) == 0)
    {
        WE_MEM_FREE(modId, token);
        (*res_token) = 0;
        return str;
    }

    if ((*str) == '"')
    {
        isString = TRUE;
        str++;
    }

    token->start = str;

    /*
     *	Scan through token data.
     */
    while ((*str) != 0 &&                                        /* Terminate at end of line */
		( (isString == TRUE && (*str) != '"') ||                 /* Terminate at string end, if string */
          (isString == FALSE && (*str) != ' ' && (*str) != '\t') /* Terminate at white-space, if not string */
          ))
    {
        str++;
    }

    token->length = str-token->start;
    token->next = 0;
    token->numberOfChars = token->length;

    if (isString == TRUE && (*str) == '"')
    {
        str++;
    }
    else if (isString == TRUE)
    {
        WE_MEM_FREE(modId, token);
        (*res_token) = 0;
        return str;        
    }

    return str;
}

/*
 *	The first function to call when to parse a command line.
 *  \param   line, Command line to parse
 *  \param   modId, Where to allocate memory.
 *  \return  structure with parsed data.
 */
msa_opts_list *msa_opts_get_list(WE_UINT8 modId, const char *commandLine)
{
    msa_opts_list   *opts = 0;
    char            *str;
    msa_opts_token  *token = 0;
    msa_opts_token  *last_token = 0;
    
    opts = (msa_opts_list*) WE_MEM_ALLOC( modId, sizeof(msa_opts_list));
    if (opts == NULL)
    {
        return NULL;
    }
    opts->lineLength = strlen(commandLine);
    opts->line = (char*) WE_MEM_ALLOC( modId, opts->lineLength+1);
    if (opts->line == NULL)
    {
        msa_opts_free_list(modId, opts);
        return NULL;
    }
    memset(opts->line, 0, opts->lineLength+1);
    memcpy(opts->line, commandLine, opts->lineLength+1);
    
    opts->tokenList = 0;
    opts->maxLength = opts->lineLength;

    str = opts->line;
    str = skip_white_space(str);
    
    while (str != 0 && (*str) != 0)
    {
        str = parseToken(modId, str, &token);
        
        if (token == 0 && str != 0)
        {
            /*
             *	Error, the token couldn't be parsed.
             */
            msa_opts_free_list(modId, opts);
            
            return NULL;
        }
        if (last_token == 0)
        {
            opts->tokenList = token;
        }
        else
        {
            last_token->next = token;
        }
        last_token = token;
        str = skip_white_space(str);
    }
    
    return opts;    
}


/*
 *	Free entire token list structure.
 *  \param   list, structure to free.
 *  \param   modId, Where to memory was allocated.
 */
void msa_opts_free_tokens(WE_UINT8 modId, msa_opts_token *list)
{
    msa_opts_token *next = list;

    while (list != NULL)
    {
        next = list->next;
        WE_MEM_FREE(modId, list);

        list = next;
    }
}

/*
 *	Free entire opts list structure.
 *  \param   list, structure to free.
 *  \param   modId, Where to memory was allocated.
 */
void msa_opts_free_list(WE_UINT8 modId, msa_opts_list *list)
{
    if (list == NULL)
    {
        return ;
    }

    msa_opts_free_tokens(modId, list->tokenList);
    if (list->line != NULL)
    {
        WE_MEM_FREE(modId, list->line);
    }

    WE_MEM_FREE(modId, list);    
    /* Hmm, maybe should dealloc string also!? */
}

/*
 *	Find parameter
 *  \param   command, the sting command (-command).
 *  \return  pointer to param.
 */
msa_opts_token *msa_opts_find_option(const msa_opts_list *p, const char *option)
{
    msa_opts_token *token = NULL;

    if (p == NULL || p->tokenList == NULL || option == NULL)
    {
        return NULL;
    }

    token = p->tokenList;
    while (token != NULL)
    {
        if (token->isOption == TRUE)
        {
            if (token->start != NULL && (strlen(option) == token->length) && 
                strncmp(option, token->start, token->length) == 0)
            {
                return token;
            }

        }
        token = token->next;
    }

    return NULL;    
}

/*
 * Get next command argument, this is called after msa_opts_find_cmd to
 * get a pointer to the first command argument, and the the next etc.
 * When the last argument was found this function returns NULL.
 *  \param   p, pointer to token list.
 *  \return  pointer to command argument, returns null if not available.
 */
msa_opts_token *msa_opts_get_next_arg(const msa_opts_token *p)
{
    if (p == NULL)
    {
        return NULL;    
    }

    return p->next;
}

/*
 *	Get command argument unsigned number value.
 *  \param   p,  pointer to command argument.
 *  \param   ip, integer to store value into (result).
 *  \return  True if successful.
 */
WE_BOOL msa_opts_get_uint32(const msa_opts_token *p, WE_UINT32 *ip)
{
    if (p == NULL || p->start == NULL)
    {
        return FALSE;
    }
    
    sscanf(p->start, "%d", ip);
    return TRUE;    
}

/*
 *	Get command argument signed number value.
 *  \param   p, pointer to command argument.
 *  \param   ip, integer to store value into (result).
 *  \return  True if successful.
 */
WE_BOOL msa_opts_get_int32(const msa_opts_token *p, WE_INT32 *ip)
{
    WE_INT32 i = 0;
    WE_BOOL  res = FALSE;

    if (p == NULL || p->start == NULL)
    {
        return FALSE;
    }

    res = (sscanf(p->start, "%d", &i) == 1);
    (*ip) = i;

    return res;
}

/*
 *	Get command argument string value.
 *  \param   p, pointer to command argument.
 *  \param   modId, Where to allocate memory.
 *  \param   str, string to store value into (result). Caller deallocates.
 *  \return  TRUE if successful.
 */
WE_BOOL msa_opts_get_str(WE_UINT8 modId, const msa_opts_token *p, char **str)
{
    if (p == NULL || p->start == NULL)
    {
        return FALSE;
    }

    (*str) = (char*) WE_MEM_ALLOC(modId, p->length+1);
    if ((*str) == NULL)
    {
        return FALSE;
    }
    memset((void*) (*str), 0, p->length+1);
    memcpy((void*) (*str), p->start, p->length);

    return TRUE;    
}

/*
 *	Helper function.
 */
static unsigned char msa_opts_get_hex(char c)
{
    if (c >= '0' && c <= '9')
    {
        return (unsigned char) (c - '0');
    }
    if (c >= 'a' && c <= 'f')
    {
        return (unsigned char) (c - 'a'+ 10);
    }
    if (c >= 'A' && c <= 'F')
    {
        return (unsigned char) (c - 'A' + 10);
    }
    return 0;
}
/*
 *	Get command argument binary value.
 *  \param   p, pointer to command argument.
 *  \param   modId, Where to allocate memory.
 *  \param   dp, pointer to allocated binary data (result).
 *  \param   size,  size of allocated binary data (result).
 *  \return  True if successful.
 */
WE_BOOL msa_opts_get_data(WE_UINT8 modId, const msa_opts_token *p, void **dp,
    WE_UINT32 *size)
{
    unsigned char *data = 0;
    char *str = 0;
    WE_UINT32 i;


    if (dp == NULL || p == NULL)
    {
        return FALSE;
    }
    (*dp) = NULL;
    (*size) = 0;
    
    data = (unsigned char*) WE_MEM_ALLOC(modId, p->length/2);
    if (data == NULL)
    {
        return FALSE;
    }

    str = p->start;

    for(i=0; i<(p->length/2); i++)
    {
        data[i] = (unsigned char) ((msa_opts_get_hex(str[0]) << 4) + msa_opts_get_hex(str[1]));
        str += 2;
        (*size)++;
    }

    (*dp) = data;

    return TRUE;    
}


/*
 *	The first function to call when to create a command line.
 *  \param   modId, Where to allocate memory.
 *  \paramm  maxLen, Maximum length of command line string to create.
 *  \return  structure with parsed data.
 */
msa_opts_list *msa_opts_create_list(WE_UINT8 modId, WE_UINT32 maxLen)
{
    msa_opts_list *list = NULL;
    
    list = (msa_opts_list*) WE_MEM_ALLOC(modId, sizeof(msa_opts_list));
    memset(list, 0, sizeof(msa_opts_list));
    if (list == NULL)
    {
        return NULL;
    }

    list->line = (char*) WE_MEM_ALLOC(modId, maxLen);
    if (list->line == NULL)
    {
        WE_MEM_FREE(modId, list);

        return NULL;
    }
    memset(list->line, 0, maxLen);

    list->lineLength = 0;
    list->maxLength = maxLen;
    list->tokenList = NULL;
    
    return list;
}

/*
 *	Add token last in list.
 */
msa_opts_token *msa_opts_add_token(WE_UINT8 modId, msa_opts_list *p)
{
    msa_opts_token *token = NULL;
    msa_opts_token *prev = NULL;
    
    if (p == NULL)
    {
        return NULL;
    }

    token = (msa_opts_token*) WE_MEM_ALLOC(modId, sizeof(msa_opts_token));

    if (token == NULL)
    {
        /*
         *	Out of memory error.
         */
        return FALSE;
    }

    memset( token, 0, sizeof(msa_opts_token));

    token->isOption = FALSE;
    token->length = 0;
    token->next = 0;
    token->numberOfChars = 0;
    token->start = 0;

    if (p->tokenList == NULL)
    {
        p->tokenList = token;
    }
    else
    {
        prev = p->tokenList;
        while(prev->next != NULL)
        {
            prev = prev->next;
        }

        prev->next = token;

        *(p->line+p->lineLength) = ' ';
        p->lineLength++;
    }
    
    return token;
}

/*
 *	Add parameter string.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   param, parameter name string.
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_option(WE_UINT8 modId, msa_opts_list *p, 
    const char *option)
{
    char            *strp = 0;
    msa_opts_token  *token = 0;
    
    if (p == NULL || (p->lineLength + strlen(option)+2) > p->maxLength)
    {
        return FALSE;        
    }
    
    token = msa_opts_add_token(modId, p);
    if (token == NULL)
    {
        return FALSE;
    }
    
    strp = p->line+p->lineLength;
    
    (*strp) = '-';
    strp++;
    token->start = strp;
    
    memcpy(token->start, option, strlen(option));
    strp += strlen(option);
    
    token->length = strp-token->start;
    token->numberOfChars = token->length;
    token->isOption = TRUE;
    
    (*strp) = 0;
    
    p->lineLength = strp-p->line;
    
    return TRUE;
}

/*
 *	Set command argument unsigned number value.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   i, integer to add as argument
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_uint32(WE_UINT8 modId, msa_opts_list *p, WE_UINT32 i)
{
    char            *strp = 0;
    msa_opts_token  *token = 0;
    
    if (p == NULL || (p->lineLength + 7+3) > p->maxLength)
    {
        return FALSE;        
    }
    
    token = msa_opts_add_token(modId, p);
    if (token == NULL)
    {
        return FALSE;
    }
    
    strp = p->line+p->lineLength;
    
    token->start = strp;
    
    sprintf(strp, "%lu", i);
    
    strp += strlen(strp);
    
    token->length = strp-token->start;
    token->numberOfChars = token->length;
    
    (*strp) = 0;
    
    p->lineLength = strp-p->line;
    
    return TRUE;
}


/*
 *	Add command argument signed number value.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   i, integer to add as argument
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_int32(WE_UINT8 modId, msa_opts_list *p, WE_INT32 i)
{
    char            *strp = 0;
    msa_opts_token  *token = 0;
    
    if (p == NULL || (p->lineLength + 7+3) > p->maxLength)
    {
        return FALSE;        
    }
    
    token = msa_opts_add_token(modId, p);
    if (token == NULL)
    {
        return FALSE;
    }
    
    
    strp = p->line+p->lineLength;
    (*strp) = '"';
    strp++;
    
    token->start = strp;

    sprintf(strp, "%lu", i);
    
    strp += strlen(strp);
        
    token->length = strp-token->start;
    token->numberOfChars = token->length;
        

    (*strp) = '"';
    strp++;
    (*strp) = 0;
    
    p->lineLength = strp-p->line;
    
    return TRUE;
}

/*
 *	Add command argument string value.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   str, integer to get value from.
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_str(WE_UINT8 modId, msa_opts_list *p, const char *str)
{
    char            *strp = 0;
    msa_opts_token  *token = 0;
    
    if (p == NULL || (p->lineLength + strlen(str)+3) > p->maxLength)
    {
        return FALSE;        
    }
    
    token = msa_opts_add_token(modId, p);
    if (token == NULL)
    {
        return FALSE;
    }
    
    strp = p->line+p->lineLength;
    
    (*strp) = '"';
    strp++;
    token->start = strp;
    
    memcpy(token->start, str, strlen(str));
    strp += strlen(str);

    token->length = strp-token->start;
    token->numberOfChars = token->length;
    
    (*strp) = '"';
    strp++;

    (*strp) = 0;
    
    p->lineLength = strp-p->line;
    
    return TRUE;
}

/*
 *	Add command argument string value.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   dp, pointer to binary data.
 *  \param   size,  size of binary data.
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_data(WE_UINT8 modId, msa_opts_list *p, void *dp, WE_UINT32 size)
{
    char            *data = (char*) dp;
    char            *str = 0;
    const char      val[17] = "0123456789ABCDEF";
    WE_UINT32      i;
    msa_opts_token  *token = 0;

    if (p == NULL || (p->lineLength + size*2+1) > p->maxLength)
    {
        return FALSE;        
    }

    token = msa_opts_add_token(modId, p);
    if (token == NULL)
    {
        return FALSE;
    }

    str = p->line+p->lineLength;

    token->start = str;

    for(i=0; i<size; i++)
    {
        (*str) = val[(data[i]>>4)&15];
        str++;
        (*str) = val[data[i]&15];
        str++;
    }
    (*str) = 0;

    token->length = token->start-str;
    token->numberOfChars = token->length;

    p->lineLength = str-p->line;
    
    return TRUE;
}

/*
 *	Get command line string
 *  \param   p,  pointer to command list.
 *  \param   line_len, length of command line.
 *  \return  Command line string.
 */
const char *msa_opts_get_line(const msa_opts_list *p, WE_UINT32 *line_len)
{
    if (p == NULL)
    {
        (*line_len) = 0;
        return NULL;
    }

    (*line_len) = p->lineLength;
    
    return p->line;
}
