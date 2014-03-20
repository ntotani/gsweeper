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

USING_NS_CC;

class TitleScene : public Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(TitleScene);
    void onTouchEnded(Touch* touch, Event* event);
};

#endif /* defined(__puzzlegirl__TitleScene__) */
