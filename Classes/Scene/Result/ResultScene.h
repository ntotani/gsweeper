#ifndef __gsweeper__ResultScene__
#define __gsweeper__ResultScene__

#include "cocos2d.h"
#include "UIButton.h"

USING_NS_CC;
using namespace ui;

class ResultScene : public Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(ResultScene);
    void onTouchEnded(Touch* touch, Event* event);
    void onOkButtonTouch(Object* target, TouchEventType type);
};

#endif /* defined(__gsweeper__ResultScene__) */
