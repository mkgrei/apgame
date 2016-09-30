#ifndef LHGAMEBASICS_H_INCLUDED
#define LHGAMEBASICS_H_INCLUDED

#include <string>
#include <stdexcept>
#include <sstream>

typedef int Color;
const Color EMPTY =  0;
const Color WHITE = -1;
const Color BLACK =  1;
const Color WALL  =  2;

struct Point {
  int x;
  int y;

  Point() {
    Point(0, 0);
  }

  Point(int x, int y) {
    this->x = x;
    this->y = y;
  }

  Point(std::string coordstr) throw(std::invalid_argument) {
    if (coordstr.length() < 2)
      throw std::invalid_argument("string point should be from a1 to h8");
    x = coordstr[0] - 'a' +1;
    y = coordstr[1] - '1' +1;
  }

  operator std::string() const {
    std::ostringstream oss;
    oss << static_cast<char>( 'a' +x-1 )
        << static_cast<char>( '1' +y-1 );
    return oss.str();
  }

  bool operator ==(const Point& p) const {
    return (this->x==p.x && this->y==p.y);
  }
};

struct Disc : public Point {
  Color color;

  Disc() : Point(0, 0) {
    color = EMPTY;
  }

  Disc(int x, int y, Color color) : Point(x, y) {
    this->color = color;
  }
};

template<typename T> class ColorStorage {
  private:
    T data[3];
  
  public:
    T& operator[](Color color) {
      return data[color+1];
    }

    const T& operator[](Color color) const {
      return data[color+1];
    }

  ColorStorage<T>& operator +=(const ColorStorage<T> &src)
  {
    data[0] += src.data[0];
    data[1] += src.data[1];
    data[2] += src.data[2];

    return *this;
  }
};

#endif
