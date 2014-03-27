#ifndef __puzzlegirl__TitleScene__
#define __puzzlegirl__TitleScene__

#include "cocos2d.h"
#include "UIButton.h"

USING_NS_CC;
using namespace ui;

class TitleScene : public Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(TitleScene);
    void onStartButtonTouch(Object* target, TouchEventType type);
};

#endif /* defined(__puzzlegirl__TitleScene__) */
