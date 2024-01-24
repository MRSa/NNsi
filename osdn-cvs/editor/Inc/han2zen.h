/*============================================================================*
 *
 * $Id: han2zen.h,v 1.1.1.1 2003/08/15 12:14:30 mrsa Exp $
 *
 *  FILE: 
 *     han2zen.h
 *
 *  Description: 
 *     NNsi Hankaku-Kana to Zenkaku-Kana convertion table.
 *
 *===========================================================================*/
#define HAN2ZEN_H

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   Function : Han2ZenTable                                               */
/*                                          ���p�J�i���S�p�J�i�ϊ��e�[�u�� */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef GLOBAL_REAL

#if 0
    UInt8 *Han2ZenTable[] = {
      "�B", "�u", "�v", "�A", "�E", "��",
      "�@", "�B", "�D", "�F", "�H", "��",
      "��", "��", "�b", "�[", "�A", "�C",
      "�E", "�G", "�I", "�J", "�L", "�N",
      "�P", "�R", "�T", "�V", "�X", "�Z",
      "�\\", "�^", "�`", "�c", "�e", "�g",
      "�i", "�j", "�k", "�l", "�m", "�n",
      "�q", "�t", "�w", "�z", "�}", "�~",
      "��", "��", "��", "��", "��", "��",
      "��", "��", "��", "��", "��", "��",
      "��", "�J", "�K", "",   "",   ""
    };
    /* 0xA1 - 0xDF �̃R�[�h��ϊ����邽�߂̃e�[�u�� */
    /* �������u�\�v�̏ꍇ�͂Q�o�C�g�ڂ��G�X�P�[�v�V�[�P���X��
      ���������邽�߁A�u�\\�v�Ƃ��Ă��� */
#else
    // �����R�[�h��SHIFT-JIS/EUC�ǂ���ł����v�Ȃ悤�ɂP�U�i���ŋL������
    UInt8 *Han2ZenTable[] = {
      // "�B", "�u", "�v", "�A", "�E", "��",
      "\x81\x42", "\x81\x75", "\x81\x76", "\x81\x41", "\x81\x45", "\x83\x92",
      
      // "�@", "�B", "�D", "�F", "�H", "��",
      "\x83\x40", "\x83\x42", "\x83\x44", "\x83\x46", "\x83\x48", "\x83\x83",

      // "��", "��", "�b", "�[", "�A", "�C",
      "\x83\x85", "\x83\x87", "\x83\x62", "\x81\x5b", "\x83\x41", "\x83\x43",

      // "�E", "�G", "�I", "�J", "�L", "�N",
      "\x83\x45", "\x83\x47", "\x83\x49", "\x83\x4a", "\x83\x4c", "\x83\x4e",

      // "�P", "�R", "�T", "�V", "�X", "�Z",
      "\x83\x50", "\x83\x52", "\x83\x54", "\x83\x56", "\x83\x58", "\x83\x5a",

      // "�\\", "�^", "�`", "�c", "�e", "�g",
      "\x83\x5c", "\x83\x5e", "\x83\x60", "\x83\x63", "\x83\x65", "\x83\x67",

      // "�i", "�j", "�k", "�l", "�m", "�n",
      "\x83\x69", "\x83\x6a", "\x83\x6b", "\x83\x6c", "\x83\x6d", "\x83\x6e",

      // "�q", "�t", "�w", "�z", "�}", "�~",
      "\x83\x71", "\x83\x74", "\x83\x77", "\x83\x7a", "\x83\x7d", "\x83\x7e",

      // "��", "��", "��", "��", "��", "��",
      "\x83\x80", "\x83\x81", "\x83\x82", "\x83\x84", "\x83\x86", "\x83\x88",

      // "��", "��", "��", "��", "��", "��",
      "\x83\x89", "\x83\x8a", "\x83\x8b", "\x83\x8c", "\x83\x8d", "\x83\x8f",

      // "��", "�J", "�K", "",   "",   ""
      "\x83\x93", "\x81\x4a", "\x81\x4b", "\x00\x00", "\x00\x00", "\x00\x00"
    };
#endif
#else
    extern UInt8 *Han2ZenTable[];
#endif