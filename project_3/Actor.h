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
  ANT_HILL0,
  ANT_HILL1,
  ANT_HILL2,
  ANT_HILL3
};
class StudentWorld;

// ////////////////////////// BASE CLASS //////////////////////////////////////
class Actor : public GraphObject {
 public:
  Actor(StudentWorld &student_world, ActorType actor_type, int iid,
        Coordinate coord, Actor::Direction dir, int depth, int initial_points);
  Coordinate getCoord() const;
  Actor::Direction randomDirection() const;
  StudentWorld &getStudentWorld() const;
  ActorType getActorType() const;
  bool checkForObjectMatch(ActorType type) const;
  int getPoints() const;
  void moveTo(Coordinate coord);
  void changePoints(int delta);

  virtual void doSomething() = 0;
  virtual void poison();
  virtual void stun();
  virtual void feed(int &available_food);
  virtual void die();
  virtual void bite(Actor* bit_by, int damage);
  virtual bool dead();

 private:
  ActorType actor_type_;
  StudentWorld &student_world_;
  bool dead_;
  int points_;
};

// //////////////////////// OBJECT CLASSES /////////////////////////////////////
class Food : public Actor {
 public:
  Food(StudentWorld &student_world, Coordinate coord, int food_points);
  void doSomething();
  void changePoints(int delta);
};

class Pebble : public Actor {
 public:
  Pebble(StudentWorld &student_world, Coordinate coord);
  void doSomething();
};

class Poison : public Actor {
 public:
  Poison(StudentWorld &student_world, Coordinate coord);
  void doSomething();
};

class WaterPool : public Actor {
 public:
  WaterPool(StudentWorld &student_world, Coordinate coord);
  void doSomething();
};

class Pheromone : public Actor {
 public:
  Pheromone(StudentWorld &student_world, Coordinate coord, int colony);
  void doSomething();
  static ActorType getActorType(int colony);

 private:
  static int getImageForColony(int colony);
};

class AntHill : public Actor {
 public:
  AntHill(StudentWorld &student_world, int colony, Coordinate coord,
          Compiler *compiler);
  void doSomething();
  static ActorType getActorTypeFromColony(int colony);

 private:
  void giveBirth();
  Compiler *compiler_;
  int colony_;
};

// //////////////////////// INSECT CLASSES /////////////////////////////////////
class Insect : public Actor {
 public:
  Insect(StudentWorld &student_world, int iid, Coordinate coord,
         ActorType actor_type, Actor::Direction direction, int depth,
         int points);
  void die();
  void stun();
  void resetStunned();
  void addSleep(int sleep);
  bool sleep();
  void poison();
  bool eatFood(int max_food);
  virtual void bite(Actor* actor, int damage);

 private:
  bool moved_from_stunned_point_;
  int sleep_ticks_;
};

class Ant : public Insect {
 public:
  Ant(StudentWorld &student_world, int colony, Coordinate coord,
      Compiler *compiler, AntHill &my_anthill);
  void doSomething();
  virtual void bite(Actor* actor, int damage);
  static ActorType getActorTypeFromColony(int colony);
  static int getColonyFromActorType(ActorType actor_type);

 private:
  bool runCommand(const Compiler::Command &c);
  static int getImageForColony(int colony);
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
  Grasshopper(StudentWorld &student_world, int iid, Coordinate coord,
              int points);
  void randomMovement();
  void feed(int &availableFood);

 private:
  int distance_;
};

class BabyGrasshopper : public Grasshopper {
 public:
  BabyGrasshopper(StudentWorld &student_world, Coordinate coord);
  void doSomething();
};

class AdultGrasshopper : public Grasshopper {
 public:
  AdultGrasshopper(StudentWorld &student_world, Coordinate coord);
  void doSomething();
  void stun();
  void poison();
  void bite(Actor* bit_by, int damage);

 private:
  std::vector<ActorType> enemy_insects_;
};

#endif  // ACTOR_H_
