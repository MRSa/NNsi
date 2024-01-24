#include "a2BcConstants.h"
#include "a2BCotherBrowserFactory.h"
#include "pGIKObrowserParser.h"
#include "GikoNaviBrowserParser.h"
#include "janeBrowserParser.h"
#include "a2BbrowserParser.h"

/**
 *   コンストラクタ
 * 
 * 
 */
a2BCotherBrowserFactory::a2BCotherBrowserFactory()
{

}

/**
 *   デストラクタ
 * 
 */
a2BCotherBrowserFactory::~a2BCotherBrowserFactory()
{

}

/**
 *  他２ちゃんねるブラウザの解析クラスに対するポインタを応答する
 *  (-1の場合は、a2Bを応答する)
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
 *  他２ちゃんねるブラウザの解析クラスのポインタを解放する
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
