#include "a2BCthread.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCthread::a2BCthread(QObject *parent) : QThread(parent)
{
    // 何もしない
}

/*!
 *  デストラクタ
 * 
 */
a2BCthread::~a2BCthread()
{
    // 何もしない
}

/*!
 *   スレッドの動作
 * 
 */
void a2BCthread::run()
{
    wait();  // 無限待ち...
    return;
}

/*! 
 *   タイムアウトを待つ
 * 
 */
void a2BCthread::setTimer(int id, unsigned long milliSeconds)
{
    // 動作をとめる
    msleep(milliSeconds);
    
    // タイムアウトイベントを発行する
    emit timeout(id);
    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
