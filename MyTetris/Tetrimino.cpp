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
    // startPos���� �� ���(�� ���ø�)���� 2���� ��ǥ �� ��ġ�� vecCoord_�� ����
    int tetriminoShape = static_cast<int>(tetriminoType);
    for (int i = 0; i < TETRIMINO_PIXEL_COUNT; ++i)
    {
        auto& loc = arr[tetriminoShape][i];
        //        auto& [x, y] = arr[tetriminoShape][i];
        vecBlockPos_.push_back(cocos2d::Vec2(startPos.x + loc.first, startPos.y + loc.second));
    }

    // ȸ����
    rotationAxisPos_ = vecBlockPos_[1];
}

bool Tetrimino::rotate()
{
    vecBlockRotatedPos_.clear();

    cocos2d::Vec2 originPos = cocos2d::Vec2(rotationAxisPos_.x - 1, rotationAxisPos_.y - 1);

    for (auto& block : vecBlockPos_)
    {
        // ��� ȸ�� / IBlock Size : 3 , �� �� Size : 2

        int blockSize = (tetriminoType_ == TetriminoType::IBLOCK) ? 3 : 2;
        cocos2d::Vec2 tempPos(blockSize - (block.y - originPos.y) + originPos.x, (block.x - originPos.x) + originPos.y);

        // ȸ�� �� ��ġ�� ��, �� �ٱ��� ��� ����
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
    // row ū �������(�Ʒ���) ����
    // ������, rightSort ���� �¿� ���� ����

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
    // �� ��ǥ �̵�
    for (auto& block : vecBlockPos_)
    {
        block.y += moveDistance;
    }

    // ���� ��ǥ �̵�
    rotationAxisPos_.y += moveDistance;
}


void Tetrimino::move(int diff)
{
    // ����ġ�κ��� diff �󸶳� ������ �ִ���
    assert(std::abs(diff) == 1); // 1, -1
    // �¿� �̵�

    // �� ��ǥ �̵�
    for (auto& block : vecBlockPos_)
    {
        block.x += diff;
    }

    // ���� �̵�
    rotationAxisPos_.x += diff;
}