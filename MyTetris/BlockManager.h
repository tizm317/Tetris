//  BlockManager.h
//  Created by Hoyoung Lee on 2023/01/25.
//

#pragma once

#include "Tetrimino.h"
#include <deque>
enum class BlockState
{
    // �� ��, �����̴� ��, ���� ��+��, ��Ʈ(�׸���, �ϵ��� �̸�����)
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
    std::vector<std::vector<BlockState>> vecCoordinate_; // ���°� �����ϴ� 2���� �迭
    Tetrimino movingTetrimino_;                         // ���� �̵����� ��

private:
    int     score_;
    bool    isGameOver_;
    bool    isScoreChanged_;
    bool    isMovingTetriminoExist_;

private:
    // ���� �� �̸����� ���� ����
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
    bool move(int dir); // �¿��̵�
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