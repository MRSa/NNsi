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
  NNshWorkingInfo          *NNshGlobal;          // NNsi�O���[�o���ݒ�
#else
  extern NNshWorkingInfo   *NNshGlobal;          // NNsi�O���[�o���ݒ�
#endif  // #ifdef GLOBAL_REAL
