//  TetrisController.cpp
//  Created by Hoyoung Lee on 2023/01/20.
//

#include "TetrisController.h"

#include "UIManager.h"
#include "XXXScene.h"
#include "MenuBar.h"

USING_NS_STD;
USING_NS_CC;
USING_NS_XXX;

CONTROLLER_REG_DEF(TetrisController);

const int BACKGROUND_TAG = 1;
const int SCORE_TAG = 2;
const int GAMEOVER_TAG = 3;

void TetrisController::changeView()
{
    NodeContextPtr pContext(new NodeContext());

    UIManager::getInstance()->getNodeAsync("TetrisLayer", XXXScene::replaceView, pContext);
}

bool TetrisController::init(xxx::NodeContext* pContext)
{
    if (XXXController::init(pContext) == false)
        return false;

    pContext->putHandler("UpBtnHandler", CC_CALLBACK_1(TetrisController::onUpBtnClick, this));
    pContext->putHandler("DownBtnHandler", CC_CALLBACK_1(TetrisController::onDownBtnClick, this));
    pContext->putHandler("LeftBtnHandler", CC_CALLBACK_1(TetrisController::onLeftBtnClick, this));
    pContext->putHandler("RightBtnHandler", CC_CALLBACK_1(TetrisController::onRightBtnClick, this));
    pContext->putHandler("SpaceBtnHandler", CC_CALLBACK_1(TetrisController::onSpaceBtnClick, this));

    // Tag
    pContext->putInt("BackgroundTag", BACKGROUND_TAG);
    pContext->putInt("GameOverTag", GAMEOVER_TAG);
    pContext->putInt("ScoreTag", SCORE_TAG);

    // Update함수 스케줄 등록
    schedule(schedule_selector(TetrisController::update), 0.5f);

    return true;
}

void TetrisController::onEnter()
{
    XXXController::onEnter();

    // 하단 버튼 리스너 추가 등록
    MenuBar::get()->setMainBackButton(true);
}

void TetrisController::onExit()
{
    MenuBar::get()->setMainBackButton(false);

    XXXController::onExit();
}


void TetrisController::onViewChanged()
{
    // 화면에 필요한 노드 모두 생성 후 호출

    // 블럭 이미지의 부모 노드인 배경노드
    pBgNode_ = findNodeByTag(BACKGROUND_TAG);
    if (pBgNode_ == nullptr)
        return;

    // 블럭 좌표계 초기화
    initBoard();

    // 게임 초기화
    BlockManager::getInstance()->init();

    // 게임오버 UI 비활성화
    setGameOverUIVisible(false);
}



void TetrisController::update(float delta)
{
    BlockManager* pBlockManager = BlockManager::getInstance();

    // 1. 게임종료 체크
    if (pBlockManager->isGameOver() == true)
    {
        // 리셋 후 재시작
        resetBlocksColor();
        setGameOverUIVisible(false);

        pBlockManager->init();
    }

    // 2. 블럭 생성 or 이동
    // 움직이는 블럭이 없으면 새로 생성
    if (pBlockManager->isTetriminoMade() == false)
    {
        bool isBlockMade = pBlockManager->makeNewBlock();
        if (isBlockMade == false)
            return;
    }
    else
    {
        pBlockManager->softDrop();
    }

    // 3. 백그라운드 격자무늬 보드 업데이트
    updateBlocksColor();

    // 4. 스코어 UI 업데이트
    if (pBlockManager->isScoreChanged() == true)
    {
        // UI 업데이트 후 bool값 false로 변경
        updateScoreUI();
        pBlockManager->setScoreChangedFalse();
    }

}

// Tetris Baground Blocks

static const int PIXEL_SIZE = 30;
static const int TETRIS_MAX_ROW = 20;       // 20 + 2
static const int TETRIS_MAX_COLUMN = 10;    // 10 + 2

void TetrisController::initBoard()
{
    // 백그라운드 노드 아래에 테트리스 격자무늬 블럭 스프라이트 생성

    for (int row = 0; row < TETRIS_MAX_ROW; ++row)
    {
        for (int col = 0; col < TETRIS_MAX_COLUMN; ++col)
        {
            Sprite* pBlockSprite = Sprite::create();
            pBlockSprite->setTextureRect(Rect(Point::ZERO, Size(PIXEL_SIZE, PIXEL_SIZE)));

            pBlockSprite->setColor(Color3B::BLACK);

            Vec2 pos(PIXEL_SIZE * col, pBgNode_->getContentSize().height - (PIXEL_SIZE * row));
            pBlockSprite->setPosition(pos);
            pBlockSprite->setAnchorPoint(Vec2::ZERO);

            pBgNode_->addChild(pBlockSprite);
            pBlockSprite->setTag(col + (row * TETRIS_MAX_COLUMN));
        }
    }
}

void TetrisController::resetBlocksColor()
{
    // 벽 제외 나머지 블럭 스프라이트 색 리셋
    for (int row = 0; row < TETRIS_MAX_ROW; ++row)
    {
        for (int col = 0; col < TETRIS_MAX_COLUMN; ++col)
        {
            Node* pBlockNode = pBgNode_->getChildByTag(col + (row * TETRIS_MAX_COLUMN));
            if (pBlockNode == nullptr)
                return;

            pBlockNode->setColor(Color3B::BLACK);
        }
    }
}

std::map<TetriminoType, cocos2d::Color3B> colorMap
{
    {TetriminoType::OBLOCK, cocos2d::Color3B(255, 255,   0)}, // Yellow
    {TetriminoType::ZBLOCK, cocos2d::Color3B(255,   0,   0)}, // Red
    {TetriminoType::SBLOCK, cocos2d::Color3B(0, 255,   0)}, // Green
    {TetriminoType::TBLOCK, cocos2d::Color3B(255,   0, 255)}, // Magenta
    {TetriminoType::JBLOCK, cocos2d::Color3B(0,   0, 255)}, // Blue
    {TetriminoType::LBLOCK, cocos2d::Color3B(255, 127,   0)}, // Orange
    {TetriminoType::IBLOCK, cocos2d::Color3B(80, 188, 223)}, // Skyblue
};

bool TetrisController::updateBlocksColor()
{
    // 테트리스 격자무늬 백그라운드 보드의 블럭 스프라이트들 색상 업데이트 함수

    BlockManager* pBlockManager = BlockManager::getInstance();

    Tetrimino& pMovingBlock = pBlockManager->getMovingBlock();

    std::vector<std::vector<BlockState>>& vecCoordinate = pBlockManager->getVecCoordinate();

    for (int row = 0; row < TETRIS_MAX_ROW; ++row)
    {
        for (int col = 0; col < TETRIS_MAX_COLUMN; ++col)
        {
            Node* pBlockNode = pBgNode_->getChildByTag(col + (row * TETRIS_MAX_COLUMN));
            if (pBlockNode == nullptr)
                return false;

            // 2차원 배열 저장된 상태값에 따라 블럭 색상 변경
            BlockState blockState = vecCoordinate[row][col];

            switch (blockState)
            {
            case BlockState::EMPTY:
                pBlockNode->setColor(Color3B::BLACK);
                break;
            case BlockState::MOVING:
            {
                TetriminoType tetriminoType = pMovingBlock.getTetriminoType();
                Color3B color = colorMap[tetriminoType];
                pBlockNode->setColor(color);
            }
            break;
            case BlockState::GHOST:
                pBlockNode->setColor(Color3B::GRAY);
                break;
            default:
                break;
            }
        }
    }
    return true;
}

// UI

void TetrisController::updateScoreUI()
{
    Node* pParentNode = pBgNode_->getParent();
    if (pParentNode == nullptr)
        return;

    Label* pScoreLabel = dynamic_cast<Label*>(pParentNode->getChildByTag(SCORE_TAG));
    if (pScoreLabel == nullptr)
        return;

    // 점수 표시
    pScoreLabel->setString(to_string(BlockManager::getInstance()->getScore()));
}


void TetrisController::setGameOverUIVisible(bool visible)
{
    Node* pParentNode = pBgNode_->getParent();
    if (pParentNode == nullptr)
        return;

    Label* pGameOverLabel = NodeUtils::findNodeByTag<Label>(pParentNode, GAMEOVER_TAG);
    if (pGameOverLabel == nullptr)
        return;

    pGameOverLabel->setVisible(visible);
}

// Handler

void TetrisController::onUpBtnClick(cocos2d::Ref* pSender)
{
    bool success = BlockManager::getInstance()->rotate();
    if (success == false)
        return;

    updateBlocksColor();
}

void TetrisController::onDownBtnClick(cocos2d::Ref* pSender)
{
    bool success = BlockManager::getInstance()->softDrop();
    if (success == false)
        return;

    updateBlocksColor();
}

void TetrisController::onLeftBtnClick(cocos2d::Ref* pSender)
{
    bool success = BlockManager::getInstance()->moveLeft();
    if (success == false)
        return;

    updateBlocksColor();
}

void TetrisController::onRightBtnClick(cocos2d::Ref* pSender)
{
    bool success = BlockManager::getInstance()->moveRight();
    if (success == false)
        return;

    updateBlocksColor();
}

void TetrisController::onSpaceBtnClick(cocos2d::Ref* pSender)
{
    bool success = BlockManager::getInstance()->hardDrop();
    if (success == false)
        return;

    updateBlocksColor();
}