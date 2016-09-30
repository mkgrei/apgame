#ifndef LHBOARD_H_INCLUDED
#define LHBOARD_H_INCLUDED

#include <iostream>
#include <vector>
#include <lh-ai/gamebasics.hpp>

const int BOARD_SIZE = 8;
const int MAX_TURNS = 60;

class Board {
  public:
    Board();

    void init();
    bool move(const Point& point);
    bool pass();
    bool undo();
    bool isGameOver();

    unsigned countDisc(Color color) const {
      return Discs[color];
    }
    Color getColor(const Point& p) const {
      return RawBoard[p.x][p.y];
    }
    std::vector<Point> getMovablePos() const {
      return MovablePos[Turns];
    }
    std::vector<Disc> getUpdate() const {
      if (UpdateLog.empty()) return std::vector<Disc>();
      else return UpdateLog.back();
    }
    Color getCurrentColor() const {
      return CurrentColor;
    }
    unsigned getTurns() const {
      return Turns;
    }
    void setCurrentTurn(unsigned Turn) {
      CurrentTurn = Turn;
    }
    unsigned getCurrentTurn() const {
      return CurrentTurn;
    }
    int getLiberty(Point p) const {
      return Liberty[p.x][p.y];
    }
    void print() {
      std::cout << " abcdefgh" << std::endl;
      unsigned tcb = 0;
      unsigned tcw = 0;
      unsigned tce = 0;
      for (int y=1; y<=8; y++) {
        std::cout << "" << y;
        unsigned cb = 0;
        unsigned cw = 0;
        unsigned ce = 0;
        for (int x=1; x<=8; x++) {
          switch(getColor(Point(x, y))) {
            case BLACK:
              std::cout << "B";
              cb++;
              break;
            case WHITE:
              std::cout << "W";
              cw++;
              break;
            default:
              std::cout << " ";
              ce++;
              break;
          }
        }
        tcb += cb;
        tcw += cw;
        tce += ce;
        std::cout << " B:" << cb << " W:" << cw << " E:" << ce << std::endl;
      }
      std::cout << " TB:" << tcb << " TW:" << tcw << " TE:" << tce << std::endl;
    }

  private:
    enum Direction {
      NONE = 0,
      UPPER = 1,
      UPPER_LEFT = 2,
      LEFT = 4,
      LOWER_LEFT = 8,
      LOWER = 16,
      LOWER_RIGHT = 32,
      RIGHT = 64,
      UPPER_RIGHT = 128,
    };

    Color RawBoard[BOARD_SIZE+2][BOARD_SIZE+2];
    int Liberty[BOARD_SIZE + 2][BOARD_SIZE + 2];
    unsigned Turns;
    unsigned CurrentTurn;
    Color CurrentColor;

    std::vector<std::vector<Disc> > UpdateLog;

    std::vector<Point> MovablePos[MAX_TURNS+1];
    
    unsigned MovableDir[MAX_TURNS+1][BOARD_SIZE+2][BOARD_SIZE+2];
    ColorStorage<unsigned> Discs;

    void flipDiscs(const Point& point);
    unsigned checkMobility(const Disc& disc);
    void initMovable();

};

#endif
