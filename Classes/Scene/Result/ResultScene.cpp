#include "ResultScene.h"
#include "AppDelegate.h"
#include "../Title/TitleScene.h"

USING_NS_CC;
using namespace ui;

Scene* ResultScene::createScene(std::vector<int> scores)
{
    auto scene = Scene::create();
    ResultScene *pRet = new ResultScene();
    if (pRet && pRet->initWithScores(scores))
    {
        pRet->autorelease();
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
    scene->addChild(pRet);
    return scene;
}

bool ResultScene::initWithScores(std::vector<int> scores)
{
    if ( !Layer::init() )
    {
        return false;
    }
    Size visibleSize = Director::getInstance()->getVisibleSize();
    visibleSize.height -= 100;
    Point origin = Director::getInstance()->getVisibleOrigin();
    origin.y += 100;

    auto ud = UserDefault::getInstance();
    int highScore = ud->getIntegerForKey("highScore");

    for (int i=0; i<scores.size(); i++) {
        auto scoreLabel = LabelTTF::create(StringUtils::format("SCORE: %d", scores[i]), "Arial", 48);
        scoreLabel->setColor(Color3B(0, 0, 0));
        scoreLabel->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2 - scoreLabel->getContentSize().height * i) + origin);
        addChild(scoreLabel);
    }

    auto tweetButton = AppDelegate::createButton("button_twitter.png", "");
    tweetButton->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2 - scores.size() * 48));
    addChild(tweetButton);

    auto facebookButton = AppDelegate::createButton("button_facebook.png", "");
    facebookButton->setPosition(tweetButton->getPosition() + Point(0, -facebookButton->getContentSize().height));
    addChild(facebookButton);

    auto okButton = AppDelegate::createButton("button_primary.png", "OK");
    okButton->setPosition(facebookButton->getPosition() + Point(0, -facebookButton->getContentSize().height));
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
