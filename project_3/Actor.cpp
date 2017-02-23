#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// ////////////////////////// BASE CLASS //////////////////////////////////////
Actor::Actor(StudentWorld &student_world, ActorType actor_type, int iid, int x,
             int y, Actor::Direction dir, int depth, int initial_points)
    : GraphObject(iid, x, y, dir, depth),
      actor_type_(actor_type),
      student_world_(student_world),
      dead_(false),
      points_(initial_points) {}
void Actor::poison() { }
void Actor::stun() { }
bool Actor::dead() { return dead_; }
void Actor::die() { dead_ = true; }
int Actor::getPoints() { return points_; };
void Actor::changePoints(int delta) { points_ += delta; }
bool Actor::checkForObjectMatch(ActorType type) { return actor_type_ == type; }
StudentWorld &Actor::getStudentWorld() { return student_world_; }

void Actor::moveTo(int x, int y) {
  student_world_.updatePositionInGrid(this, x, y);
}

// //////////////////////// OBJECT CLASSES /////////////////////////////////////
Food::Food(StudentWorld &student_world, int x, int y, int food_points)
    : Actor(student_world, ActorType::FOOD, IID_FOOD, x, y, right, 2, 0) {}
void Food::increaseFood(int food_points) { changePoints(food_points); }

void Food::doSomething() {}


Pebble::Pebble(StudentWorld &student_world, int x, int y)
    : Actor(student_world, ActorType::PEBBLE, IID_ROCK, x, y, right, 1, 0) {}

void Pebble::doSomething() {}


Poison::Poison(StudentWorld &student_world, int x, int y)
    : Actor(student_world, ActorType::POISON, IID_POISON, x, y, right, 2, 0) {}

void Poison::doSomething() {
  std::list<Actor *> insects_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::INSECT, getX(), getY());
  for (std::list<Actor *>::const_iterator i = insects_at_point.begin();
       i != insects_at_point.end(); i++) {
    if((*i)->dead()) continue;
    (*i)->poison();
  }
}

WaterPool::WaterPool(StudentWorld &student_world, int x, int y)
    : Actor(student_world, ActorType::WATER_POOL, IID_WATER_POOL, x, y, right, 2, 0) {}

void WaterPool::doSomething() {
  std::list<Actor *> insects_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::INSECT, getX(), getY());
  for (std::list<Actor *>::const_iterator i = insects_at_point.begin();
       i != insects_at_point.end(); i++) {
    if((*i)->dead()) continue;
    (*i)->stun();
  }
}

// //////////////////////// INSECT CLASSES /////////////////////////////////////
Insect::Insect(StudentWorld &student_world, int iid,
               int x, int y, Actor::Direction direction, int depth, int points)
    : Actor(student_world, ActorType::INSECT, iid, x, y, direction, depth, points) {}

BabyGrasshopper::BabyGrasshopper(StudentWorld &student_world, int x, int y)
    : Insect(student_world, IID_BABY_GRASSHOPPER,
             x, y, randomDirection(), 1, 500),
      sleep_ticks_(0),
      distance_(randInt(2, 10)),
      moved_from_stunned_point_(true) {}

void BabyGrasshopper::doSomething() {
  changePoints(-1);  // Subtract one hit point.
  if (getPoints() <= 0) {
    getStudentWorld().addFood(getX(), getY(), 100);
    die();
    setVisible(false);
    return;
  }

  if (sleep_ticks_-- > 0) return;

  randomMovement();
  sleep_ticks_ = 2;
}

void BabyGrasshopper::poison() { changePoints(-150); }
void BabyGrasshopper::stun() {
  if(!moved_from_stunned_point_) return;
  sleep_ticks_ = 2;
  moved_from_stunned_point_ = false;
}

Actor::Direction BabyGrasshopper::randomDirection() {
  return static_cast<Actor::Direction>(randInt(1, 4));
}

void BabyGrasshopper::randomMovement() {
  if (distance_ <= 0) {
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

  std::list<Actor *> pebbles_in_direction =
      getStudentWorld().actorsOfTypeAt(ActorType::PEBBLE, x, y);
  if (pebbles_in_direction.size() > 0) distance_ = 0;

  if (distance_-- > 0) {
    moveTo(x, y);
    moved_from_stunned_point_ = true;
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
