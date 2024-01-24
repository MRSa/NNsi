/*============================================================================*
 *  FILE: 
 *     global.h
 *
 *  Description: 
 *     global variables for NNsh.
 *
 *===========================================================================*/
#ifdef GLOBAL_REAL
  NNshSavedPref            *NNshParam;         // NNsi設定
  NNshWorkingInfo          *NNshGlobal;        // NNsiグローバル設定
#else
  extern NNshSavedPref     *NNshParam;         // NNsi設定
  extern NNshWorkingInfo   *NNshGlobal;        // NNsiグローバル設定
#endif  // #ifdef GLOBAL_REAL
