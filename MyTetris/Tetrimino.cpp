//  Tetrimino.cpp
//  Created by Hoyoung Lee on 2023/01/25.
//

#include "Tetrimino.h"

static const int TETRIS_MAX_ROW = 20;
static const int TETRIS_MAX_COLUMN = 10;
static const int TETRIS_MAX_TYPE = 7;
static const int TETRIMINO_PIXEL_COUNT = 4;

std::pair<int, int> arr[TETRIS_MAX_TYPE][TETRIMINO_PIXEL_COUNT] =
{
    { {1, 0}, {1, 1}, {2, 0}, {2, 1}}, // O
    { {2, 0}, {1, 1}, {2, 1}, {1, 2}}, // Z
    { {1, 0}, {1, 1}, {2, 1}, {2, 2}}, // S
    { {1, 0}, {1, 1}, {1, 2}, {2, 1}}, // T
    { {1, 0}, {1, 1}, {1, 2}, {0, 2}}, // J
    { {1, 0}, {1, 1}, {1, 2}, {2, 2}}, // L
    { {1, 0}, {1, 1}, {1, 2}, {1, 3}}  // I
};

Tetrimino::Tetrimino(TetriminoType tetriminoType, cocos2d::Vec2 startPos)
    : tetriminoType_(tetriminoType)
{
    // startPos에서 블럭 모양(블럭 템플릿)으로 2차원 좌표 내 위치를 vecCoord_에 저장
    int tetriminoShape = static_cast<int>(tetriminoType);
    for (int i = 0; i < TETRIMINO_PIXEL_COUNT; ++i)
    {
        auto& loc = arr[tetriminoShape][i];
        //        auto& [x, y] = arr[tetriminoShape][i];
        vecBlockPos_.push_back(cocos2d::Vec2(startPos.x + loc.first, startPos.y + loc.second));
    }

    // 회전축
    rotationAxisPos_ = vecBlockPos_[1];
}

bool Tetrimino::rotate()
{
    vecBlockRotatedPos_.clear();

    cocos2d::Vec2 originPos = cocos2d::Vec2(rotationAxisPos_.x - 1, rotationAxisPos_.y - 1);

    for (auto& block : vecBlockPos_)
    {
        // 행렬 회전 / IBlock Size : 3 , 그 외 Size : 2

        int blockSize = (tetriminoType_ == TetriminoType::IBLOCK) ? 3 : 2;
        cocos2d::Vec2 tempPos(blockSize - (block.y - originPos.y) + originPos.x, (block.x - originPos.x) + originPos.y);

        // 회전 후 위치가 벽, 벽 바깥일 경우 실패
        if (tempPos.x < 0 || tempPos.x > TETRIS_MAX_COLUMN - 1)
        {
            return false;
        }

        if (tempPos.y < 0 || tempPos.y > TETRIS_MAX_ROW - 1)
        {
            return false;
        }

        vecBlockRotatedPos_.push_back(tempPos);
    }

    return true;
}


bool compare(const cocos2d::Vec2& p1, const cocos2d::Vec2& p2, bool isRightSort)
{
    // row 큰 순서대로(아래쪽) 정렬
    // 같으면, rightSort 따라서 좌우 기준 정렬

    if (p1.y == p2.y)
        return (isRightSort == true ? (p1.x > p2.x) : (p1.x < p2.x));

    return (p1.y > p2.y);
}

void Tetrimino::sortVecCoord(bool isRightSort)
{
    std::sort(vecBlockPos_.begin(), vecBlockPos_.end(), std::bind(compare, std::placeholders::_1, std::placeholders::_2, isRightSort));
}


void Tetrimino::drop(int moveDistance)
{
    // 블럭 좌표 이동
    for (auto& block : vecBlockPos_)
    {
        block.y += moveDistance;
    }

    // 블럭축 좌표 이동
    rotationAxisPos_.y += moveDistance;
}


void Tetrimino::move(int diff)
{
    // 현위치로부터 diff 얼마나 떨어져 있는지
    assert(std::abs(diff) == 1); // 1, -1
    // 좌우 이동

    // 블럭 좌표 이동
    for (auto& block : vecBlockPos_)
    {
        block.x += diff;
    }

    // 블럭축 이동
    rotationAxisPos_.x += diff;
}