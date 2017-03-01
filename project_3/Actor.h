#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include "Compiler.h"

enum class ActorType {
  FOOD,
  PEBBLE,
  POISON,
  WATER_POOL,
  GRASSHOPPER,
  ANT0,
  ANT1,
  ANT2,
  ANT3,
  PHEROMONE0,
  PHEROMONE1,
  PHEROMONE2,
  PHEROMONE3,
  ANT_HILL
};
class StudentWorld;

// ////////////////////////// BASE CLASS //////////////////////////////////////
class Actor : public GraphObject {
 public:
  Actor(StudentWorld &student_world, ActorType actor_type, int iid, int x,
        int y, Actor::Direction dir, int depth, int initial_points);
  virtual void doSomething() = 0;
  virtual void poison();
  virtual void stun();
  virtual void feed(int &available_food);
  virtual void die();
  virtual void bite();
  bool dead();
  void moveTo(int x, int y);
  bool checkForObjectMatch(ActorType type);
  ActorType getActorType();
  int getPoints();
  void changePoints(int delta);
  Actor::Direction randomDirection();
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

class Pheromone : public Actor {
 public:
  Pheromone(StudentWorld &student_world, int colony, int x, int y);
  void doSomething();
  static ActorType getActorType(int colony);

 private:
  int getImageForColony(int colony);
};

class AntHill : public Actor {
 public:
  AntHill(StudentWorld &student_world, int colony, int x, int y,
          Compiler *compiler);
  void doSomething();

 private:
  void giveBirth();
  Compiler *compiler_;
  int colony_;
  int food_eaten_;
};

// //////////////////////// INSECT CLASSES /////////////////////////////////////
class Insect : public Actor {
 public:
  Insect(StudentWorld &student_world, int iid, int x, int y,
         ActorType actor_type, Actor::Direction direction, int depth,
         int points);
  void die();
  void stun();
  void resetStunned();
  void addSleep(int sleep);
  bool sleep();

 private:
  bool moved_from_stunned_point_;
  int sleep_ticks_;
};

class Ant : public Insect {
 public:
  Ant(StudentWorld &student_world, int colony, int x, int y, Compiler *compiler,
      AntHill &my_anthill);
  void doSomething();
  static ActorType getActorTypeFromColony(int colony);
  static int getColonyFromActorType(ActorType actor_type);
  virtual void bite();

 private:
  bool runCommand(const Compiler::Command &c);
  int getImageForColony(int colony);
  Compiler *compiler_;
  int instruction_counter_;
  bool blocked_by_pebble_;
  int last_random_number_;
  int food_carried_;
  bool was_bit_;
  std::vector<ActorType> other_insects_;
  AntHill &my_ant_hill_;
};

class Grasshopper : public Insect {
 public:
  Grasshopper(StudentWorld &student_world, int iid, int x, int y, int points);
  void randomMovement();
  void feed(int &availableFood);

 private:
  bool withinBounds(Actor::Direction dir, int distance);

  int distance_;
};

class BabyGrasshopper : public Grasshopper {
 public:
  BabyGrasshopper(StudentWorld &student_world, int x, int y);
  void doSomething();
  void poison();
};

class AdultGrasshopper : public Grasshopper {
 public:
  AdultGrasshopper(StudentWorld &student_world, int x, int y);
  void doSomething();
  void stun();
};

#endif  // ACTOR_H_
