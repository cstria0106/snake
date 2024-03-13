#include "console/console.h"
#include <cstring>
#include <string>

#define BOARD_SIZE 20
#define MOVE_DELAY 15
#define WALL_VERTICAL_STRING "┃"
#define WALL_HORIZONTAL_STRING "━"
#define WALL_RIGHT_TOP_STRING "┓"
#define WALL_LEFT_TOP_STRING "┏"
#define WALL_RIGHT_BOTTOM_STRING "┛"
#define WALL_LEFT_BOTTOM_STRING "┗"
#define SNAKE_STRING "■"
#define TAIL_STRING "■"
#define APPLE_STRING "●"

#define FRUIT_SCORE 10

// 게임을 초기화(설정) 한다
void initialize();
// 과일을 랜덤 위치에 생성한다
void respawnFruit();
// 게임 오버 됐는지 체크한다
bool gameOver();
// x, y 위치에 뱀의 머리가 있는지 체크한다
bool isSnake(int x, int y);
// 뱀의 이동 방향을 설정한다
void setDirection(int d);
// 뱀이 벽이나 자기 자신과 충돌했는지 체크한다
bool checkSnakeCollision();
// 뱀이 과일과 충돌했는지 체크한다
bool checkAppleCollision();
// 뱀 이동에 대한 키보드 입력을 처리한다
void handleSnakeInput();
// 뱀을 이동시킨다
void moveSnake();
// 한 프레임을 처리하여 게임 상태를 업데이트한다.
void update();
// 맵을 그린다
void drawBoard();
// 과일을 그린다
void drawApple();
// 뱀을 그린다
void drawSnake();
// 게임 오버 텍스트를 그린다
void drawGameOverText();
// 점수를 그린다
void drawScore();
// 모든 그래픽을 그린다.
void draw();
// 게임을 실행한다
void game();

// 게임
bool win = false;  // 게임에서 승리하였는지 여부
bool lose = false; // 게임에서 패배하였는지 여부
int score = 0;     // 점수
int totalFruits = (BOARD_SIZE - 2) * (BOARD_SIZE - 2) - 1; // 남은 과일의 개수

// 뱀/과일
int head[2] = {BOARD_SIZE / 2, BOARD_SIZE / 2}; // 뱀 머리 위치
int tails[BOARD_SIZE * BOARD_SIZE][2];          // 꼬리 위치 배열
int length = 0;                                 // 뱀 꼬리의 길이
int fruit[2];                                   // 과일의 위치

// movement
int moveTimer = MOVE_DELAY; // 뱀 이동 타이머
int direction = 1; // 뱀 이동 방향 (0: 왼쪽, 1: 오른쪽, 2: 위, 3: 아래)
int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // 이동 방향 벡터

void initialize() {
  // 게임 변수 초기화
  win = false;
  lose = false;
  score = 0;
  totalFruits = (BOARD_SIZE - 2) * (BOARD_SIZE - 2) - 1;

  // 뱀 변수 초기화
  length = 0;

  // 이동 변수 초기화
  moveTimer = MOVE_DELAY;
  direction = 1;

  // 뱀 머리 위치를 중앙으로 설정
  head[0] = BOARD_SIZE / 2;
  head[1] = BOARD_SIZE / 2;

  // 과일을 랜덤 위치에 생성
  respawnFruit();
}

void respawnFruit() {
  static int availablePositions[BOARD_SIZE * BOARD_SIZE][2];
  int avaialblePositionsCount = 0;

  // 뱀의 머리나 꼬리가 없는 위치를 찾아서 배열에 저장
  for (int i = 1; i < BOARD_SIZE - 1; i++) {
    for (int j = 1; j < BOARD_SIZE - 1; j++) {
      if (!isSnake(i, j)) {
        availablePositions[avaialblePositionsCount][0] = i;
        availablePositions[avaialblePositionsCount][1] = j;
        avaialblePositionsCount++;
      }
    }
  }

  // 배열에서 랜덤 위치를 선택
  int randomIndex = rand() % avaialblePositionsCount;
  fruit[0] = availablePositions[randomIndex][0];
  fruit[1] = availablePositions[randomIndex][1];
}

bool gameOver() { return win || lose; }

bool isSnake(int x, int y) {
  // 뱀의 꼬리가 있는 위치인지 체크
  for (int i = 0; i < length; i++) {
    if (tails[i][0] == x && tails[i][1] == y)
      return true;
  }

  // 뱀의 머리가 있는 위치인지 체크
  return head[0] == x && head[1] == y;
}

void setDirection(int d) {
  // 자기 꼬리가 있는 방향이 아닌 경우에만 방향을 설정
  if (length == 0 || (head[0] + directions[d][0] != tails[0][0] &&
                      head[1] + directions[d][1] != tails[0][1]))
    direction = d;
}

bool checkSnakeCollision() {
  // 뱀의 머리가 꼬리에 부딪히는지 체크
  for (int i = 0; i < length; i++) {
    if (head[0] == tails[i][0] && head[1] == tails[i][1])
      return true;
  }

  // 뱀의 머리가 벽에 부딪히는지 체크
  return (head[0] == 0 || head[0] == BOARD_SIZE - 1 || head[1] == 0 ||
          head[1] == BOARD_SIZE - 1);
}

bool checkAppleCollision() {
  // 뱀의 머리가 과일에 부딪히는지 체크
  return (head[0] == fruit[0] && head[1] == fruit[1]);
}

void handleSnakeInput() {
  if (key(console::K_LEFT))
    setDirection(0);
  if (key(console::K_RIGHT))
    setDirection(1);
  if (key(console::K_UP))
    setDirection(2);
  if (key(console::K_DOWN))
    setDirection(3);
}

void moveSnake() {
  // 뱀 이동 타이머가 0이 되면
  // 타이머를 다시 MOVE_DELAY로 설정하고 뱀을 이동시킨다
  moveTimer--;
  if (moveTimer == 0) {
    moveTimer = MOVE_DELAY;
    // 뱀의 꼬리 위치를 갱신한다
    for (int i = length; i > 0; i--) {
      tails[i][0] = tails[i - 1][0];
      tails[i][1] = tails[i - 1][1];
    }
    tails[0][0] = head[0];
    tails[0][1] = head[1];

    // 머리를 방향 벡터만큼 더하여 이동시킨다
    head[0] += directions[direction][0];
    head[1] += directions[direction][1];
  }
}

void update() {
  // 게임 오버인 경우 Enter 키를 누르면 게임을 초기화한다
  // 그 외에는 아무 처리도 하지 않도록 한다
  if (gameOver()) {
    if (key(console::K_ENTER)) {
      initialize();
    }
    return;
  }

  handleSnakeInput();
  moveSnake();

  // 뱀이 벽이나 꼬리에 충돌했다면 게임에서 패배하도록 한다
  if (checkSnakeCollision()) {
    lose = true;
    return;
  }

  // 뱀이 사과에 충돌했다면 뱀의 길이를 늘리고 점수를 올린다
  if (checkAppleCollision()) {
    length++;
    score += FRUIT_SCORE;
    totalFruits -= 1;
    if (totalFruits > 0) {
      respawnFruit();
    }
  }

  // 더이상 과일을 생성할 수 없다면 게임에서 승리하도록 한다
  if (totalFruits == 0) {
    win = true;
  }
}

void drawBoard() {
  // 맵의 가장자리에 벽을 그린다
  for (int i = 0; i < BOARD_SIZE; i++) {
    console::draw(i, 0, WALL_HORIZONTAL_STRING);
    console::draw(i, BOARD_SIZE - 1, WALL_HORIZONTAL_STRING);
    console::draw(0, i, WALL_VERTICAL_STRING);
    console::draw(BOARD_SIZE - 1, i, WALL_VERTICAL_STRING);
  }

  // 맵의 모서리에 벽을 그린다
  console::draw(0, 0, WALL_LEFT_TOP_STRING);
  console::draw(0, BOARD_SIZE - 1, WALL_LEFT_BOTTOM_STRING);
  console::draw(BOARD_SIZE - 1, 0, WALL_RIGHT_TOP_STRING);
  console::draw(BOARD_SIZE - 1, BOARD_SIZE - 1, WALL_RIGHT_BOTTOM_STRING);
}

void drawApple() { console::draw(fruit[0], fruit[1], APPLE_STRING); }

void drawSnake() {
  // 꼬리를 그린다
  for (int i = 0; i < length; i++) {
    console::draw(tails[i][0], tails[i][1], TAIL_STRING);
  }
  // 머리를 그린다
  console::draw(head[0], head[1], SNAKE_STRING);
}

void drawGameOverText() {
  const char *gameOverText = win ? "YOU WIN!" : "YOU LOSE!";
  const char *tryAgainText = "Try again? (Enter)";
  console::draw(BOARD_SIZE / 2 - strlen(gameOverText) / 2, BOARD_SIZE / 2,
                gameOverText);
  console::draw(BOARD_SIZE / 2 - strlen(tryAgainText) / 2, BOARD_SIZE / 2 + 1,
                tryAgainText);
}

void drawScore() {
  std::string scoreString = "Score: " + std::to_string(score);
  console::draw(BOARD_SIZE / 2 - scoreString.length() / 2, BOARD_SIZE,
                scoreString);
}

void draw() {
  drawBoard();
  drawApple();
  drawSnake();
  drawScore();
  if (gameOver()) {
    drawGameOverText();
  }
}

void game() {
  initialize();

  while (true) {
    console::clear();
    update();
    draw();
    console::wait();

    if (key(console::K_ESC))
      return;
  }
}
