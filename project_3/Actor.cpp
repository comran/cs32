#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

Actor::Actor(StudentWorld &student_world, int iid, int x, int y,
             Actor::Direction dir, int depth)
    : GraphObject(iid, x, y, dir, depth),
      student_world_(student_world),
      dead_(false) {}

bool Actor::dead() { return dead_; }
void Actor::setDead(bool dead) { dead_ = dead; }

void Actor::moveTo(int x, int y) {
  student_world_.updatePositionInGrid(this, x, y);
  GraphObject::moveTo(x, y);
}

Pebble::Pebble(StudentWorld &student_world, int x, int y)
    : Actor(student_world, IID_ROCK, x, y, right, 1) {}

void Pebble::doSomething() {}

BabyGrasshopper::BabyGrasshopper(StudentWorld &student_world, int x, int y)
    : Actor(student_world, IID_BABY_GRASSHOPPER, x, y, randomDirection(), 0),
      student_world_(student_world),
      hit_points_(500),
      sleep_ticks_(0) {}

void BabyGrasshopper::doSomething() {
  if (sleep_ticks_ > 0) {
    sleep_ticks_--;
    return;
  }

  randomMovement();

  if (--hit_points_ <= 0) {
    student_world_.addFood(getX(), getY(), 100);
    setDead(true);
    setVisible(false);
    return;
  }
}

Actor::Direction BabyGrasshopper::randomDirection() {
  switch (randInt(0, 3)) {
    case 0:
      return BabyGrasshopper::right;
    case 1:
      return BabyGrasshopper::left;
    case 2:
      return BabyGrasshopper::up;
    case 3:
      return BabyGrasshopper::down;
    default:
      return BabyGrasshopper::none;
  }
}

void BabyGrasshopper::randomMovement() {
  Actor::Direction dir = randomDirection();
  int distance = randInt(2, 10);
  while (!withinBounds(dir, distance)) dir = randomDirection();

  setDirection(dir);
  switch (dir) {
    case right:
      moveTo(getX() + distance, getY());
      break;
    case left:
      moveTo(getX() - distance, getY());
      break;
    case up:
      moveTo(getX(), getY() + distance);
      break;
    case down:
      moveTo(getX(), getY() - distance);
      break;
    case none:
      break;
  }
}

bool BabyGrasshopper::withinBounds(Direction dir, int distance) {
  switch (dir) {
    case right:
      if (getX() + distance >= VIEW_WIDTH) return false;
      break;
    case left:
      if (getX() - distance < 0) return false;
      break;
    case up:
      if (getY() + distance >= VIEW_HEIGHT) return false;
      break;
    case down:
      if (getY() - distance < 0) return false;
      break;
    case none:
      break;
  }

  return true;
}

Food::Food(StudentWorld &student_world, int x, int y, int food_points) : Actor(student_world, IID_FOOD, x, y, right, 2) {}

void Food::doSomething() {}
