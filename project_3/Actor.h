#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

enum class ActorType { PEBBLE = 0, BABY_GRASSHOPPER = 1, FOOD = 2 };
class StudentWorld;

class Actor : public GraphObject {
 public:
  Actor(StudentWorld &student_world, ActorType actor_type, int iid, int x,
        int y, Actor::Direction dir, int depth, int initial_points);
  virtual void doSomething() = 0;
  bool dead();
  void die();
  void moveTo(int x, int y);
  bool checkForObjectMatch(ActorType type);
  int getPoints();
  void changePoints(int delta);

 private:
  ActorType actor_type_;
  StudentWorld &student_world_;
  bool dead_;
  int points_;
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
  int sleep_ticks_;
  int distance_;
};

class Food : public Actor {
 public:
  Food(StudentWorld &student_world, int x, int y, int food_points);
  void doSomething();
  void increaseFood(int food_points);
};

#endif  // ACTOR_H_
