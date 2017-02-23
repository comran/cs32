#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

enum class ActorType { FOOD, PEBBLE, POISON, WATER_POOL, INSECT };
class StudentWorld;

// ////////////////////////// BASE CLASS //////////////////////////////////////
class Actor : public GraphObject {
 public:
  Actor(StudentWorld &student_world, ActorType actor_type, int iid, int x,
        int y, Actor::Direction dir, int depth, int initial_points);
  virtual void doSomething() = 0;
  virtual void poison();
  virtual void stun();
  bool dead();
  void die();
  void moveTo(int x, int y);
  bool checkForObjectMatch(ActorType type);
  int getPoints();
  void changePoints(int delta);
  StudentWorld &getStudentWorld();

 private:
  ActorType actor_type_;
  StudentWorld &student_world_;
  bool dead_;
  int points_;
};

// //////////////////////// OBJECT CLASSES /////////////////////////////////////
class Food : public Actor {
 public:
  Food(StudentWorld &student_world, int x, int y, int food_points);
  void doSomething();
  void increaseFood(int food_points);
};

class Pebble : public Actor {
 public:
  Pebble(StudentWorld &student_world, int x, int y);
  void doSomething();
};

class Poison : public Actor {
 public:
  Poison(StudentWorld &student_world, int x, int y);
  void doSomething();
};

class WaterPool : public Actor {
 public:
  WaterPool(StudentWorld &student_world, int x, int y);
  void doSomething();
};

// //////////////////////// INSECT CLASSES /////////////////////////////////////
class Insect : public Actor {
 public:
  Insect(StudentWorld &student_world, int iid, int x,
         int y, Actor::Direction direction, int depth, int points);
};

class BabyGrasshopper : public Insect {
 public:
  BabyGrasshopper(StudentWorld &student_world, int x, int y);
  void doSomething();
  void poison();
  void stun();

 private:
  Actor::Direction randomDirection();
  void randomMovement();
  bool withinBounds(Actor::Direction dir, int distance);

  bool moved_from_stunned_point_;
  int sleep_ticks_;
  int distance_;
};

#endif  // ACTOR_H_
