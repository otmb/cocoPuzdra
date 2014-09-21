//
//  Block.cpp
//  PuzzleDora
//
//  Created by otmb on 2014/09/20.
//
//

#include "Block.h"

USING_NS_CC;

//コンストラクタ
Block::Block()
: _state(State::Stop)
{
}


Block* Block::create()
{
    
    auto node = new Block();
    node->init();
    node->autorelease();
    
    return node;
}

bool Block::init()
{
    if (!Sprite::initWithFile("ball.png")){
        return false;
    }
    return true;
    
}

// ブロックの半径
void Block::setRadius()
{
    _radius = getContentSize().width * getScale() * 0.5;
}

void Block::moveBlock(Vec2& pos)
{
    _state = State::Moving;
    auto move = MoveTo::create(0.2f, pos);
    auto func = CallFunc::create([&]{
        _state = State::Stop;
    });
    
    auto seq = Sequence::create(move,func,nullptr);
    runAction(seq);
}

void Block::brokenBlock()
{
    stopAllActions();
    
    // 弾を消すアクションを起こす
    Action* action = RemoveSelf::create();
    runAction(action);
    
    _state = State::Broken;
}