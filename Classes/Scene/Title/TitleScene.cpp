#include "TitleScene.h"
#include "../Game/GameScene.h"
#include "AppDelegate.h"

USING_NS_CC;
using namespace ui;

Scene* TitleScene::createScene()
{
    auto scene = Scene::create();
    auto layer = TitleScene::create();
    scene->addChild(layer);
    return scene;
}

bool TitleScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    Size visibleSize = Director::getInstance()->getVisibleSize();
    visibleSize.height -= 100;
    Point origin = Director::getInstance()->getVisibleOrigin();
    origin.y += 100;

    auto label = LabelTTF::create("GREED SWEEPER", "Arial", 64);
    label->setColor(Color3B(0, 0, 0));
    label->setPosition(Point(origin.x + visibleSize.width/2,
                             origin.y + visibleSize.height - label->getContentSize().height));
    this->addChild(label);

    auto tutorial = Sprite::create("tutorial.png");
    tutorial->setPosition(Point(visibleSize) / 2 + origin);
    addChild(tutorial);

    auto startButton = AppDelegate::createButton("button_primary.png", "START");
    startButton->setPosition(tutorial->getPosition() + Point(0, -300));
    startButton->addTouchEventListener(this, toucheventselector(TitleScene::onStartButtonTouch));
    addChild(startButton);

    int highScore = UserDefault::getInstance()->getIntegerForKey("highScore", -1);
    if (highScore > 0) {
        auto scoreLabel = LabelTTF::create(StringUtils::format("HIGH SCORE: %d", highScore), "Arial", 48);
        scoreLabel->setColor(Color3B(0, 0, 0));
        scoreLabel->setPosition(label->getPosition() + Point(0, -label->getContentSize().height));
        addChild(scoreLabel);
        
        auto multiButton = AppDelegate::createButton("button_primary.png", "MULTI");
        multiButton->setPosition(startButton->getPosition() + Point(0, -72));
        multiButton->addTouchEventListener(this, toucheventselector(TitleScene::onMultiButtonTouch));
        addChild(multiButton);
    }

    return true;
}

void TitleScene::onStartButtonTouch(Ref* target, TouchEventType type)
{
    if (type == TouchEventType::TOUCH_EVENT_ENDED) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameScene::createScene(1), Color3B(255, 255, 255)));
    }
}

void TitleScene::onMultiButtonTouch(Ref* target, TouchEventType type)
{
    if (type == TouchEventType::TOUCH_EVENT_ENDED) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameScene::createScene(2), Color3B(255, 255, 255)));
    }
}
