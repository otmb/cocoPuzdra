//
//  GameScene.cpp
//  PuzzleDora
//
//  Created by otmb on 2014/09/20.
//
//

#include "GameScene.h"
#include "Block.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    auto layer = GameScene::create();
    scene->addChild(layer);
    
    return scene;
}

// 初期化
bool GameScene::init()
{
    
    if (!Layer::init())
        return false;
    
    initBackground();
    initTouchEvent();
    scheduleUpdate();
    
    int x = 0;
    int max = 36;
    
    for (int _y = 0; _y < max; ++_y) {
        auto y = _y%6;
        auto* block = createBlock(x,y);
        addChild(block,Z_Block,T_Block);
        _block[x][y] = block;
        
        if (y == 5) x++;
    }
    checkBlock();
    return true;
}

Block* GameScene::createBlock(int x,int y){
    
    auto* block = Block::create();
    block->setScale(0.13f * _blocktSize);
    block->setPosition(getPosition(x,y));
    
    int tagNum = arc4random()%5;
    /*
    int test[6][6] = {
        {1,1,1,2,2,2},
        {1,3,3,3,3,3},
        {1,1,1,1,1,1},
        {1,1,1,1,1,1},
        {1,1,1,1,1,1},
        {1,1,1,1,1,1},
    };
    tagNum = test[x][y];
    */
    
    block->setColor(_tagColor[tagNum]);
    block->setRadius();
    
    _tag[x][y] = tagNum;
    block->_tagX = x;
    block->_tagY = y;
    block->_tag = tagNum;
    
    return block;
}

Vec2 GameScene::getPosition(int x,int y){
    
    float margin = 12.0f;
    
    return Vec2(x*_blocktSize * 2 + margin * x + _blocktSize / 2 + 34
                       , y*_blocktSize * 2 + margin * y + _blocktSize / 2 + 40);
}

void GameScene::initBackground()
{
    // 画面サイズ取得
    Size winSize = Director::getInstance()->getWinSize();
    
    // 背景の生成
    auto background = Sprite::create("Background.png");
    
    // 背景のアンカーポイント設定
    background->setAnchorPoint(Vec2(0.5,0));
    
    background->setPosition(Vec2(winSize.width / 2 , 0));
    
    // 背景の位置設定
    //background->setPosition(winSize /2);
    
    // 背景の追加
    addChild(background, Z_Bg);
    
    // アニメーションの作成
    auto movePosition = Vec2(winSize.width / 2, winSize.height - background->getContentSize().height);
    
    auto move = MoveTo::create(120 , movePosition);
    
    // アニメーションの実行
    background->runAction(move);
}

void GameScene::update(float dt)
{
    _time += dt;
    
    if (_checkBlockWaiting == true){
        bool isMoved = false;
        for (auto x = 0; x < 6; ++x) {
            for (auto y = 0; y < 6; ++y) {
                if (_block[x][y]->getState() != Block::State::Stop) isMoved = true;
            }
        }
        if (isMoved == false && _checkBlockWaiting == true){
             _checkBlockWaiting = false;
            checkBlock();
        }
    }
}

void GameScene::initTouchEvent()
{
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
}

#define mark - タップイベント

bool GameScene::onTouchBegan(Touch* touch, Event* event)
{
    log("onTouchBegan");
    if (_checkBlockWaiting == true) return false;
    
    auto location = touch->getLocation();
    for (int y = 0; y < 6; ++y) {
        for (int x = 0; x < 6; ++x) {
            auto* block = _block[x][y];
            
            if (block == nullptr) continue;
            if (block->getState() != Block::State::Stop) continue;
            
            float distance = block->getPosition().distance(location);
            
            if (distance <= block->getRadius())
            {
                _touchBlock = block;
                break;
            }
        }
    }
    return true;
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
    log("onTouchMoved");
    //if (checkBlockWaiting == true) return;

    auto location = touch->getLocation();
    if (_touchBlock != nullptr){
        _touchBlock->setPosition(location);
        
        for (int y = 0; y < 6; ++y) {
            for (int x = 0; x < 6; ++x) {
                auto* block = _block[x][y];
                if (block == nullptr) continue;
                if (block == _touchBlock) continue;
                if (block->getState() != Block::State::Stop) continue;
            
                float distance = block->getPosition().distance(location);
                if (distance <= block->getRadius())
                {
                    // 衝突ブロックと座標の入れ替え
                    int _x = _touchBlock->_tagX;
                    int _y = _touchBlock->_tagY;
                    
                    _touchBlock->_tagX = block->_tagX;
                    _touchBlock->_tagY = block->_tagY;
                    
                    auto pos = getPosition(_x, _y);
                    _block[_x][_y] = block;
                    
                    _block[_x][_y]->moveBlock(pos);
                    setTag(_x,_y,block->_tag);
                    
                    break;
                }
            }
        }
    }
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
    if (_touchBlock != nullptr){
        _touchBlock->setPosition(getPosition(_touchBlock->_tagX,_touchBlock->_tagY));
        _block[_touchBlock->_tagX][_touchBlock->_tagY] = _touchBlock;
        setTag(_touchBlock->_tagX,_touchBlock->_tagY,_touchBlock->_tag);
    }
    _touchBlock = nullptr;
    //_checkBlockWaiting = true;
    checkBlock();
}

/* ブロック消す処理
 * 前提 ブロックのオブジェクトは消さないで再利用。消す表示は別で用意する。
*/
void GameScene::checkBlock(){
    
    bool isBroken = false;
    // 揃っていればブロックを壊す
    // 水平方向から順に垂直をチェック
    Vector<Block*> tmpRemoveBlock;
    auto tmp = 9;
    for (auto x = 0; x < 6; ++x) {
        tmp = 9;
        tmpRemoveBlock.clear();
        for (auto y = 0; y < 6; ++y) {
            if (tmp != _tag[x][y])
                tmpRemoveBlock.clear();
            
            tmp = _tag[x][y];
            tmpRemoveBlock.pushBack(_block[x][y]);
            if (tmpRemoveBlock.size() == 3){
                for (auto * block : tmpRemoveBlock){
                    //block->setState(Block::State::Broken);
                    _block[block->_tagX][block->_tagY]->setState(Block::State::Broken);
                    isBroken = true;
                }
            } else if (tmpRemoveBlock.size() > 3){
                _block[x][y]->setState(Block::State::Broken);
                isBroken = true;
            }
        }
    }
    
    for (auto y = 0; y < 6; ++y) {
        tmp = 9;
        tmpRemoveBlock.clear();
        for (auto x = 0; x < 6; ++x) {
            if (tmp != _tag[x][y])
                tmpRemoveBlock.clear();
            
            tmp = _tag[x][y];
            tmpRemoveBlock.pushBack(_block[x][y]);
            if (tmpRemoveBlock.size() == 3){
                for (auto * block : tmpRemoveBlock){
                    _block[block->_tagX][block->_tagY]->setState(Block::State::Broken);
                    isBroken = true;
                }
            } else if (tmpRemoveBlock.size() > 3){
                _block[x][y]->setState(Block::State::Broken);
                isBroken = true;
            }
        }
    }
    
    // 終了処理
    if (isBroken == false){
        _checkBlockWaiting = false;
        return;
    }
    
    for (auto x = 0; x < 6; ++x) {
        Vector<Block*> tmpBlock;
        Vector<Block*> brokenBlock;
        
        for (auto y = 0; y < 6; ++y) {
            //tmpY[y] = _y;
            if (_block[x][y]->getState() == Block::State::Broken){ // 壊れたブロックとxyを入れ替えて位置も移動
                brokenBlock.pushBack(_block[x][y]);
                _block[x][y] = nullptr;
            } else {
                tmpBlock.pushBack(_block[x][y]);
                _block[x][y] = nullptr;
            }
        }
        auto y = 0;
        for(auto* block : tmpBlock){
            _block[x][y] = block;
            auto pos = getPosition(x, y);
            _block[x][y]->moveBlock(pos);
            setTag(x,y,block->_tag);
            y++;
        }
        auto tmpY = 0;
        for(auto* block : brokenBlock){
            int tagNum = arc4random()%5;
            _block[x][y] = block;
            setTag(x,y,tagNum);
            
            _block[x][y]->setPosition(getPosition(x,7 + tmpY));
            auto pos = getPosition(x, y);
            _block[x][y]->moveBlock(pos);
            y++;
            tmpY++;
        }
    }
    // 座標チェック
    /*
    for (int x = 0; x < 6; ++x) {
        for (int y = 0; y < 6; ++y) {
            //_block[x][y]->setPosition(getPosition(x,y));
            
            log("%i %i %i %f %f",x,y,_tag[x][y],_block[x][y]->getPosition().x,_block[x][y]->getPosition().y);
        }
    }
    */
    _checkBlockWaiting = true;
}

void GameScene::setTag(int x,int y,int tagNum){
    _block[x][y]->setColor(_tagColor[tagNum]);
    _block[x][y]->_tag = tagNum;
    _block[x][y]->_tagX = x;
    _block[x][y]->_tagY = y;
    _tag[x][y] = tagNum;
}
