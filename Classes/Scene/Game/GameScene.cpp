#include "GameScene.h"
#include "AppDelegate.h"
#include "../Result/ResultScene.h"
#include "../Title/TitleScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace ui;

#define TILE_LEN 64
#define ROW_INIT 9
#define COL_INIT 9
#define MINE_NUM_INIT 10
#define ROW_LIMIT 15
#define MINE_NUM_LIMIT 30

Scene* GameScene::createScene(int playerNum)
{
    auto scene = Scene::create();
    GameScene *pRet = new GameScene();
    if (pRet && pRet->initWithPlayerNum(playerNum))
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

bool GameScene::initWithPlayerNum(int playerNum)
{
    if ( !Layer::init() )
    {
        return false;
    }
    ROUND.push_back(pair<int, int>(-1, 0));
    ROUND.push_back(pair<int, int>(-1, 1));
    ROUND.push_back(pair<int, int>(0, 1));
    ROUND.push_back(pair<int, int>(1, 1));
    ROUND.push_back(pair<int, int>(1, 0));
    ROUND.push_back(pair<int, int>(1, -1));
    ROUND.push_back(pair<int, int>(0, -1));
    ROUND.push_back(pair<int, int>(-1, -1));
    row = ROW_INIT;
    col = COL_INIT;
    mineNum = MINE_NUM_INIT;
    currentTurn = 0;
    for (int i=0; i<playerNum; i++) {
        scores.push_back(0);
        droped.push_back(false);
        auto scoreLabel = LabelTTF::create("SCORE: 0", "Arial", 48);
        scoreLabel->setColor(Color3B(0, 0, 0));
        addChild(scoreLabel);
        scoreLabels.push_back(scoreLabel);
    }
    adjustScoreLabel();
    dropBtn = AppDelegate::createButton("button_primary.png", "DROP");
    adjustDropBtn();
    dropBtn->setOpacity(0);
    dropBtn->setVisible(false);
    dropBtn->addTouchEventListener(this, toucheventselector(GameScene::onDropButtonTouch));
    addChild(dropBtn);
    resetTiles();

    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
    audio->preloadEffect("se_coin.mp3");
    audio->preloadEffect("se_ob.mp3");

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    listener->onTouchMoved = [](Touch* touch, Event* event) {};
    listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
    listener->onTouchCancelled = [](Touch* touch, Event* event) {};
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void GameScene::onDropButtonTouch(Ref* target, TouchEventType type)
{
    if (dropBtn->getOpacity() < 1 || type != TouchEventType::TOUCH_EVENT_ENDED) {
        return;
    }
    droped[currentTurn] = true;
    auto ud = UserDefault::getInstance();
    int highScore = ud->getIntegerForKey("highScore", -1);
    if (scores[currentTurn] > highScore) {
        ud->setIntegerForKey("highScore", scores[currentTurn]);
        ud->flush();
    }
    if (dropedAll()) {
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, ResultScene::createScene(scores), Color3B(255, 255, 255)));
    } else {
        stepTurn();
    }
}

void GameScene::onRetryButtonTouch(Ref* target, TouchEventType type)
{
    if (type != TouchEventType::TOUCH_EVENT_ENDED) {
        return;
    }
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameScene::createScene((int)scores.size()), Color3B(255, 255, 255)));
}

void GameScene::onTopButtonTouch(Ref* target, TouchEventType type)
{
    if (type != TouchEventType::TOUCH_EVENT_ENDED) {
        return;
    }
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, TitleScene::createScene(), Color3B(255, 255, 255)));
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point base = Point((visibleSize.width - TILE_LEN * col) / 2, (visibleSize.height - TILE_LEN * row) / 2);
    Point p = touch->getLocation();
    p = (p - base) / TILE_LEN;
    if (!outObBounds(p.y, p.x) && sprites[p.y][p.x] && sprites[p.y][p.x]->getOpacity() == 255) {
        if (tiles[p.y][p.x] == TILE::BOMB) {
            tiles[p.y][p.x] = TILE::SHOWN_BOMB;
            sprites[p.y][p.x]->setTexture("bomb.png");
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("se_ob.mp3");
            scores[currentTurn] = 0;
            droped[currentTurn] = true;
            if (dropedAll()) {
                if (scores.size() == 1) {
                    dropBtn->setOpacity(0);
                    dropBtn->setVisible(false);
                    auto topBtn = AppDelegate::createButton("button_default.png", "TOP");
                    topBtn->setTitleColor(Color3B(0, 0, 0));
                    topBtn->addTouchEventListener(this, toucheventselector(GameScene::onTopButtonTouch));
                    auto retryBtn = AppDelegate::createButton("button_primary.png", "RETRY");
                    retryBtn->addTouchEventListener(this, toucheventselector(GameScene::onRetryButtonTouch));
                    float margin = (visibleSize.width - retryBtn->getContentSize().width - topBtn->getContentSize().width) / 3;
                    topBtn->setPosition(Point(margin + topBtn->getContentSize().width / 2, dropBtn->getPositionY()));
                    retryBtn->setPosition(Point(margin + topBtn->getContentSize().width + margin + retryBtn->getContentSize().width / 2, dropBtn->getPositionY()));
                    addChild(topBtn);
                    addChild(retryBtn);
                } else {
                    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, ResultScene::createScene(scores), Color3B(255, 255, 255)));
                }
            } else {
                adjustScoreLabel();
                stepTurn();
            }
        } else if (tiles[p.y][p.x] == TILE::HIDE) {
            int mine = countMine(p.y, p.x);
            if (mine > 0) {
                scores[currentTurn] += mine;
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("se_coin.mp3");
            }
            parseTile(p.y, p.x);
            adjustScoreLabel();
            if (dropBtn->getOpacity() == 0) {
                dropBtn->setVisible(true);
                dropBtn->runAction(FadeIn::create(0.5f));
            }
            if (openCount >= row * col - mineNum) {
                float duration = 0.5f;
                for (auto ee : sprites) {
                    for (auto e : ee) {
                        if (e) {
                            e->runAction(FadeOut::create(duration));
                        }
                    }
                }
                dropBtn->runAction(Sequence::create(FadeOut::create(duration), CallFunc::create([this]() {
                    dropBtn->setVisible(false);
                    adjustDropBtn();
                }), NULL));
                float oldRate = 1.0f * mineNum / (row * col);
                row = std::min(row + 1, ROW_LIMIT);
                mineNum = std::min((int)(row * col * (oldRate + 0.01f)), MINE_NUM_LIMIT);
                runAction(Sequence::create(DelayTime::create(duration), CallFunc::create([this](){
                    resetTiles();
                }), NULL));
            }
            stepTurn();
        }
    }
}

void GameScene::resetTiles()
{
    for (auto line : sprites) {
        for (auto e : line ) {
            if (e) {
                e->getParent()->removeChild(e);
            }
        }
    }
    tiles.clear();
    sprites.clear();
    openCount = 0;
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    Point base = Point((visibleSize.width - TILE_LEN * col) / 2, (visibleSize.height - TILE_LEN * row) / 2);
    for (int i=0; i<row; i++) {
        tiles.push_back({});
        sprites.push_back({});
        for (int j=0; j<col; j++) {
            auto block = Sprite::create("block.png");
            auto cSize = block->getContentSize();
            block->setScale(TILE_LEN / cSize.width);
            block->setPosition(TILE_LEN * (j + 0.5) + base.x + origin.x, TILE_LEN * (i + 0.5) + base.y + origin.y);
            block->setOpacity(0);
            block->runAction(FadeIn::create(0.5f));
            addChild(block);
            tiles[i].push_back(TILE::HIDE);
            sprites[i].push_back(block);
        }
    }
    for (int i=0; i<mineNum; i++) {
        while(true) {
            int ii = rand() % row;
            int jj = rand() % col;
            if (tiles[ii][jj] == TILE::HIDE) {
                tiles[ii][jj] = TILE::BOMB;
                break;
            }
        }
    }
}

int GameScene::countMine(int i, int j) const
{
    int num = 0;
    for (auto e : ROUND) {
        if (!outObBounds(i + e.first, j + e.second) && (tiles[i + e.first][j + e.second] == BOMB || tiles[i + e.first][j + e.second] == SHOWN_BOMB)) {
            num++;
        }
    }
    return num;
}

bool GameScene::outObBounds(int i, int j) const
{
    return i < 0 || i >= row || j < 0 || j >= col;
}

void GameScene::parseTile(int i, int j) {
    if (outObBounds(i, j) || tiles[i][j] == SHOWN || tiles[i][j] == SHOWN_BOMB) {
        return;
    }
    tiles[i][j] = TILE::SHOWN;
    openCount++;
    int count = countMine(i, j);
    if (count == 0) {
        removeChild(sprites[i][j]);
        sprites[i][j] = nullptr;
        for (auto e : ROUND) {
            parseTile(i + e.first, j + e.second);
        }
    } else {
        sprites[i][j]->setTexture(StringUtils::format("num_%d.png", count));
    }
}

void GameScene::adjustScoreLabel()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    for (int i=0; i<scores.size(); i++) {
        if (scores.size() > 1) {
            scoreLabels[i]->setString(StringUtils::format("%dP SCORE: %d", i + 1, scores[i]));
        } else {
            scoreLabels[i]->setString(StringUtils::format("SCORE: %d", scores[i]));
        }
        auto s = scoreLabels[i]->getContentSize();
        scoreLabels[i]->setPosition(Point(visibleSize.width / 2, visibleSize.height - s.height / 2 - s.height * i) + Point(10, -10) + origin);
    }
}

void GameScene::adjustDropBtn()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    dropBtn->setPosition(Point(visibleSize.width / 2, (visibleSize.height - row * TILE_LEN) / 4.0f) + origin);
}

void GameScene::stepTurn()
{
    while (true) {
        currentTurn = (currentTurn + 1) % scores.size();
        if (!droped[currentTurn]) {
            break;
        }
    }
    for (int i=0; i<scoreLabels.size(); i++) {
        scoreLabels[i]->runAction(ScaleTo::create(0.1f, i == currentTurn ? 1.1f : 0.9f));
    }
}

bool GameScene::dropedAll()
{
    bool ret = true;
    for (bool e : droped) {
        ret &= e;
    }
    return ret;
}
