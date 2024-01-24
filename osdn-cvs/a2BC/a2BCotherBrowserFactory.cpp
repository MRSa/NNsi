#include "a2BcConstants.h"
#include "a2BCotherBrowserFactory.h"
#include "pGIKObrowserParser.h"
#include "GikoNaviBrowserParser.h"
#include "janeBrowserParser.h"
#include "a2BbrowserParser.h"

/**
 *   �R���X�g���N�^
 * 
 * 
 */
a2BCotherBrowserFactory::a2BCotherBrowserFactory()
{

}

/**
 *   �f�X�g���N�^
 * 
 */
a2BCotherBrowserFactory::~a2BCotherBrowserFactory()
{

}

/**
 *  ���Q�����˂�u���E�U�̉�̓N���X�ɑ΂���|�C���^����������
 *  (-1�̏ꍇ�́Aa2B����������)
 * 
 */
a2BC_AbstractOtherBrowserParser *a2BCotherBrowserFactory::createOtherBrowserFactory(int otherBrowserType)
{
    if (otherBrowserType < 0)
    {
        return (new a2BbrowserParser());
    }
    if (otherBrowserType == OTHERBROWSER_PGIKO)
    {
        return (new pGIKObrowserParser());
    }
    if (otherBrowserType == OTHERBROWSER_GIKONAVI)
    {
        return (new GikoNaviBrowserParser());
    }
    return (new janeBrowserParser());
}

/**
 *  ���Q�����˂�u���E�U�̉�̓N���X�̃|�C���^���������
 * 
 */
void a2BCotherBrowserFactory::deleteOtherBrowserFactory(a2BC_AbstractOtherBrowserParser *apBrowserParser)
{
    if (apBrowserParser != 0)
    {
        delete apBrowserParser;
    }
    return;
}

/**
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
