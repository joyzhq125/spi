/*=====================================================================================
    HEADER NAME : we_rnd.h
    MODULE NAME : WE

    GENERAL DESCRIPTION
        In this file, define the struct and function prototype. 

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification              Tracking
    Date            Author      Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-12-06       Sam         none      Init
  
=====================================================================================*/
/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef WE_RND_H
#define WE_RND_H


/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeLibPrngInstS St_WeLibPrngInstS;

struct tagSt_WeLibPrngInstS
{
    WE_INT32   (*piSeedFn)(WE_VOID *pvSrc, WE_UINT32 uiLen, St_WeLibPrngInstS *pstPrng);
    WE_INT32   (*piGetFn)(WE_VOID *pvDest, WE_UINT32 uiLen, St_WeLibPrngInstS *pstPrng);
    WE_VOID    (*pvDestroyFn)(St_WeLibPrngInstS *pstPrng);
    WE_VOID    *pvState;
};



/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
St_WeLibPrngInstS  *We_LibPrngCreatBbs(WE_UINT8 *pucParams);





#endif /* endif WE_RND_H */
