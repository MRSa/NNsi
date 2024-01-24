/*============================================================================*
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
