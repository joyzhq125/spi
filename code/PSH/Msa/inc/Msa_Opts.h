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

/*! \file maopts.h
 * \brief A liwbary for parsing/encoding command line options.
 */


#ifndef _maopts_H
#define _maopts_H

/*--- Definitions/Declarations ---*/
typedef struct msa_opts_token_s
{
    WE_BOOL                isOption;       /* Is Option parameter, otherwise option data */
    WE_UINT32              numberOfChars;  /* String length of token (parsed chars) */
    WE_UINT32              length;         /* Binary length of token */
    char                    *start;         /* start of token string */
    struct msa_opts_token_s *next;          /* Next token */
} msa_opts_token;

typedef struct msa_opts_list
{
    char                    *line;          /* The command line */
    WE_UINT32             lineLength ;    /* Line buffer length */
    struct msa_opts_token_s *tokenList;     /* List of tokens */
    WE_UINT32             maxLength;      /* Maximum command line length */
} msa_opts_list;

/*!
 *	\brief The first function to call when to parse a command line.
 *  \param   line, Command line to parse
 *  \param   modId, Where to allocate memory.
 *  \return  structure with parsed data.
 */
msa_opts_list *msa_opts_get_list(WE_UINT8 modId, const char *commandLine);

/*!
 *	\brief Free entire args list structure.
 *  \param   list, structure to free.
 *  \param   modId, Where to memory was allocated.
 */
void msa_opts_free_list(WE_UINT8 modId, msa_opts_list *list);

/*!
 *	\brief Find parameter
 *  \param   option, the string command option (-option).
 *  \return  pointer to param.
 */
msa_opts_token *msa_opts_find_option(const msa_opts_list *p, const char *option);

/*!
 * \brief Get next command argument,
 * This is called after msa_opts_find_cmd to
 * get a pointer to the first command argument, and the the next etc.
 * When the last argument was found this function returns NULL.
 *  \param   p, pointer to token list.
 *  \return  pointer to command argument, returns null if not available.
 */
msa_opts_token *msa_opts_get_next_arg(const msa_opts_token *p);

/*!
 *	\brief Get command argument unsigned number value.
 *  \param   p,  pointer to command argument.
 *  \param   ip, integer to store value into (result).
 *  \return  True if successful.
 */
WE_BOOL msa_opts_get_uint32(const msa_opts_token *p, WE_UINT32 *ip);

/*!
 *	\brief Get command argument signed number value.
 *  \param   p, pointer to command argument.
 *  \param   ip, integer to store value into (result).
 *  \return  True if successful.
 */
WE_BOOL msa_opts_get_int32(const msa_opts_token *p, WE_INT32 *ip);

/*!
 *	\brief Get command argument string value.
 *  \param   p, pointer to command argument.
 *  \param   modId, Where to allocate memory.
 *  \param   str, string to store value into (result).
 *  \return  True if successful.
 */
WE_BOOL msa_opts_get_str(WE_UINT8 modId, const msa_opts_token *p, char **str);

/*!
 *	\brief Get command argument binary value.
 *  \param   p, pointer to command argument.
 *  \param   modId, Where to allocate memory.
 *  \param   dp, pointer to allocated binary data (result).
 *  \param   size,  size of allocated binary data (result).
 *  \return  True if successful.
 */
WE_BOOL msa_opts_get_data(WE_UINT8 modId, const msa_opts_token *p, void **dp,
    WE_UINT32 *size);


/*!
 *	\brief The first function to call when to create a command line.
 *  \param   modId, Where to allocate memory.
 *  \paramm  maxLen, Maximum length of command line string to create.
 *  \return  structure with parsed data.
 */
msa_opts_list *msa_opts_create_list(WE_UINT8 modId, WE_UINT32 maxLen);


/*!
 *	\brief Add parameter string.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   option, option name string.
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_option(WE_UINT8 modId, msa_opts_list *p,
    const char *option);

/*!
 *	\brief Set command argument unsigned number value.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   i, integer to add as argument
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_uint32(WE_UINT8 modId, msa_opts_list *p, WE_UINT32 i);


/*!
 *	\brief Add command argument signed number value.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   i, integer to add as argument
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_int32(WE_UINT8 modId, msa_opts_list *p, 
    WE_INT32 i);

/*!
 *	\brief Add command argument string value.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   str, integer to get value from.
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_str(WE_UINT8 modId, msa_opts_list *p, 
    const char *str);

/*!
 *	\brief Add command argument string value.
 *  \param   modId, Where to allocate memory.
 *  \param   p,  pointer to command list.
 *  \param   dp, pointer to binary data.
 *  \param   size,  size of binary data.
 *  \return  True if successful.
 */
WE_BOOL msa_opts_add_data(WE_UINT8 modId, msa_opts_list *p, void *dp,
    WE_UINT32 size);

/*!
 *	\brief Get options line string
 *  \param   p,  pointer to command list.
 *  \param   line_len, length of command line.
 *  \return  Command line string.
 */
const char *msa_opts_get_line(const msa_opts_list *p, WE_UINT32 *line_len);


#endif /* _msa_opts_H */
