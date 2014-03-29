#include "ResultScene.h"
#include "AppDelegate.h"
#include "../Title/TitleScene.h"
#include "../../Common/GamePlatform.h"

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

    float scoreBottom = 0;
    for (int i=0; i<scores.size(); i++) {
        auto prefix = "";
        if (scores.size() > 1) {
            prefix = StringUtils::format("%dP ", i + 1).c_str();
        }
        auto scoreLabel = LabelTTF::create(StringUtils::format("%sSCORE: %d", prefix, scores[i]), "Arial", 48);
        scoreLabel->setColor(Color3B(0, 0, 0));
        float labHei = scoreLabel->getContentSize().height;
        float top = (visibleSize.height - labHei * scores.size()) / 2;
        scoreLabel->setPosition(Point(visibleSize.width / 2, visibleSize.height - top - labHei * (i + 0.5f)) + origin);
        addChild(scoreLabel);
        scoreBottom = scoreLabel->getPositionY() - labHei / 2;
    }

    auto tweetButton = AppDelegate::createButton("icon_tw.png", "");
    auto facebookButton = AppDelegate::createButton("icon_fb.png", "");
    auto rankButton = AppDelegate::createButton("icon_rank.png", "");
    rankButton->addTouchEventListener(this, toucheventselector(ResultScene::onRankButtonTouch));

    facebookButton->setPosition(Point(visibleSize.width / 2, scoreBottom - facebookButton->getContentSize().height));
    tweetButton->setPosition(facebookButton->getPosition() + Point(-facebookButton->getContentSize().width - 10, 0));
    rankButton->setPosition(facebookButton->getPosition() + Point(facebookButton->getContentSize().width + 10, 0));

    addChild(tweetButton);
    addChild(facebookButton);
    addChild(rankButton);

    auto okButton = AppDelegate::createButton("button_primary.png", "OK");
    okButton->setPosition(Point(visibleSize.width / 2, okButton->getContentSize().height) + origin);
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

void ResultScene::onRankButtonTouch(Ref* target, TouchEventType type)
{
    if (type != TouchEventType::TOUCH_EVENT_ENDED) {
        return;
    }
    GamePlatform::show();
}
