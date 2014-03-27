#include "ResultScene.h"
#include "AppDelegate.h"
#include "../Title/TitleScene.h"

USING_NS_CC;
using namespace ui;

Scene* ResultScene::createScene()
{
    auto scene = Scene::create();
    auto layer = ResultScene::create();
    scene->addChild(layer);
    return scene;
}

bool ResultScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    auto ud = UserDefault::getInstance();
    int score = ud->getIntegerForKey("lastScore");
    int highScore = ud->getIntegerForKey("highScore");

    auto scoreLabel = LabelTTF::create(StringUtils::format("SCORE: %d", score), "Arial", 48);
    scoreLabel->setColor(Color3B(0, 0, 0));
    scoreLabel->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2) + origin);
    addChild(scoreLabel);

    auto tweetButton = AppDelegate::createButton("button_twitter.png", "");
    tweetButton->setPosition(scoreLabel->getPosition() + Point(0, -scoreLabel->getContentSize().height) + origin);
    addChild(tweetButton);

    auto facebookButton = AppDelegate::createButton("button_facebook.png", "");
    facebookButton->setPosition(tweetButton->getPosition() + Point(0, -facebookButton->getContentSize().height) + origin);
    addChild(facebookButton);

    auto okButton = AppDelegate::createButton("button_primary.png", "OK");
    okButton->setPosition(facebookButton->getPosition() + Point(0, -facebookButton->getContentSize().height) + origin);
    okButton->addTouchEventListener(this, toucheventselector(ResultScene::onOkButtonTouch));
    addChild(okButton);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    listener->onTouchMoved = [](Touch* touch, Event* event) {};
    listener->onTouchEnded = CC_CALLBACK_2(ResultScene::onTouchEnded, this);
    listener->onTouchCancelled = [](Touch* touch, Event* event) {};
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void ResultScene::onTouchEnded(Touch* touch, Event* event) {
}

void ResultScene::onOkButtonTouch(Ref* target, TouchEventType type)
{
    if (type != TouchEventType::TOUCH_EVENT_ENDED) {
        return;
    }
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, TitleScene::createScene(), Color3B(255, 255, 255)));
}
