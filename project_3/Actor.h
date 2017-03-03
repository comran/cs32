#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include "Compiler.h"

class StudentWorld;

// Actor identifier for use in searching for specific types of actors in the
// grid.
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

// ////////////////////////// BASE CLASS //////////////////////////////////// //
class Actor : public GraphObject {
 public:
  Actor(StudentWorld &student_world, ActorType actor_type, int iid,
        Coordinate coord, Actor::Direction dir, int depth, int initial_points);
  virtual ~Actor();

  // Getters.
  Coordinate getCoord() const;  // Get xy wrapped into a coordinate object.
  Actor::Direction randomDirection() const;  // Random direction for movement.
  StudentWorld &getStudentWorld() const;  // Getter for student world reference.
  ActorType getActorType() const;         // Identifier of the the actor's form.
  // Current number of points that the actor holds. Depending on the actor's
  // identifier, this can be used to store food content, hit points, etc.
  int getPoints() const;
  // Check if two objects share the same type.
  bool checkForObjectMatch(ActorType type) const;
  bool dead() const;  // Return whether the actor has died.

  // Move actor around the coordinate grid.
  void moveTo(Coordinate coord);
  // Change points and update death status if points fall below the threshold.
  void changePoints(int delta);

  virtual void doSomething() = 0;  // Method called every tick for each actor.
  virtual void poison();           // Called when poisoned.
  virtual void stun();             // Called when stunned.
  virtual void bite(Actor *bit_by, int damage);  // Called when bitten.
  virtual void die();                            // Death routine.

 private:
  ActorType actor_type_;
  StudentWorld &student_world_;
  bool dead_;
  int points_;
};

// //////////////////////// OBJECT CLASSES ////////////////////////////////// //
static const GraphObject::Direction kObjectStartingDirection =
    GraphObject::Direction::right;

// Food energy holders that insects can use to increase hit points (or carry, if
// the insect is an ant) when they are on the same square as food.
class Food : public Actor {
 public:
  Food(StudentWorld &student_world, Coordinate coord, int food_points);
  void doSomething();

 private:
  static const int kDepth = 2;
};

// Blocks insects from moving to the same square as the pebble.
class Pebble : public Actor {
 public:
  Pebble(StudentWorld &student_world, Coordinate coord);
  void doSomething();

 private:
  static const int kDepth = 1;
  static const int kInitialPoints = 0;  // Pebbles will not track their points.
};

// Poisons insects at its coordinate every tick, if the insect is affected
// by poison.
class Poison : public Actor {
 public:
  Poison(StudentWorld &student_world, Coordinate coord);
  void doSomething();

 private:
  static const int kDepth = 2;
  static const int kInitialPoints = 0;  // Poison will not track points.
};

// Slows down (stuns) insects that are affected by water.
class WaterPool : public Actor {
 public:
  WaterPool(StudentWorld &student_world, Coordinate coord);
  void doSomething();

 private:
  static const int kDepth = 2;
  static const int kInitialPoints = 0;  // Water will not track points.
};

// Tracer for ants as they navigate the field.
class Pheromone : public Actor {
 public:
  Pheromone(StudentWorld &student_world, Coordinate coord, int colony);
  void doSomething();
  static ActorType getActorType(int colony);

 private:
  static const int kDepth = 2;
  static const int kInitialPoints = 256;  // Pheromones will not track points.

  static int getImageForColony(int colony);
};

class AntHill : public Actor {
 public:
  AntHill(StudentWorld &student_world, int colony, Coordinate coord,
          Compiler *compiler);
  ~AntHill();
  void doSomething();
  static ActorType getActorTypeFromColony(int colony);

 private:
  static const int kDepth = 2;
  static const int kInitialHitPoints = 8999;
  const int kReserveHitPoints = 500;
  const int kAntHitPoints = 1500;
  const int kMaxFoodConsumptionPerTick = 10000;
  const int kPoisonDamage = 150;

  void giveBirth();
  Compiler *compiler_;
  int colony_;
};

// //////////////////////// INSECT CLASSES ////////////////////////////////// //
class Insect : public Actor {
 public:
  Insect(StudentWorld &student_world, int iid, Coordinate coord,
         ActorType actor_type, Actor::Direction direction, int depth,
         int points);
  void doSomething() = 0;
  // Reset stunned state
  void resetStunned();
  // Decrement sleep (if any) and return whether the insect is still sleeping.
  bool sleep();
  void addSleep(int sleep);

  virtual void die();
  virtual void stun();
  virtual void bite(Actor *actor, int damage);
  virtual void poison();

 private:
  const int kPoisonDamage = 150;

  bool moved_from_stunned_point_;
  int sleep_ticks_;
};

class Ant : public Insect {
 public:
  Ant(StudentWorld &student_world, int colony, Coordinate coord,
      Compiler *compiler, AntHill &my_anthill, int hit_points);
  void doSomething();
  void bite(Actor *actor, int damage);

  // Convert between raw colony number and ant actor type.
  static ActorType getActorTypeFromColony(int colony);
  static int getColonyFromActorType(ActorType actor_type);

 private:
  static const int kDepth = 1;
  const int kHungerThreshold = 25;
  const int kPheromoneStrength = 256;
  const int kBiteStrength = 25;
  const int kMaxFoodPerPickup = 400;
  const int kMaxTotalCarryingFood = 1800;
  const int kMaxFoodPerEating = 100;

  // Get image for an ant by the ant's raw colony number.
  static int getImageForColony(int colony);
  // Run a command and return whether another command should be run.
  bool runCommand(const Compiler::Command &c);

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
  void doSomething() = 0;
  // Move in a certain random direction until blocked by a pebble.
  void randomMovement();
  // Eat any food at the insects coordinate, up to the maximum. Returns whether
  // food was eaten.
  bool eatFood(int max_food);

 private:
  static const int kDepth = 1;
  static const int kMinRandomWalkRange = 2;
  static const int kMaxRandomWalkRange = 10;

  int distance_;
};
const int kInitialAdultGrasshopperHitPoints = 1600;
const int kMaxGrasshopperFoodPerEating = 200;

class BabyGrasshopper : public Grasshopper {
 public:
  BabyGrasshopper(StudentWorld &student_world, Coordinate coord);
  virtual void doSomething();

 private:
  static const int kInitialHitPoints = 500;
  const int kMaxFoodPerEating = 200;
};

class AdultGrasshopper : public Grasshopper {
 public:
  AdultGrasshopper(StudentWorld &student_world, Coordinate coord);
  void doSomething();
  void stun();
  void poison();
  void bite(Actor *bit_by, int damage);

 private:
  const int kBiteStrength = 50;
  const int kJumpRadius = 10;

  std::vector<ActorType> enemy_insects_;
};

#endif  // ACTOR_H_
