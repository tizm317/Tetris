//  BlockManager.cpp
//  Created by Hoyoung Lee on 2023/01/25.
//

#include "BlockManager.h"

using namespace std;
USING_NS_CC;

static const int TETRIS_MAX_ROW = 20;       // 20 + 2
static const int TETRIS_MAX_COLUMN = 10;    // 10 + 2
static const int TETRIS_MAX_TYPE = 7;

BlockManager* BlockManager::g_instance = nullptr;

BlockManager* BlockManager::getInstance()
{
    if (g_instance == nullptr)
    {
        g_instance = new BlockManager();
    }
    return g_instance;
}

BlockManager::BlockManager()
{
    init();
}

//

void BlockManager::init()
{
    score_ = 0;
    isGameOver_ = false;
    isScoreChanged_ = false;

    // 2���� ��ǥ �ʱ�ȭ
    initCoordinate();
}

void BlockManager::initCoordinate()
{
    // 2���� ��ǥ �迭 �ʱ�ȭ
    // �� ����(0) / �����̴� ��(1) / ���� ����(2) / �׸���(3)

    // 0���� ä��
    std::vector<BlockState> tempVec(TETRIS_MAX_COLUMN, BlockState::EMPTY);
    vecCoordinate_.assign(TETRIS_MAX_ROW, tempVec);
}

#include <algorithm>

int BlockManager::getDistanceToBottom()
{
    // ������ �ٴڱ��� �Ÿ� ���
    // �ٴ� ã��, �� �� �ִ� �Ÿ� ���
    int moveMinDistance = TETRIS_MAX_ROW;
    for (auto& blockCoord : movingTetrimino_.getVecCoord())
    {
        // �ٴ� ��ǥ ã�� �̵� �Ÿ� ���
        for (int row = blockCoord.y; row < TETRIS_MAX_ROW; ++row)
        {
            int nextRow = row + 1;
            if (nextRow == TETRIS_MAX_ROW || vecCoordinate_[nextRow][blockCoord.x] == BlockState::STOPPED)
            {
                moveMinDistance = std::min<int>(row - blockCoord.y, moveMinDistance); // 0 ����
                break;
            }
        }
    }

    return moveMinDistance;
}

//

bool BlockManager::makeNewBlock()
{
    // 1. �����ϰ� �� ���� ����

    // ������ ���Ϳ� TETRIS_MAX_TYPE �� ����� Ǫ��, ������ ���Ϳ��� ��
    if (vecNextTetriminos_.empty() == true)
    {
        RandomHelper randHelper = RandomHelper(); // TODO: RandomHelper static Ȯ��
        for (int i = 0; i < TETRIS_MAX_TYPE; ++i)
        {
            int value = randHelper.random_int(0, TETRIS_MAX_TYPE - 1);
            vecNextTetriminos_.push_back(static_cast<TetriminoType>(value));
        }
    }

    TetriminoType blockType = vecNextTetriminos_.front();
    vecNextTetriminos_.pop_front();


    // 2. �� ����
    // �� ���� ��ġ ����
    Vec2 startPos(Vec2(TETRIS_MAX_COLUMN / 2 - 2, 0)); // (3,0)

    movingTetrimino_ = Tetrimino(blockType, startPos);
    isMovingTetriminoExist_ = true;

    // ������ ������� pos���� ���� 2���� �迭�� ���°� ����
    movingTetrimino_.sortVecCoord();
    std::vector<Vec2>& vecCoord = movingTetrimino_.getVecCoord();

    for (auto& blockCoord : vecCoord)
    {
        // 1. ���� ��ġ�� �̹� ���� �����ϴ��� üũ
        if (vecCoordinate_[blockCoord.y][blockCoord.x] == BlockState::STOPPED)
        {
            // �̹� �����ϸ� ���� �� ���ӿ���
            isGameOver_ = true;

            isMovingTetriminoExist_ = false;

            return false;
        }

        // 2. ��ü ����
        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::MOVING;
    }

    // 3. �׸��� ����
    makeGhost(vecCoord);

    return true;
}

bool BlockManager::softDrop()
{
    int moveDistance = 1;
    drop(moveDistance);

    return true;
}

bool BlockManager::hardDrop()
{
    // �ٴ� ã��, �� �� �ִ� �Ÿ� ���
    int moveMinDistance = getDistanceToBottom();
    if (moveMinDistance == 0)
        return false;

    // �Ÿ� ��ŭ ��ǥ �̵�
    drop(moveMinDistance);

    return true;
}

void BlockManager::drop(int distance)
{
    // �� ������ �� �ִ��� üũ �� ������ �� ������ ���� �� ���� üũ, Ŭ����

    movingTetrimino_.sortVecCoord();
    std::vector<Vec2>& vecCoord = movingTetrimino_.getVecCoord();

    // ��Ʈ���̳��� 4�� �� �� ���� �Ʒ� �� row��
    int lowestRow = vecCoord[0].y;

    for (auto& blockCoord : vecCoord)
    {
        if (blockCoord.y >= TETRIS_MAX_ROW - 1 || vecCoordinate_[blockCoord.y + distance][blockCoord.x] == BlockState::STOPPED)
        {
            stopBlock();
            lineCheck(lowestRow);
            return;
        }
    }

    for (auto& blockCoord : vecCoord)
    {
        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::EMPTY;
        vecCoordinate_[blockCoord.y + distance][blockCoord.x] = BlockState::MOVING;
    }

    // �� ��ġ ���� (�̵��Ÿ���ŭ)
    movingTetrimino_.drop(distance);
}

void BlockManager::stopBlock()
{
    // ��ǥ 2�� ä��
    for (auto& blockCoord : movingTetrimino_.getVecCoord())
    {
        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::STOPPED;
    }

    isMovingTetriminoExist_ = false;
}

void BlockManager::lineCheck(int lowestRow)
{
    // ���� ä�������� üũ �� Ŭ����

    // ���� �Ʒ� ������ ���� 4�� üũ
    for (int row = lowestRow; row > lowestRow - 4;)
    {
        if (row == 0)
            return;

        // ������ STOPPED ���� ���� üũ
        long count = std::count(vecCoordinate_[row].begin(), vecCoordinate_[row].end(), BlockState::STOPPED);

        if (count != TETRIS_MAX_COLUMN)
        {
            row -= 1;
            continue;
        }

        // ���� ����
        score_ += 1;
        isScoreChanged_ = true;

        // ���� Ŭ����
        lineClear(row);
    }
}

void BlockManager::lineClear(int targetRow)
{
    // Ŭ���� Ÿ�� row ���� �ִ� �迭 ��ĭ�� �����
    for (int row = targetRow; row > 1; --row)
    {
        vecCoordinate_[row] = vecCoordinate_[row - 1];
    }

    // �� �� �迭 �ʱ�ȭ
    for (int col = 0; col < TETRIS_MAX_COLUMN; ++col)
    {
        vecCoordinate_[0][col] = BlockState::EMPTY;
    }
}


bool BlockManager::rotate()
{
    // OBlock ȸ�� �Ȱ��� ������ ����
    if (movingTetrimino_.getTetriminoType() == TetriminoType::OBLOCK)
        return false;

    // 1. ������ �����,
    movingTetrimino_.sortVecCoord();
    std::vector<Vec2>& vecCoord = movingTetrimino_.getVecCoord();
    for (auto& blockCoord : vecCoord)
    {
        if (vecCoordinate_[blockCoord.y][blockCoord.x] == BlockState::STOPPED)
        {
            // �ٴڿ� ���� �� ȸ���Ϸ��� �ϴ� ���, ���°��� moving ���� �ٽ� ���̴� ���� ����
            return false;
        }

        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::EMPTY;
    }

    // �̵� �� �׸��� ����
    clearGhost(vecCoord);

    // 2. ȸ��
    bool rotateSuccess = movingTetrimino_.rotate();
    if (rotateSuccess != false)
    {
        std::vector<cocos2d::Vec2>& vecBlockRotated = movingTetrimino_.getRotatedVecCoord();

        for (auto& block : vecBlockRotated)
        {
            // ȸ�� �� ��ġ�� �ٸ� ���� �̹� ������ ��, ����
            if (vecCoordinate_[block.y][block.x] == BlockState::STOPPED)
            {
                rotateSuccess = false;
                break;
            }
        }

        // ȸ�� ���� ��, ȸ�� ����� ���ͷ� set����,
        // ���� �� ���� ���� �״��
        if (rotateSuccess == true)
        {
            movingTetrimino_.setVecCoord(vecBlockRotated);
            vecBlockRotated.clear();
        }
    }


    // 3. ȸ�� �� ����� ����
    movingTetrimino_.sortVecCoord();
    for (auto& blockCoord : vecCoord)
    {
        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::MOVING;
    }

    makeGhost(vecCoord);

    return true;
}

bool BlockManager::moveLeft()
{
    bool success = move(-1);
    return success;
}

bool BlockManager::moveRight()
{
    bool success = move(1);
    return success;
}

bool BlockManager::move(int diff)
{
    // ����ġ�κ��� diff �󸶳� ������ �ִ���
    // �¿� �̵�
    assert(std::abs(diff) == 1);
    // ������ �̵� : dir == 1
    // ���� �̵� : dir == -1

    // �� �� �ִ��� üũ

    // �¿� ���� ����
    if (diff == 1)
    {
        movingTetrimino_.sortVecCoord(true);
    }
    else
    {
        movingTetrimino_.sortVecCoord(false);
    }

    std::vector<cocos2d::Vec2>& vecBlockPos = movingTetrimino_.getVecCoord();
    for (auto& blockCoord : vecBlockPos)
    {
        // stop������ �� moving���� ���̴� ���� ����
        if (vecCoordinate_[blockCoord.y][blockCoord.x] == BlockState::STOPPED)
            return false;

        if (blockCoord.x + diff < 0 || blockCoord.x + diff > TETRIS_MAX_COLUMN - 1)
        {
            return false;
        }

        if (vecCoordinate_[blockCoord.y][blockCoord.x + diff] == BlockState::STOPPED)
        {
            return false;
        }
    }

    // �̵� �� �׸��� ����
    clearGhost(vecBlockPos);

    // ��ü �̵�
    for (auto& blockCoord : vecBlockPos)
    {
        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::EMPTY;
        vecCoordinate_[blockCoord.y][blockCoord.x + diff] = BlockState::MOVING;
    }

    movingTetrimino_.move(diff);

    // �̵� �� �׸��� ����
    makeGhost(vecBlockPos);

    return true;
}

void BlockManager::makeGhost(std::vector<cocos2d::Vec2>& vecBlockPos)
{
    int distance = getDistanceToBottom();
    if (distance <= 0)
        return;

    for (auto& blockCoord : vecBlockPos)
    {
        if (vecCoordinate_[blockCoord.y + distance][blockCoord.x] != BlockState::EMPTY)
            continue;

        vecCoordinate_[blockCoord.y + distance][blockCoord.x] = BlockState::GHOST;
    }
}

void BlockManager::clearGhost(std::vector<cocos2d::Vec2>& vecBlockPos)
{
    int distance = getDistanceToBottom();
    if (distance <= 0)
        return;

    for (auto& blockCoord : vecBlockPos)
    {
        // �̵� �� �׸��� ����
        BlockState& ghostBlockState = vecCoordinate_[blockCoord.y + distance][blockCoord.x];
        if (ghostBlockState == BlockState::GHOST)
        {
            ghostBlockState = BlockState::EMPTY;
        }
    }
}