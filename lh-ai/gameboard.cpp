#include <iostream>
#include <lh-ai/gameboard.hpp>

unsigned Board::checkMobility(const Disc &disc) {
  if (RawBoard[disc.x][disc.y] != EMPTY) return NONE;
  
  int x, y;
  unsigned dir = NONE;

  if (RawBoard[disc.x][disc.y-1] == -disc.color) {
    x = disc.x; y = disc.y-2;
    while (RawBoard[x][y] == -disc.color) { y--; }
    if (RawBoard[x][y] == disc.color) dir |= UPPER;
  }

  if (RawBoard[disc.x][disc.y+1] == -disc.color) {
    x = disc.x; y = disc.y+2;
    while (RawBoard[x][y] == -disc.color) { y++; }
    if (RawBoard[x][y] == disc.color) dir |= LOWER;
  }

  if (RawBoard[disc.x-1][disc.y] == -disc.color) {
    x = disc.x-2; y = disc.y;
    while (RawBoard[x][y] == -disc.color) { x--; }
    if (RawBoard[x][y] == disc.color) dir |= LEFT;
  }

  if (RawBoard[disc.x+1][disc.y] == -disc.color) {
    x = disc.x+2; y = disc.y;
    while (RawBoard[x][y] == -disc.color) { x++; }
    if (RawBoard[x][y] == disc.color) dir |= RIGHT;
  }

  if (RawBoard[disc.x+1][disc.y-1] == -disc.color) {
    x = disc.x+2; y = disc.y-2;
    while (RawBoard[x][y] == -disc.color) { x++; y--; }
    if (RawBoard[x][y] == disc.color) dir |= UPPER_RIGHT;
  }

  if (RawBoard[disc.x-1][disc.y-1] == -disc.color) {
    x = disc.x-2; y = disc.y-2;
    while (RawBoard[x][y] == -disc.color) { x--; y--; }
    if (RawBoard[x][y] == disc.color) dir |= UPPER_LEFT;
  }

  if (RawBoard[disc.x-1][disc.y+1] == -disc.color) {
    x = disc.x-2; y = disc.y+2;
    while (RawBoard[x][y] == -disc.color) { x--; y++; }
    if (RawBoard[x][y] == disc.color) dir |= LOWER_LEFT;
  }

  if (RawBoard[disc.x+1][disc.y+1] == -disc.color) {
    x = disc.x+2; y = disc.y+2;
    while (RawBoard[x][y] == -disc.color) { x++; y++; }
    if (RawBoard[x][y] == disc.color) dir |= LOWER_RIGHT;
  }

  return dir;
}

bool Board::move(const Point& point) {
  if (point.x < 1 || point.x > BOARD_SIZE) return false;
  if (point.y < 1 || point.y > BOARD_SIZE) return false;
  if (MovableDir[Turns][point.x][point.y] == NONE) return false;

  Liberty[point.x][point.y] = -8;
  Liberty[point.x][point.y-1]--;
  Liberty[point.x][point.y+1]--;
  Liberty[point.x-1][point.y-1]--;
  Liberty[point.x-1][point.y]--;
  Liberty[point.x-1][point.y+1]--;
  Liberty[point.x+1][point.y-1]--;
  Liberty[point.x+1][point.y]--;
  Liberty[point.x+1][point.y+1]--;

  flipDiscs(point);

  Turns++;
  CurrentColor = -CurrentColor;

  initMovable();

  return true;
}

void Board::initMovable() {
  Disc disc(0, 0, CurrentColor);

  int dir;

  MovablePos[Turns].clear();

//  std::cout << "INIT NEW ==============" << std::endl;
  for (int x=1; x<=BOARD_SIZE; x++) {
    disc.x = x;
    for (int y=1; y<=BOARD_SIZE; y++) {
      disc.y = y;

      dir = checkMobility(disc);
//      std::cout << x << " " << y << " " << dir << std::endl;
      if (dir != NONE) {
        MovablePos[Turns].push_back(disc);
      }
      MovableDir[Turns][x][y] = dir;
    }
  }
//  std::cout << "===========" << Turns << " " << MovablePos[Turns].size() << std::endl;
}

void Board::flipDiscs(const Point& point) {
  int x, y;
  Disc operation(point.x, point.y, CurrentColor);

  int dir = MovableDir[Turns][point.x][point.y];

  std::vector<Disc> update;

  RawBoard[point.x][point.y] = CurrentColor;
  update.push_back(operation);

  if (dir & UPPER) {
    y = point.y;
    operation.x = point.x;
    while (RawBoard[point.x][--y] != CurrentColor) {
      RawBoard[point.x][y] = CurrentColor;
      operation.y = y;
      update.push_back(operation);
    }
  }

  if (dir & LOWER) {
    y = point.y;
    operation.x = point.x;
    while (RawBoard[point.x][++y] != CurrentColor) {
      RawBoard[point.x][y] = CurrentColor;
      operation.y = y;
      update.push_back(operation);
    }
  }

  if (dir & LEFT) {
    x = point.x;
    operation.y = point.y;
    while (RawBoard[--x][point.y] != CurrentColor) {
      RawBoard[x][point.y] = CurrentColor;
      operation.x = x;
      update.push_back(operation);
    }
  }

  if (dir & RIGHT) {
    x = point.x;
    operation.y = point.y;
    while (RawBoard[++x][point.y] != CurrentColor) {
      RawBoard[x][point.y] = CurrentColor;
      operation.x = x;
      update.push_back(operation);
    }
  }

  if (dir & UPPER_RIGHT) {
    x = point.x;
    y = point.y;
    while (RawBoard[++x][--y] != CurrentColor) {
      RawBoard[x][y] = CurrentColor;
      operation.x = x;
      operation.y = y;
      update.push_back(operation);
    }
  }

  if (dir & UPPER_LEFT) {
    x = point.x;
    y = point.y;
    while (RawBoard[--x][--y] != CurrentColor) {
      RawBoard[x][y] = CurrentColor;
      operation.x = x;
      operation.y = y;
      update.push_back(operation);
    }
  }

  if (dir & LOWER_LEFT) {
    x = point.x;
    y = point.y;
    while (RawBoard[--x][++y] != CurrentColor) {
      RawBoard[x][y] = CurrentColor;
      operation.x = x;
      operation.y = y;
      update.push_back(operation);
    }
  }

  if (dir & LOWER_RIGHT) {
    x = point.x;
    y = point.y;
    while (RawBoard[++x][++y] != CurrentColor) {
      RawBoard[x][y] = CurrentColor;
      operation.x = x;
      operation.y = y;
      update.push_back(operation);
    }
  }

  int discdiff = update.size();

  Discs[CurrentColor]  += discdiff;
  Discs[-CurrentColor] -= (discdiff - 1);
  Discs[EMPTY]--;

  UpdateLog.push_back(update);
}

bool Board::isGameOver() {
  if (Turns == MAX_TURNS) return true;

  if (MovablePos[Turns].size() != 0) return false;

  Disc disc;
  disc.color = -CurrentColor;
  for (int x=1; x<=BOARD_SIZE; x++) {
    disc.x = x;
    for (int y=1; y<=BOARD_SIZE; y++) {
      disc.y = y;
      
      if (checkMobility(disc) != NONE) return false;
    }
  }

  return true;
}

bool Board::pass() {
  if (MovablePos[Turns].size() != 0) return false;

  if (isGameOver()) return false;

  CurrentColor = -CurrentColor;

  UpdateLog.push_back(std::vector<Disc>());

  initMovable();

  return true;
}

bool Board::undo() {
  if (Turns == 0) return false;
//  std::cout << "UNDOING: " << Turns << std::endl;

  CurrentColor = -CurrentColor;

  const std::vector<Disc>& update = UpdateLog.back();

  if (update.empty()) {
    MovablePos[Turns].clear();
    for (unsigned x=1; x<=BOARD_SIZE; x++) {
      for (unsigned y=1; y<=BOARD_SIZE; y++) {
        MovableDir[Turns][x][y] = NONE;
      }
    }
  } else {
    Turns--;

    RawBoard[update[0].x][update[0].y] = EMPTY;
    for (unsigned i=1; i<update.size(); i++) {
      RawBoard[update[i].x][update[i].y] = -CurrentColor;
    }

    unsigned discdiff = update.size();
    Discs[CurrentColor] -= discdiff;
    Discs[-CurrentColor] += discdiff - 1;
    Discs[EMPTY]++;
  }

  UpdateLog.pop_back();

  return true;
}

void Board::init() {
  for (int x=1; x<=BOARD_SIZE; x++) {
    for (int y=1; y<=BOARD_SIZE; y++) {
      RawBoard[x][y] = EMPTY;
    }
  }

  for (int y=0; y<BOARD_SIZE+2; y++) {
    RawBoard[0][y] = WALL;
    RawBoard[BOARD_SIZE+1][y] = WALL;
  }

  for (int x=0; x<BOARD_SIZE+2; x++) {
    RawBoard[x][0] = WALL;
    RawBoard[x][BOARD_SIZE+1] = WALL;
  }

  RawBoard[4][4] = WHITE;
  RawBoard[5][5] = WHITE;
  RawBoard[4][5] = BLACK;
  RawBoard[5][4] = BLACK;

  Discs[BLACK] = 2;
  Discs[WHITE] = 2;
  Discs[EMPTY] = BOARD_SIZE*BOARD_SIZE - 4;

  Turns = 0;
  CurrentColor = BLACK;

  UpdateLog.clear();

  initMovable();
}

Board::Board() {
  init();
}
