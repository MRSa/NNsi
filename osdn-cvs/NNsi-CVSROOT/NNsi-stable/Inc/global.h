/*============================================================================*
 *  FILE: 
 *     global.h
 *
 *  Description: 
 *     global variables for NNsh.
 *
 *===========================================================================*/
#ifdef GLOBAL_REAL
  NNshWorkingInfo          *NNshGlobal;          // NNsi�O���[�o���ݒ�
#else
  extern NNshWorkingInfo   *NNshGlobal;          // NNsi�O���[�o���ݒ�
#endif  // #ifdef GLOBAL_REAL
