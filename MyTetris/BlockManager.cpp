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

    // 2차원 좌표 초기화
    initCoordinate();
}

void BlockManager::initCoordinate()
{
    // 2차원 좌표 배열 초기화
    // 빈 공간(0) / 움직이는 블럭(1) / 막힌 공간(2) / 그림자(3)

    // 0으로 채움
    std::vector<BlockState> tempVec(TETRIS_MAX_COLUMN, BlockState::EMPTY);
    vecCoordinate_.assign(TETRIS_MAX_ROW, tempVec);
}

#include <algorithm>

int BlockManager::getDistanceToBottom()
{
    // 블럭부터 바닥까지 거리 계산
    // 바닥 찾기, 갈 수 있는 거리 계산
    int moveMinDistance = TETRIS_MAX_ROW;
    for (auto& blockCoord : movingTetrimino_.getVecCoord())
    {
        // 바닥 좌표 찾고 이동 거리 계산
        for (int row = blockCoord.y; row < TETRIS_MAX_ROW; ++row)
        {
            int nextRow = row + 1;
            if (nextRow == TETRIS_MAX_ROW || vecCoordinate_[nextRow][blockCoord.x] == BlockState::STOPPED)
            {
                moveMinDistance = std::min<int>(row - blockCoord.y, moveMinDistance); // 0 가능
                break;
            }
        }
    }

    return moveMinDistance;
}

//

bool BlockManager::makeNewBlock()
{
    // 1. 랜덤하게 블럭 종류 설정

    // 없으면 벡터에 TETRIS_MAX_TYPE 개 만들고 푸시, 있으면 벡터에서 팝
    if (vecNextTetriminos_.empty() == true)
    {
        RandomHelper randHelper = RandomHelper(); // TODO: RandomHelper static 확인
        for (int i = 0; i < TETRIS_MAX_TYPE; ++i)
        {
            int value = randHelper.random_int(0, TETRIS_MAX_TYPE - 1);
            vecNextTetriminos_.push_back(static_cast<TetriminoType>(value));
        }
    }

    TetriminoType blockType = vecNextTetriminos_.front();
    vecNextTetriminos_.pop_front();


    // 2. 블럭 생성
    // 블럭 시작 위치 설정
    Vec2 startPos(Vec2(TETRIS_MAX_COLUMN / 2 - 2, 0)); // (3,0)

    movingTetrimino_ = Tetrimino(blockType, startPos);
    isMovingTetriminoExist_ = true;

    // 생성된 블럭모양의 pos값에 따라 2차원 배열에 상태값 저장
    movingTetrimino_.sortVecCoord();
    std::vector<Vec2>& vecCoord = movingTetrimino_.getVecCoord();

    for (auto& blockCoord : vecCoord)
    {
        // 1. 시작 위치에 이미 블럭이 존재하는지 체크
        if (vecCoordinate_[blockCoord.y][blockCoord.x] == BlockState::STOPPED)
        {
            // 이미 존재하면 삭제 후 게임오버
            isGameOver_ = true;

            isMovingTetriminoExist_ = false;

            return false;
        }

        // 2. 본체 생성
        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::MOVING;
    }

    // 3. 그림자 생성
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
    // 바닥 찾기, 갈 수 있는 거리 계산
    int moveMinDistance = getDistanceToBottom();
    if (moveMinDistance == 0)
        return false;

    // 거리 만큼 좌표 이동
    drop(moveMinDistance);

    return true;
}

void BlockManager::drop(int distance)
{
    // 더 내려갈 수 있는지 체크 후 내려갈 수 없으면 정지 및 라인 체크, 클리어

    movingTetrimino_.sortVecCoord();
    std::vector<Vec2>& vecCoord = movingTetrimino_.getVecCoord();

    // 테트리미노의 4개 블럭 중 가장 아래 블럭 row값
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

    // 블럭 위치 수정 (이동거리만큼)
    movingTetrimino_.drop(distance);
}

void BlockManager::stopBlock()
{
    // 좌표 2로 채움
    for (auto& blockCoord : movingTetrimino_.getVecCoord())
    {
        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::STOPPED;
    }

    isMovingTetriminoExist_ = false;
}

void BlockManager::lineCheck(int lowestRow)
{
    // 라인 채워졌는지 체크 후 클리어

    // 가장 아래 블럭부터 위로 4열 체크
    for (int row = lowestRow; row > lowestRow - 4;)
    {
        if (row == 0)
            return;

        // 라인의 STOPPED 상태 갯수 체크
        long count = std::count(vecCoordinate_[row].begin(), vecCoordinate_[row].end(), BlockState::STOPPED);

        if (count != TETRIS_MAX_COLUMN)
        {
            row -= 1;
            continue;
        }

        // 점수 증가
        score_ += 1;
        isScoreChanged_ = true;

        // 라인 클리어
        lineClear(row);
    }
}

void BlockManager::lineClear(int targetRow)
{
    // 클리어 타겟 row 위에 있는 배열 한칸씩 덮어쓰기
    for (int row = targetRow; row > 1; --row)
    {
        vecCoordinate_[row] = vecCoordinate_[row - 1];
    }

    // 맨 위 배열 초기화
    for (int col = 0; col < TETRIS_MAX_COLUMN; ++col)
    {
        vecCoordinate_[0][col] = BlockState::EMPTY;
    }
}


bool BlockManager::rotate()
{
    // OBlock 회전 똑같기 때문에 안함
    if (movingTetrimino_.getTetriminoType() == TetriminoType::OBLOCK)
        return false;

    // 1. 원래꺼 지우고,
    movingTetrimino_.sortVecCoord();
    std::vector<Vec2>& vecCoord = movingTetrimino_.getVecCoord();
    for (auto& blockCoord : vecCoord)
    {
        if (vecCoordinate_[blockCoord.y][blockCoord.x] == BlockState::STOPPED)
        {
            // 바닥에 닿을 때 회전하려고 하는 경우, 상태값이 moving 으로 다시 덮이는 버그 방지
            return false;
        }

        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::EMPTY;
    }

    // 이동 전 그림자 제거
    clearGhost(vecCoord);

    // 2. 회전
    bool rotateSuccess = movingTetrimino_.rotate();
    if (rotateSuccess != false)
    {
        std::vector<cocos2d::Vec2>& vecBlockRotated = movingTetrimino_.getRotatedVecCoord();

        for (auto& block : vecBlockRotated)
        {
            // 회전 후 위치에 다른 블럭이 이미 존재할 시, 실패
            if (vecCoordinate_[block.y][block.x] == BlockState::STOPPED)
            {
                rotateSuccess = false;
                break;
            }
        }

        // 회전 성공 시, 회전 결과물 벡터로 set해줌,
        // 실패 시 원래 벡터 그대로
        if (rotateSuccess == true)
        {
            movingTetrimino_.setVecCoord(vecBlockRotated);
            vecBlockRotated.clear();
        }
    }


    // 3. 회전 후 결과물 생성
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
    // 현위치로부터 diff 얼마나 떨어져 있는지
    // 좌우 이동
    assert(std::abs(diff) == 1);
    // 오른쪽 이동 : dir == 1
    // 왼쪽 이동 : dir == -1

    // 갈 수 있는지 체크

    // 좌우 기준 정렬
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
        // stop상태일 때 moving으로 덮이는 버그 방지
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

    // 이동 전 그림자 제거
    clearGhost(vecBlockPos);

    // 본체 이동
    for (auto& blockCoord : vecBlockPos)
    {
        vecCoordinate_[blockCoord.y][blockCoord.x] = BlockState::EMPTY;
        vecCoordinate_[blockCoord.y][blockCoord.x + diff] = BlockState::MOVING;
    }

    movingTetrimino_.move(diff);

    // 이동 후 그림자 생성
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
        // 이동 전 그림자 제거
        BlockState& ghostBlockState = vecCoordinate_[blockCoord.y + distance][blockCoord.x];
        if (ghostBlockState == BlockState::GHOST)
        {
            ghostBlockState = BlockState::EMPTY;
        }
    }
}