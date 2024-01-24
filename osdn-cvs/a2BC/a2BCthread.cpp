#include "a2BCthread.h"

/*!
 *  �R���X�g���N�^
 * 
 */
a2BCthread::a2BCthread(QObject *parent) : QThread(parent)
{
    // �������Ȃ�
}

/*!
 *  �f�X�g���N�^
 * 
 */
a2BCthread::~a2BCthread()
{
    // �������Ȃ�
}

/*!
 *   �X���b�h�̓���
 * 
 */
void a2BCthread::run()
{
    wait();  // �����҂�...
    return;
}

/*! 
 *   �^�C���A�E�g��҂�
 * 
 */
void a2BCthread::setTimer(int id, unsigned long milliSeconds)
{
    // ������Ƃ߂�
    msleep(milliSeconds);
    
    // �^�C���A�E�g�C�x���g�𔭍s����
    emit timeout(id);
    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
