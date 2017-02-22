#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class Actor : public GraphObject {
 public:
  Actor(StudentWorld &student_world, int iid, int x, int y,
        Actor::Direction dir, int depth);
  virtual void doSomething() = 0;
  bool dead();
  void setDead(bool dead);
  void moveTo(int x, int y);

 private:
  bool dead_;
  StudentWorld &student_world_;
};

class Pebble : public Actor {
 public:
  Pebble(StudentWorld &student_world, int x, int y);
  void doSomething();
};

class BabyGrasshopper : public Actor {
 public:
  BabyGrasshopper(StudentWorld &student_world, int x, int y);
  void doSomething();

 private:
  Actor::Direction randomDirection();
  void randomMovement();
  bool withinBounds(Actor::Direction dir, int distance);

  StudentWorld &student_world_;
  int hit_points_;
  int sleep_ticks_;
};

class Food : public Actor {
 public:
  Food(StudentWorld &student_world, int x, int y, int food_points);
  void doSomething();

 private:
  int food_points_;
};

#endif  // ACTOR_H_
