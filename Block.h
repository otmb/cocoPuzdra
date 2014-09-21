//
//  Block.h
//  PuzzleDora
//
//  Created by otmb on 2014/09/20.
//
//

#ifndef __PuzzleDora__Block__
#define __PuzzleDora__Block__

#include "cocos2d.h"

USING_NS_CC;

class Block : public cocos2d::Sprite
{
public:
    
    // 弾の状態
    enum class State
    {
        Stop,
        Moving, // 移動中
        Broken, // 壊れる
    };
    
    Block();
    static Block* create();
    virtual bool init();
    
    CC_SYNTHESIZE(State, _state, State); // 弾の状態
    CC_SYNTHESIZE(float, _radius, Radius); // 当たり判定用の半径
    
    void brokenBlock(); // 弾が壊れる
    void setRadius();
    float const blocktSize = 45.0f;
    
    int _tagX;
    int _tagY;
    int _tag;
    void moveBlock(Vec2& pos);
};


#endif /* defined(__PuzzleDora__Block__) */
