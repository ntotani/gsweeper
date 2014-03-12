//
//  TitleScene.h
//  puzzlegirl
//
//  Created by n_totani on 2014/03/12.
//
//

#ifndef __puzzlegirl__TitleScene__
#define __puzzlegirl__TitleScene__

#include "cocos2d.h"

class TitleScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(TitleScene);
};

#endif /* defined(__puzzlegirl__TitleScene__) */
