# Tetris
C++ 과 Cocos2d-x 를 이용하여 제작한 테트리스


**[게임명] : 테트리스**

**[제작 기간] : 2023/01/21 ~ 2023/02/06**
 
**[제작엔진/언어] : Cocos2d-x / C++**
 
**[코드 요약]**

**1. TetrisController**
 - 씬 초기화
 - 게임 업데이트 , UI 업데이트
 - 버튼 입력 체크
 
**2. BlockManager**
 - 블럭 상태 저장 2차원 배열 관리
 - 현재 이동중인 블럭 관리
 - 스코어 및 게임오버 체크
 
**3. Tetrimino**
 - 종류, 색깔 정보 저장
 - 입력 받은 종류에 따라, 블록 생성
 - 블록 회전 / 이동
 
**[구현 기능]**
- 테트리미노(테트리스 블록) 종류별 랜덤 생성 및 아래로 이동
- 점수 표시 및 게임오버 표시
- 테트리미노 조작 (벽/블록 부딪힐 시 이동, 회전 불가)
  - 좌우 이동
  - 소프트 드롭 : 아래로 한칸 이동
  - 하드 드롭 : 바닥으로 바로 이동
  - 회전
- 고스트 기능 : 하단 그림자 미리보기
- 라인 체크 및 제거
 
 
**[게임영상] : https://www.youtube.com/watch?v=uxivK7ykagA&ab_channel=%EC%9D%B4%ED%9D%90%EC%97%89**
