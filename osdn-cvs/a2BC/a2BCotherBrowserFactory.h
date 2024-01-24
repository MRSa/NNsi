#ifndef A2BCOTHERBROWSERFACTORY_H
#define A2BCOTHERBROWSERFACTORY_H

#include "a2BC_AbstractOtherBrowserParser.h"

/*!
 *  �u���E�U�̊Ǘ��t�@�C��/���O�t�@�C��(dat�t�@�C��)��͗p�N���X�̃|�C���^����������
 * 
 */
class a2BCotherBrowserFactory
{
public:
    a2BCotherBrowserFactory();
    ~a2BCotherBrowserFactory();
   
public:
    a2BC_AbstractOtherBrowserParser *createOtherBrowserFactory(int otherBrowserType);
    void deleteOtherBrowserFactory(a2BC_AbstractOtherBrowserParser *apBrowserParser);    
};

#endif
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
