/*
 *  a2BC : a2B Connectivity  (定数の定義)
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

// スレ状態の定義。
const int A2BCSTATUS_NEW      = 1;   // 新規取得
const int A2BCSTATUS_UPDATE   = 2;   // 更新取得
const int A2BCSTATUS_REMAIN   = 3;   // 未読あり
const int A2BCSTATUS_ALREADY  = 4;   // 既読
const int A2BCSTATUS_RESERVE  = 5;   // 取得予約
const int A2BCSTATUS_UNKNOWN  = 6;   // スレ状態不明
const int A2BCSTATUS_GETERROR = 7;
const int A2BCSTATUS_OVER     = 8;
const int A2BCSTATUS_NOTYET   = 10;  // 未取得
const int A2BCSTATUS_DEL_OFFL = 20;

const int OTHERBROWSER_A2B      = -1;  // a2Bタイプ
const int OTHERBROWSER_PGIKO    = 0;   // ぽけギコタイプ
const int OTHERBROWSER_JANE     = 1;   // Janeタイプ
const int OTHERBROWSER_GIKONAVI = 2;   // ギコナビタイプ
const int OTHERBROWSER_ABONE    = 3;   // ABoneタイプ
const int OTHERBROWSER_HZN2     = 4;   // HZN2タイプ
const int OTHERBROWSER_V2C      = 5;   // V2Cタイプ

const int SYNC_ALL              = 0;
const int SYNC_TO_a2B           = 1;    // other → a2B
const int SYNC_FROM_a2B         = 2;    // a2B   → other

#endif  // #ifndef A2BC_CONSTANTS_H
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
