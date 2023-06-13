//  TetrisController.cpp
//  Created by Hoyoung Lee on 2023/01/20.
//

#pragma once

#include "XXXController.h"
#include "ApiCallback.h"
#include "libjson.h"

#include "BlockManager.h"

class TetrisController : public xxx::XXXController
{
private:
    cocos2d::Node* pBgNode_; // 블럭 스프라이트들의 상위 노드

private:
    CONTROLLER_REG_DEC(TetrisController);

public:
    bool init(xxx::NodeContext* pContext);
    static void changeView();

private:
    void onEnter();
    void onExit();
    void onViewChanged();

private:
    void update(float delta);

private:
    void initBoard();
    void resetBlocksColor();
    bool updateBlocksColor();

private:
    void updateScoreUI();
    void setGameOverUIVisible(bool visible);

private:
    void onUpBtnClick(cocos2d::Ref* pSender);
    void onDownBtnClick(cocos2d::Ref* pSender);
    void onLeftBtnClick(cocos2d::Ref* pSender);
    void onRightBtnClick(cocos2d::Ref* pSender);
    void onSpaceBtnClick(cocos2d::Ref* pSender);
};