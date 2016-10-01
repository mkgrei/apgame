#include <cassert>
#include <iostream>
#include <random>
#include <chrono>
#include <lh-ai/gamebasics.hpp>
#include <lh-ai/ai.hpp>

Point AlphaBetaAI::move() {
  std::vector<Point> movables = this->ai_board.getMovablePos();
  Point p = Point(0, 0);

  if (movables.empty()) {
    this->ai_board.pass();
    return p;
  }

  if (movables.size() == 1) {
    this->ai_board.move(movables[0]);
    return p;
  }

  int limit;
  Eval = new PerfectEvaluator();
  sort(this->ai_board, movables, presearch_depth);

  if (MAX_TURNS - this->ai_board.getTurns() <= wld_depth) {
    delete Eval;
    limit = std::numeric_limits<int>::max();
    if (MAX_TURNS - this->ai_board.getTurns() <= perfect_depth)
      Eval = new PerfectEvaluator();
    else
      Eval = new WLDEvaluator();
  } else {
    limit = normal_depth;
  }

  int eval, alpha = -std::numeric_limits<int>::max();
  int beta = std::numeric_limits<int>::max();
  int count = 0;
  for (unsigned i=0; i<movables.size(); i++) {
    this->ai_board.move(movables[i]);
    eval = -alphabeta(this->ai_board, limit-1, -beta, -alpha);
    this->ai_board.undo();

    if (eval > alpha) {
      count = 0;
      alpha = eval;
      p = movables[i];
    } else if (eval == alpha) {
      count += 1;
      std::random_device rnd;
      std::mt19937 mt(rnd());
      std::uniform_int_distribution<> rand10(0,count);
      if (rand10(mt) == 0) {
        p = movables[i];
      }
    }
  }
  delete Eval;
  Eval = NULL;
  
  this->ai_board.move(p);
  return p;
}

void AlphaBetaAI::sort(Board& board, std::vector<Point>& movables, int limit) {
  std::vector<Move> moves;

  for (unsigned i=0; i<movables.size(); i++) {
    int eval;

    board.move(movables[i]);
    eval = -alphabeta(board, limit-1, -INT_MAX, INT_MAX);
    board.undo();

    Move move(movables[i].x, movables[i].y, eval);
    moves.push_back(move);
  }

  std::stable_sort(moves.begin(), moves.end(), MoveGreater());

  movables.clear();
  for (unsigned i=0; i<moves.size(); i++) {
    movables.push_back(moves[i]);
  }

  return;
}

int AlphaBetaAI::alphabeta(Board& board, int limit, int alpha, int beta) {
  if (board.isGameOver() || limit == 0) {
    return Eval->evaluate(board);
  }
  int score;
  const std::vector<Point>& movables = board.getMovablePos();

  if (movables.empty()) {
    board.pass();
    score = -alphabeta(board, limit, -beta, -alpha);
    board.undo();
    return score;
  }

  for (unsigned i=0; i<movables.size(); i++) {
    board.move(movables[i]);
    score = -alphabeta(board, limit-1, -beta, -alpha);
    board.undo();

    if (score >= beta)
      return score;

    alpha = std::max(alpha, score);
  }

  return alpha;
}
