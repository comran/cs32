#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

Actor::Actor(StudentWorld &student_world, ActorType actor_type, int iid, int x,
             int y, Actor::Direction dir, int depth, int initial_points)
    : GraphObject(iid, x, y, dir, depth),
      actor_type_(actor_type),
      student_world_(student_world),
      dead_(false),
      points_(initial_points) {}

bool Actor::dead() { return dead_; }
void Actor::die() { dead_ = true; }

void Actor::moveTo(int x, int y) {
  student_world_.updatePositionInGrid(this, x, y);
}

bool Actor::checkForObjectMatch(ActorType type) { return actor_type_ == type; }

int Actor::getPoints() { return points_; };

void Actor::changePoints(int delta) { points_ += delta; }

Pebble::Pebble(StudentWorld &student_world, int x, int y)
    : Actor(student_world, ActorType::PEBBLE, IID_ROCK, x, y, right, 1, 0) {}

void Pebble::doSomething() {}

BabyGrasshopper::BabyGrasshopper(StudentWorld &student_world, int x, int y)
    : Actor(student_world, ActorType::BABY_GRASSHOPPER, IID_BABY_GRASSHOPPER, x, y,
            randomDirection(), 1, 500),
      student_world_(student_world),
      sleep_ticks_(0),
      distance_(randInt(2, 10)) {}

void BabyGrasshopper::doSomething() {
  // TODO(comran): Re-enable for part 2.
  /*
  changePoints(-1);  // Subtract one hit point.
  if (getPoints() <= 0) {
    student_world_.addFood(getX(), getY(), 100);
    die();
    setVisible(false);
    return;
  }
  */

  if (sleep_ticks_-- > 0) return;

  randomMovement();
  sleep_ticks_ = 2;
}

Actor::Direction BabyGrasshopper::randomDirection() {
  return static_cast<Actor::Direction>(randInt(1, 4));
}

void BabyGrasshopper::randomMovement() {
  static bool print = false;
  if(distance_ <= 0) {
    setDirection(randomDirection());
    distance_ = randInt(2, 10);
  }

  int x = getX(), y = getY();

  switch (getDirection()) {
    case right:
      x++;
      break;
    case left:
      x--;
      break;
    case up:
      y++;
      break;
    case down:
      y--;
      break;
    case none:
      break;
  }

  std::list<Actor *> pebbles_in_direction = student_world_.actorsOfTypeAt(ActorType::PEBBLE, x, y);
  if(pebbles_in_direction.size() > 0) distance_ = 0;

  if (distance_-- > 0) moveTo(x, y);
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

Food::Food(StudentWorld &student_world, int x, int y, int food_points)
    : Actor(student_world, ActorType::FOOD, IID_FOOD, x, y, right, 2, 0) {}

void Food::doSomething() {}

void Food::increaseFood(int food_points) { changePoints(food_points); }
