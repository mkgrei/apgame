#ifndef LHAI_EVALUATOR_H_INCLUDED
#define LHAI_EVALUATOR_H_INCLUDED

#include <iostream>
#include <vector>
#include <lh-ai/gameboard.hpp>
#include <lh-ai/gamebasics.hpp>

class Evaluator {
  public:
    virtual int evaluate(const Board& board) = 0;
    virtual ~Evaluator() {};
};

class PerfectEvaluator : public Evaluator {
  public:
    int evaluate(const Board& board) {
      int discdiff
          = board.getCurrentColor()
          * (board.countDisc(BLACK) - board.countDisc(WHITE));
      return discdiff;
    }
};

class WLDEvaluator : public Evaluator {
  public:
    static const int WIN  =  1;
    static const int DRAW =  0;
    static const int LOSE = -1;

    int evaluate(const Board& board) {
      int discdiff
          = board.getCurrentColor()
          * (board.countDisc(BLACK) - board.countDisc(WHITE));
      if (discdiff > 0) return WIN;
      else if (discdiff < 0) return LOSE;
      else return DRAW;
    }
};
#endif
