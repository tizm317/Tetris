//  BlockManager.h
//  Created by Hoyoung Lee on 2023/01/25.
//

#pragma once

#include "Tetrimino.h"
#include <deque>
enum class BlockState
{
    // 빈 블럭, 움직이는 블럭, 멈춘 블럭+벽, 고스트(그림자, 하드드랍 미리보기)
    EMPTY = 0,
    MOVING,
    STOPPED,
    GHOST
};

class BlockManager
{
private:
    static BlockManager* g_instance;

private:
    std::vector<std::vector<BlockState>> vecCoordinate_; // 상태값 저장하는 2차원 배열
    Tetrimino movingTetrimino_;                         // 현재 이동중인 블럭

private:
    int     score_;
    bool    isGameOver_;
    bool    isScoreChanged_;
    bool    isMovingTetriminoExist_;

private:
    // 다음 블럭 미리보기 관련 변수
    std::deque<TetriminoType>   vecNextTetriminos_;
    TetriminoType               nextTetriminoType_;

public:
    static BlockManager* getInstance();

public:
    void init();

private:
    void initCoordinate();

public:
    bool isGameOver() { return isGameOver_; }
    bool isScoreChanged() { return isScoreChanged_; }
    bool isTetriminoMade() { return isMovingTetriminoExist_; }

public:
    std::vector<std::vector<BlockState>>& getVecCoordinate() { return vecCoordinate_; }
    TetriminoType getNextBlockType() { return nextTetriminoType_; }
    Tetrimino& getMovingBlock() { return movingTetrimino_; }
    int getScore() { return score_; }
    int getDistanceToBottom();

public:
    void setScoreChangedFalse() { isScoreChanged_ = false; }

public:
    bool makeNewBlock();
    bool softDrop();
    bool hardDrop();
    bool rotate();
    bool moveLeft();
    bool moveRight();

private:
    void drop(int distance);
    bool move(int dir); // 좌우이동
    void stopBlock();
    void lineCheck(int lowestRow);
    void lineClear(int targetRow);

private:
    void makeGhost(std::vector<cocos2d::Vec2>& vecBlockPos);
    void clearGhost(std::vector<cocos2d::Vec2>& vecBlockPos);

private:
    BlockManager();
    ~BlockManager() {}
};