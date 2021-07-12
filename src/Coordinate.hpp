#ifndef COORDINATE_H
#define COORDINATE_H

class Coordinate {
  int x;
  int y;

public:
  int getX();
  int getY();
  Coordinate(int aX, int aY);
  bool isEqual(Coordinate c);
  friend bool operator==(const Coordinate &c1, const Coordinate &c2);
  friend bool operator<(const Coordinate &c1, const Coordinate &c2);
};

#endif
