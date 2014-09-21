//
//  GameScene.h
//  PuzzleDora
//
//  Created by otmb on 2014/09/20.
//
//

#ifndef __PuzzleDora__GameScene__
#define __PuzzleDora__GameScene__

#include "cocos2d.h"
#include "Block.h"

USING_NS_CC;

class GameScene : public cocos2d::Layer
{
protected:
    enum ZOrder
    {
        Z_Bg = 0,
        Z_Block,
    };
    
    enum Tag
    {
        T_Block = 1,
        T_Block1,
        T_Block2,
        T_Block3,
        T_Block4,
        T_Block5,
    };
    
    float _time;
    void initBackground();
    void initTouchEvent();
    void showBullet();
    
    
    cocos2d::Color3B _tagColor[5]{
        cocos2d::Color3B(0,255,255),
        cocos2d::Color3B(0,0,255),
        cocos2d::Color3B(128,128,128),
        cocos2d::Color3B(255,0,0),
        cocos2d::Color3B(255,255,255)
    };
    //Vector<Block*> _block = {};
    Block* _block[6][6];
    
    Block* _touchBlock;
    bool _checkBlockWaiting;
    
    int _tag[6][6];
    void checkBlock();
    Block* createBlock(int x,int y);
    const float _blocktSize = 46.5f;
    Vec2 getPosition(int x,int y);
    void setTag(int x,int y,int tagNum);
    
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);
    
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* evnet);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* evnet);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* evnet);
    virtual void update(float dt);
};
#endif /* defined(__PuzzleDora__GameScene__) */
