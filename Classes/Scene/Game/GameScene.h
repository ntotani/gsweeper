#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "UIButton.h"

using namespace std;
USING_NS_CC;
using namespace ui;

enum TILE {
    HIDE = 0,
    SHOWN,
    BOMB,
};

class GameScene : public Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);
    void onTouchEnded(Touch* touch, Event* event);
    vector< pair<int, int> > ROUND;
    void onDropButtonTouch(Object* target, TouchEventType type);
    void onRetryButtonTouch(Object* target, TouchEventType type);
    void onTopButtonTouch(Object* target, TouchEventType type);
private:
    int row;
    int col;
    int mineNum;
    vector< vector<TILE> > tiles;
    vector< vector<Sprite*> > sprites;
    int openCount;
    int score;
    LabelTTF* scoreLabel;
    Button* dropBtn;

    void resetTiles();
    int countMine(int i, int j) const;
    bool outObBounds(int i, int j) const;
    void parseTile(int i, int j);
    void adjustScoreLabel();
    Node* createInfoNode() const;
    void adjustDropBtn();
};

#endif // __GAME_SCENE_H__
