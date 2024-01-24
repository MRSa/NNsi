/*
 *  a2BC : a2B Connectivity  (�萔�̒�`)
 *
 */
#ifndef A2BC_CONSTANTS_H
#define A2BC_CONSTANTS_H

enum status_ui_enum
{
    STATUS_INITIAL,
    STATUS_NORMAL,
    STATUS_BUTTOM
};

// �X����Ԃ̒�`�B
const int A2BCSTATUS_NEW      = 1;   // �V�K�擾
const int A2BCSTATUS_UPDATE   = 2;   // �X�V�擾
const int A2BCSTATUS_REMAIN   = 3;   // ���ǂ���
const int A2BCSTATUS_ALREADY  = 4;   // ����
const int A2BCSTATUS_RESERVE  = 5;   // �擾�\��
const int A2BCSTATUS_UNKNOWN  = 6;   // �X����ԕs��
const int A2BCSTATUS_GETERROR = 7;
const int A2BCSTATUS_OVER     = 8;
const int A2BCSTATUS_NOTYET   = 10;  // ���擾
const int A2BCSTATUS_DEL_OFFL = 20;

const int OTHERBROWSER_A2B      = -1;  // a2B�^�C�v
const int OTHERBROWSER_PGIKO    = 0;   // �ۂ��M�R�^�C�v
const int OTHERBROWSER_JANE     = 1;   // Jane�^�C�v
const int OTHERBROWSER_GIKONAVI = 2;   // �M�R�i�r�^�C�v
const int OTHERBROWSER_ABONE    = 3;   // ABone�^�C�v
const int OTHERBROWSER_HZN2     = 4;   // HZN2�^�C�v
const int OTHERBROWSER_V2C      = 5;   // V2C�^�C�v

const int SYNC_ALL              = 0;
const int SYNC_TO_a2B           = 1;    // other �� a2B
const int SYNC_FROM_a2B         = 2;    // a2B   �� other

#endif  // #ifndef A2BC_CONSTANTS_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
