#ifndef LHAI_H_INCLUDED
#define LHAI_H_INCLUDED

#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include <random>
#include <lh-ai/gameboard.hpp>
#include <lh-ai/gamebasics.hpp>

struct Move : public Point {
  int eval;
  Move() : Point(0, 0), eval(0) {}
  Move(int x, int y, int eval) {
    this->x = x;
    this->y = y;
    this->eval = eval;
  }
};

struct MoveGreater {
  bool operator() (const Move& lhs, const Move& rhs) {
    return (lhs.eval > rhs.eval);
  }
};

class AI {
  public:
    AI() : presearch_depth(3), normal_depth(5), wld_depth(15),
           perfect_depth(13) {};

    virtual ~AI() {};

    unsigned presearch_depth;
    unsigned normal_depth;
    unsigned wld_depth;
    unsigned perfect_depth;

    Board ai_board;

    void setOpponentMove(Point& p) {
      this->ai_board.move(p);
    };

    void setOpponentMove(std::array<apgame::reversi_stone, 64> const &board) {
      const std::vector<Point>& movables = this->ai_board.getMovablePos();
      Point p;
      for (unsigned i=0; i<movables.size(); i++) {
        unsigned idx;
        idx = movables[i].x + (movables[i].y-1)*8;
        if (board[idx] != 0) {
          p = movables[i];
        }
      }
      this->ai_board.move(p);
    };
    
    virtual Point move() = 0;
  private:
};

class RandomAI : public AI {
  public:
    RandomAI() : AI() {};
    Point move() {
      std::vector<Point> movables = this->ai_board.getMovablePos();
      Point p;
      p = Point(0, 0);

      if (movables.empty()) {
        this->ai_board.pass();
        return p;
      }

      if (movables.size() == 1) {
        this->ai_board.move(movables[0]);
        return p;
      }

      std::random_device rnd;
      std::mt19937 mt(rnd());
      std::uniform_int_distribution<> rand10(0,movables.size()-1);
      p = movables[rand10(mt)];

      this->ai_board.move(p);
      return p;
    };
};

#endif
