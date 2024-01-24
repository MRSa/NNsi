/*============================================================================*
 *
 * $Id: global.h,v 1.4 2004/03/08 13:57:31 mrsa Exp $
 *
 *  FILE: 
 *     global.h
 *
 *  Description: 
 *     global variables for NNsh.
 *
 *===========================================================================*/
#ifdef GLOBAL_REAL
  NNshWorkingInfo          *NNshGlobal;          // NNsiグローバル設定
#else
  extern NNshWorkingInfo   *NNshGlobal;          // NNsiグローバル設定
#endif  // #ifdef GLOBAL_REAL
