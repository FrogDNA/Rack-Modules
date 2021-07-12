#include "Coordinate.hpp"

Coordinate::Coordinate(int aX, int aY) {
  x = aX;
  y = aY;
}

bool Coordinate::isEqual(Coordinate c) { return (c.x == x && c.y == y); }

int Coordinate::getX() { return x; }

int Coordinate::getY() { return y; }

bool operator==(const Coordinate &c1, const Coordinate &c2) {
  return (c1.x == c2.x && c1.y == c2.y);
}

bool operator<(const Coordinate &c1, const Coordinate &c2) {
  if (c1.x < c2.x) {
    return true;
  } else if (c1.x == c2.x && c1.y < c2.y) {
    return true;
  } else {
    return false;
  }
}
