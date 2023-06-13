//  Tetrimino.h
//  Created by Hoyoung Lee on 2023/01/25.
//

#pragma once

enum class TetriminoType
{
    OBLOCK = 0,
    ZBLOCK,
    SBLOCK,
    TBLOCK,
    JBLOCK,
    LBLOCK,
    IBLOCK,
};

class Tetrimino
{
private:
    TetriminoType               tetriminoType_;
    cocos2d::Color3B            tetriminoColor_;
    cocos2d::Vec2               rotationAxisPos_;      // ȸ���� ������
    std::vector<cocos2d::Vec2>  vecBlockPos_;          // �� ������ ����
    std::vector<cocos2d::Vec2>  vecBlockRotatedPos_;   // ȸ�� ������ ����

public:
    TetriminoType getTetriminoType() { return tetriminoType_; }
    std::vector<cocos2d::Vec2>& getVecCoord() { return vecBlockPos_; }
    std::vector<cocos2d::Vec2>& getRotatedVecCoord() { return vecBlockRotatedPos_; }


public:
    void setVecCoord(std::vector<cocos2d::Vec2> vecRotated) { vecBlockPos_ = vecRotated; }

public:
    void sortVecCoord(bool isRightSort = false);

public:
    void drop(int moveDistance = 1);
    void move(int dir); // �¿��̵�
    bool rotate();

public:
    Tetrimino() {}
    Tetrimino(TetriminoType tetriminoType, cocos2d::Vec2 startPos);
    ~Tetrimino() {}
};