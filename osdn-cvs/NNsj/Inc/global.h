/*============================================================================*
 *  FILE: 
 *     global.h
 *
 *  Description: 
 *     global variables for NNsh.
 *
 *===========================================================================*/
#ifdef GLOBAL_REAL
  NNshSavedPref            *NNshParam;         // NNsi�ݒ�
  NNshWorkingInfo          *NNshGlobal;        // NNsi�O���[�o���ݒ�
#else
  extern NNshSavedPref     *NNshParam;         // NNsi�ݒ�
  extern NNshWorkingInfo   *NNshGlobal;        // NNsi�O���[�o���ݒ�
#endif  // #ifdef GLOBAL_REAL
