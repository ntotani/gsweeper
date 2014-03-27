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

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    auto layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
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
    score = 0;

    scoreLabel = LabelTTF::create("SCORE: 0", "Arial", 48);
    scoreLabel->setColor(Color3B(0, 0, 0));
    adjustScoreLabel();
    addChild(scoreLabel);
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
    if (dropBtn->getOpacity() < 1) {
        return;
    }
    if (type == TouchEventType::TOUCH_EVENT_ENDED) {
        auto ud = UserDefault::getInstance();
        ud->setIntegerForKey("lastScore", score);
        int highScore = ud->getIntegerForKey("highScore", -1);
        if (score > highScore) {
            ud->setIntegerForKey("highScore", score);
        }
        ud->flush();
        Director::getInstance()->replaceScene(TransitionFade::create(0.5f, ResultScene::createScene(), Color3B(255, 255, 255)));
    }
}

void GameScene::onRetryButtonTouch(Ref* target, TouchEventType type)
{
    if (type != TouchEventType::TOUCH_EVENT_ENDED) {
        return;
    }
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, GameScene::createScene(), Color3B(255, 255, 255)));
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
            sprites[p.y][p.x]->setTexture("bomb.png");
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("se_ob.mp3");
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
            int mine = countMine(p.y, p.x);
            if (mine > 0 && tiles[p.y][p.x] != SHOWN) {
                score += mine;
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
                log("%d, %d, %d", row, col, mineNum);
                runAction(Sequence::create(DelayTime::create(duration), CallFunc::create([this](){
                    resetTiles();
                }), NULL));
            }
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
        if (!outObBounds(i + e.first, j + e.second) && tiles[i + e.first][j + e.second] == TILE::BOMB) {
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
    if (outObBounds(i, j) || tiles[i][j] == SHOWN) {
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
    scoreLabel->setString(StringUtils::format("SCORE: %d", score));
    scoreLabel->setPosition(Point(scoreLabel->getContentSize().width / 2, visibleSize.height - scoreLabel->getContentSize().height / 2) + Point(10, -10) + origin);
}

Node* GameScene::createInfoNode() const
{
    auto node = Node::create();
    auto nextLabel = LabelTTF::create("NEXT", "", 48);
    nextLabel->setColor(Color3B(0, 0, 0));
    auto iconTile = Sprite::create("block.png");
    iconTile->setPosition(0, -nextLabel->getContentSize().height);
    auto tileNumLabel = LabelTTF::create(StringUtils::format("%d x %d", col, row), "", 48);
    tileNumLabel->setColor(Color3B(0, 0, 0));
    tileNumLabel->setPosition((iconTile->getContentSize().width + tileNumLabel->getContentSize().width) / 2 + 10, -nextLabel->getContentSize().height);
    auto iconBomb = Sprite::create("bomb.png");
    iconBomb->setPosition(0, iconTile->getPositionY() - iconTile->getContentSize().height - 10);
    auto mineNumLabel = LabelTTF::create(StringUtils::format("%d", mineNum), "", 48);
    mineNumLabel->setColor(Color3B(0, 0, 0));
    mineNumLabel->setPosition((iconBomb->getContentSize().width + mineNumLabel->getContentSize().width) / 2 + 10, iconBomb->getPositionY());
    node->addChild(nextLabel);
    node->addChild(iconTile);
    node->addChild(tileNumLabel);
    node->addChild(iconBomb);
    node->addChild(mineNumLabel);
    /*
    float width = nextLabel->getContentSize().width / 2 + tileNumLabel->getPositionX() + tileNumLabel->getContentSize().width / 2;
    float height = nextLabel->getContentSize().height / 2 - iconBomb->getPositionY() + iconBomb->getContentSize().height / 2;
    */
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    node->setPosition(Point(visibleSize) / 2 + origin);
    return node;
}

void GameScene::adjustDropBtn()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    dropBtn->setPosition(Point(visibleSize.width / 2, (visibleSize.height - row * TILE_LEN) / 4.0f) + origin);
}
