#include <algorithm>
#include <cmath>

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// ////////////////////////// BASE CLASS ///////////////////////////////////////
Actor::Actor(StudentWorld &student_world, ActorType actor_type, int iid,
             Coordinate coord, Actor::Direction dir, int depth,
             int initial_points)
    : GraphObject(iid, coord.getX(), coord.getY(), dir, depth),
      actor_type_(actor_type),
      student_world_(student_world),
      dead_(false),
      points_(initial_points) {}
Actor::~Actor() {}
ActorType Actor::getActorType() const { return actor_type_; }
int Actor::getPoints() const { return points_; };
StudentWorld &Actor::getStudentWorld() const { return student_world_; }
Coordinate Actor::getCoord() const { return Coordinate(getX(), getY()); }
bool Actor::dead() const { return dead_; }
Actor::Direction Actor::randomDirection() const {
  // Choose a random direction by choosing a random direction from the enum
  // class based on the integer value of each enum element. (with up being of
  // least value 1 and left being of greatest value 4)
  return static_cast<Actor::Direction>(randInt(1, 4));
}
bool Actor::checkForObjectMatch(ActorType type) const {
  return actor_type_ == type;
}

void Actor::changePoints(int delta) {
  points_ += delta;
  if (getPoints() < 1) die();
}
void Actor::moveTo(Coordinate coord) {
  // Cap X and Y within bounds.
  coord.setX(std::max(0, coord.getX()));
  coord.setX(std::min(VIEW_WIDTH, coord.getX()));
  coord.setY(std::max(0, coord.getY()));
  coord.setY(std::min(VIEW_HEIGHT, coord.getY()));

  student_world_.updatePositionInGrid(this, coord);
}
void Actor::die() { dead_ = true; setVisible(false); }
void Actor::bite(Actor *bit_by, int damage) {
  // Actor defaults to doing nothing when bitten.
}
void Actor::poison() {
  // Actor defaults to doing nothing when poisoned.
}
void Actor::stun() {
  // Actor defaults to doing nothing when stunned.
}

// //////////////////////// OBJECT CLASSES /////////////////////////////////////
Food::Food(StudentWorld &student_world, Coordinate coord, int food_points)
    : Actor(student_world, ActorType::FOOD, IID_FOOD, coord,
            kObjectStartingDirection, kDepth, food_points) {}
void Food::doSomething() {}

Pebble::Pebble(StudentWorld &student_world, Coordinate coord)
    : Actor(student_world, ActorType::PEBBLE, IID_ROCK, coord,
            kObjectStartingDirection, kDepth, kInitialPoints) {}
void Pebble::doSomething() {}

Poison::Poison(StudentWorld &student_world, Coordinate coord)
    : Actor(student_world, ActorType::POISON, IID_POISON, coord,
            kObjectStartingDirection, kDepth, kInitialPoints) {}
void Poison::doSomething() {
  // Generate a list of all types of insects.
  std::vector<ActorType> insects;
  insects.push_back(ActorType::GRASSHOPPER);
  for (int i = 0; i < 4; i++) {
    insects.push_back(Ant::getActorTypeFromColony(i));
  }

  // Find all insects at the coordinate of the poison (using the list generated
  // above) and poison all living insects that are in that list.
  std::list<Actor *> insects_at_point =
      getStudentWorld().actorsOfTypesAt(insects, getCoord());
  for (std::list<Actor *>::const_iterator i = insects_at_point.begin();
       i != insects_at_point.end(); i++) {
    if ((*i)->dead()) continue;

    (*i)->poison();
  }
}

WaterPool::WaterPool(StudentWorld &student_world, Coordinate coord)
    : Actor(student_world, ActorType::WATER_POOL, IID_WATER_POOL, coord,
            kObjectStartingDirection, kDepth, kInitialPoints) {}
void WaterPool::doSomething() {
  // Generate a list of all types of insects.
  std::vector<ActorType> insects;
  for (int i = 0; i < 4; i++) {
    insects.push_back(Ant::getActorTypeFromColony(i));
  }

  // Find all insects at the coordinate of the water (using the list generated
  // above) and stun all living insects that are in that list.
  std::list<Actor *> insects_at_point =
      getStudentWorld().actorsOfTypesAt(insects, getCoord());
  for (std::list<Actor *>::const_iterator i = insects_at_point.begin();
       i != insects_at_point.end(); i++) {
    if ((*i)->dead()) continue;

    (*i)->stun();
  }
}

Pheromone::Pheromone(StudentWorld &student_world, Coordinate coord, int colony)
    : Actor(student_world, getActorType(colony), getImageForColony(colony),
            coord, kObjectStartingDirection, kDepth, kInitialPoints) {}
void Pheromone::doSomething() {
  changePoints(-1);
}

ActorType Pheromone::getActorType(int colony) {
  ActorType pheromone_actor_types[] = {
      ActorType::PHEROMONE0, ActorType::PHEROMONE1, ActorType::PHEROMONE2,
      ActorType::PHEROMONE3};

  return pheromone_actor_types[colony];
}
int Pheromone::getImageForColony(int colony) {
  int pheromone_iids[] = {IID_PHEROMONE_TYPE0, IID_PHEROMONE_TYPE1,
                          IID_PHEROMONE_TYPE2, IID_PHEROMONE_TYPE3};
  return pheromone_iids[colony];
}

AntHill::AntHill(StudentWorld &student_world, int colony, Coordinate coord,
                 Compiler *compiler)
    : Actor(student_world, getActorTypeFromColony(colony), IID_ANT_HILL, coord,
            kObjectStartingDirection, kDepth, kInitialHitPoints),
      compiler_(compiler),
      colony_(colony) {}
void AntHill::doSomething() {
  changePoints(-1);
  if (dead()) return;

  // Eat food at this square, up to a maximum number per tick.
  std::list<Actor *> food_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getCoord());
  for (std::list<Actor *>::const_iterator i = food_at_point.begin();
       i != food_at_point.end(); i++) {
    int food_available =
        std::min((*i)->getPoints(), kMaxFoodConsumptionPerTick);

    // Swap points from the food to this ant hill.
    changePoints(food_available);
    (*i)->changePoints(-1 * food_available);
    break;
  }

  if (getPoints() >= kReserveHitPoints + kAntHitPoints) {
    giveBirth();
    changePoints(-1 * kAntHitPoints);
  }
}

void AntHill::giveBirth() {
  // Add a new ant on top of the ant hill and update the scoreboard to account
  // for this ant hill's success in producing a new ant.
  getStudentWorld().addActor(new Ant(getStudentWorld(), colony_, getCoord(),
                                     compiler_, *this, kAntHitPoints));
  getStudentWorld().updateScoreboard(colony_);
}

ActorType AntHill::getActorTypeFromColony(int colony) {
  ActorType ant_hill_colony_to_actor_type[] = {
      ActorType::ANT_HILL0, ActorType::ANT_HILL1, ActorType::ANT_HILL2,
      ActorType::ANT_HILL3};

  return ant_hill_colony_to_actor_type[colony];
}

// //////////////////////// INSECT CLASSES /////////////////////////////////////
Insect::Insect(StudentWorld &student_world, int iid, Coordinate coord,
               ActorType actor_type, Actor::Direction direction, int depth,
               int points)
    : Actor(student_world, actor_type, iid, coord, direction, depth, points),
      sleep_ticks_(0),
      moved_from_stunned_point_(true) {}

void Insect::die() {
  // Add food to the coordinate where the insect died and set up insect to be
  // removed from the grid.
  getStudentWorld().addFood(getCoord(), 100);
  Actor::die();
}

void Insect::stun() {
  // Sleep for two ticks if the insect was stunned, and ensure that the insect
  // does not continuously get stunned in the same spot.
  if (!moved_from_stunned_point_) return;
  addSleep(2);
  moved_from_stunned_point_ = false;
}

void Insect::resetStunned() { moved_from_stunned_point_ = true; }

void Insect::addSleep(int sleep) { sleep_ticks_ += sleep; }

bool Insect::sleep() {
  if (sleep_ticks_ > 0) {
    sleep_ticks_--;
    return true;
  }

  return false;
}

void Insect::bite(Actor *actor, int damage) { changePoints(-1 * damage); }
void Insect::poison() { changePoints(-1 * kPoisonDamage); }

Ant::Ant(StudentWorld &student_world, int colony, Coordinate coord,
         Compiler *compiler, AntHill &my_ant_hill, int hit_points)
    : Insect(student_world, getImageForColony(colony), coord,
             getActorTypeFromColony(colony), randomDirection(), kDepth,
             hit_points),
      compiler_(compiler),
      my_ant_hill_(my_ant_hill),
      blocked_by_pebble_(false),
      was_bit_(false),
      last_random_number_(0),
      food_carried_(0),
      instruction_counter_(0) {
  // Populate the list of enemy insects.
  other_insects_.push_back(ActorType::ANT0);
  other_insects_.push_back(ActorType::ANT1);
  other_insects_.push_back(ActorType::ANT2);
  other_insects_.push_back(ActorType::ANT3);
  other_insects_.push_back(ActorType::GRASSHOPPER);

  // Remove our own actor type from the list of enemy insects.
  for (int i = 0; i < other_insects_.size(); i++) {
    if (other_insects_[i] == Actor::getActorType()) {
      other_insects_.erase(other_insects_.begin() + i);
      break;
    }
  }
}

void Ant::doSomething() {
  changePoints(-1);
  if (getPoints() <= 0) {
    die();
    return;
  }

  if (sleep()) return;

  // Run commands until an action is perform by the ant, up to a total of 10
  // commands.
  Compiler::Command command;
  int commands_this_tick = 0;
  do {
    if (!compiler_->getCommand(instruction_counter_++, command)) {
      die();
      return;
    }
  } while (runCommand(command) && ++commands_this_tick < 10);
}

ActorType Ant::getActorTypeFromColony(int colony) {
  ActorType ant_colony_to_actor_type[] = {ActorType::ANT0, ActorType::ANT1,
                                          ActorType::ANT2, ActorType::ANT3};
  return ant_colony_to_actor_type[colony];
}

int Ant::getColonyFromActorType(ActorType actor_type) {
  std::map<ActorType, int> ant_actor_type_to_colony = {{ActorType::ANT0, 0},
                                                       {ActorType::ANT1, 1},
                                                       {ActorType::ANT2, 2},
                                                       {ActorType::ANT3, 3}};

  return ant_actor_type_to_colony[actor_type];
}

bool Ant::runCommand(const Compiler::Command &c) {
  switch (c.opcode) {
    case Compiler::goto_command: {
      // Set the next instruction to the one specified by the operand.
      instruction_counter_ = stoi(c.operand1);
      return true;
    }
    case Compiler::if_command: {
      switch (stoi(c.operand1)) {
        // If any of these cases are true, jump to the instruction specified.

        case Compiler::i_smell_danger_in_front_of_me: {
          // Check to see if there are enemy insects, water, or poison in the
          // square directly in front of the insect.
          Coordinate coord_in_front =
              getCoord().coordInDirection(getDirection());

          std::vector<ActorType> stranger_danger = other_insects_;
          stranger_danger.push_back(ActorType::WATER_POOL);
          stranger_danger.push_back(ActorType::POISON);

          if (getStudentWorld()
                  .actorsOfTypesAt(stranger_danger, coord_in_front)
                  .size() > 0)
            instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::i_smell_pheromone_in_front_of_me: {
          // Search for pheromones on the current coordinate.
          Coordinate coord_in_front =
              getCoord().coordInDirection(getDirection());
          std::list<Actor *> pheromone = getStudentWorld().actorsOfTypeAt(
              Pheromone::getActorType(getColonyFromActorType(getActorType())),
              coord_in_front);

          if (pheromone.size() > 0) {
            // Do not account for pheromones with no strength.
            if ((*pheromone.begin())->getPoints() < 1) break;
            instruction_counter_ = stoi(c.operand2);
          }
          break;
        }
        case Compiler::i_was_bit: {
          if (was_bit_) instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::i_am_carrying_food: {
          if (food_carried_ > 0) instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::i_am_hungry: {
          if (getPoints() <= kHungerThreshold)
            instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::i_am_standing_on_my_anthill: {
          // Search for an ant hill at the current coordinate that corresponds
          // to this ant's colony.
          std::list<Actor *> anthill = getStudentWorld().actorsOfTypeAt(
              AntHill::getActorTypeFromColony(
                  getColonyFromActorType(getActorType())),
              getCoord());

          if (anthill.size() > 0) {
            if ((*anthill.begin())->dead()) break;

            instruction_counter_ = stoi(c.operand2);
          }
          break;
        }
        case Compiler::i_am_standing_on_food: {
          // Search for food at the current coordinate.
          std::list<Actor *> food =
              getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getCoord());

          if (food.size() > 0) {
            // Don't count food that has no energy stored.
            if ((*food.begin())->getPoints() < 1) break;
            instruction_counter_ = stoi(c.operand2);
          }
          break;
        }
        case Compiler::i_am_standing_with_an_enemy: {
          // Search for enemies at the current coordinate.
          std::list<Actor *> enemies =
              getStudentWorld().actorsOfTypesAt(other_insects_, getCoord());

          // Make sure at least one of the enemies in the list is alive.
          bool alive_enemy = false;
          for (std::list<Actor *>::const_iterator i = enemies.begin();
               i != enemies.end(); i++) {
            if (!(*i)->dead()) {
              alive_enemy = true;
              break;
            }
          }

          instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::i_was_blocked_from_moving: {
          if (blocked_by_pebble_) instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::last_random_number_was_zero: {
          if (last_random_number_ == 0) instruction_counter_ = stoi(c.operand2);
          break;
        }
      }

      return true;
    }
    case Compiler::invalid: {
      std::cerr << "INVALID COMMAND" << std::endl;
      die();
      return false;
    }
    case Compiler::rotateClockwise: {
      // Rotate clockwise by increasing the direction enum value. If the raw
      // direction number maxes out (when it equals left), wrap so that the next
      // direction is up.
      int direction_int = static_cast<int>(getDirection());
      direction_int++;
      if (static_cast<Actor::Direction>(direction_int) ==
          Actor::Direction::left)
        direction_int = Actor::Direction::up;

      setDirection(static_cast<Actor::Direction>(direction_int));
      return false;
    }
    case Compiler::rotateCounterClockwise: {
      // Rotate counterclockwise by decreasing the direction enum value. If the
      // raw direction number reaches minimum (when it equals up), wrap so that
      // the next direction is left.
      int direction_int = static_cast<int>(getDirection());
      direction_int--;
      if (static_cast<Actor::Direction>(direction_int) == Actor::Direction::up)
        direction_int = Actor::Direction::left;

      setDirection(static_cast<Actor::Direction>(direction_int));
      return false;
    }
    case Compiler::faceRandomDirection: {
      setDirection(randomDirection());
      return false;
    }
    case Compiler::emitPheromone: {
      getStudentWorld().addPheromone(
          getCoord(), kPheromoneStrength,
          getColonyFromActorType(Actor::getActorType()));
      return false;
    }
    case Compiler::dropFood: {
      getStudentWorld().addFood(getCoord(), food_carried_);
      food_carried_ = 0;
      return false;
    }
    case Compiler::moveForward: {
      blocked_by_pebble_ = false;

      // Attempt to move one square in the current direction.
      int x = getX(), y = getY();
      switch (getDirection()) {
        case Direction::right:
          x++;
          break;
        case Direction::left:
          x--;
          break;
        case Direction::up:
          y++;
          break;
        case Direction::down:
          y--;
          break;
        default:
          break;
      }

      // Check to see if a pebble is blocking the path.
      std::list<Actor *> pebbles_in_direction =
          getStudentWorld().actorsOfTypeAt(ActorType::PEBBLE, Coordinate(x, y));
      if (pebbles_in_direction.size() > 0) {
        blocked_by_pebble_ = true;  // Remember the block for the next tick.
        return false;
      }

      // Move was successful, so reset conditions that only lasted when the ant
      // had not moved from the current square and proceed to move the ant to
      // the new square.
      resetStunned();
      was_bit_ = false;
      moveTo(Coordinate(x, y));
      return false;
    }
    case Compiler::label:
      return false;
    case Compiler::bite: {
      // Find enemy actors at the given coordinate to bite.
      std::list<Actor *> other_insects_at_point =
          getStudentWorld().actorsOfTypesAt(other_insects_, getCoord());

      for (std::list<Actor *>::const_iterator i =
               other_insects_at_point.begin();
           i != other_insects_at_point.end(); i++) {
        if ((*i)->dead()) continue;
        (*i)->bite(this, kBiteStrength);
      }

      return false;
    }
    case Compiler::pickupFood: {
      std::list<Actor *> food_at_point =
          getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getCoord());
      if (food_at_point.size() < 1) return false;  // No food at point.

      int held_food = (*food_at_point.begin())->getPoints();
      held_food = std::min(held_food, kMaxFoodPerPickup);
      held_food = std::min(held_food, kMaxTotalCarryingFood - food_carried_);
      food_carried_ += held_food;
      (*food_at_point.begin())->changePoints(-1 * held_food);

      return false;
    }
    case Compiler::eatFood: {
      int food_eaten = std::min(food_carried_, kMaxFoodPerEating);
      changePoints(food_eaten);
      food_carried_ -= food_eaten;

      return false;
    }
    case Compiler::generateRandomNumber: {
      last_random_number_ = randInt(0, stoi(c.operand1) - 1);

      return false;
    }
  }
}

int Ant::getImageForColony(int colony) {
  int ant_iids[] = {IID_ANT_TYPE0, IID_ANT_TYPE1, IID_ANT_TYPE2, IID_ANT_TYPE3};
  return ant_iids[colony];
}

void Ant::bite(Actor *bit_by, int damage) {
  Insect::bite(bit_by, damage);
  was_bit_ = true;  // Track whether the ant was bit at the current square.
}

Grasshopper::Grasshopper(StudentWorld &student_world, int iid, Coordinate coord,
                         int points)
    : Insect(student_world, iid, coord, ActorType::GRASSHOPPER,
             randomDirection(), kDepth, points),
      distance_(randInt(kMinRandomWalkRange, kMaxRandomWalkRange)) {}

void Grasshopper::randomMovement() {
  // Pick a new direction and distance once the last walk finishes.
  if (distance_ <= 0) {
    setDirection(randomDirection());
    distance_ = randInt(kMinRandomWalkRange, kMaxRandomWalkRange);
  }

  Coordinate coord_in_direction = getCoord().coordInDirection(getDirection());
  std::list<Actor *> pebbles_in_direction =
      getStudentWorld().actorsOfTypeAt(ActorType::PEBBLE, coord_in_direction);

  // Don't walk into pebbles.
  if (pebbles_in_direction.size() > 0) distance_ = 0;

  if (distance_-- > 0) {
    moveTo(coord_in_direction);
    resetStunned();
  }
}

bool Grasshopper::eatFood(int max_food) {
  // Find any food objects at the given point.
  std::list<Actor *> food_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getCoord());

  // Exit if there is no food at the coordinate or the food there has no points.
  if (food_at_point.size() < 1 || (*food_at_point.begin())->getPoints() < 1)
    return false;

  // Cap food available to what is at the square and what the parameter says to
  // cap it at.
  int food_available = (*food_at_point.begin())->getPoints();
  food_available = std::min(food_available, max_food);

  // Transfer the food energy.
  (*food_at_point.begin())->changePoints(-1 * food_available);
  changePoints(food_available);
  if (food_available > 0) return true;

  return false;
}

BabyGrasshopper::BabyGrasshopper(StudentWorld &student_world, Coordinate coord)
    : Grasshopper(student_world, IID_BABY_GRASSHOPPER, coord,
                  kInitialHitPoints) {}

void BabyGrasshopper::doSomething() {
  changePoints(-1);

  if (getPoints() <= 0) {
    die();
    return;
  } else if (getPoints() >= kInitialAdultGrasshopperHitPoints) {
    getStudentWorld().addActor(
        new AdultGrasshopper(getStudentWorld(), getCoord()));
    die();
  }

  if (sleep()) return;
  if (!(eatFood(kMaxGrasshopperFoodPerEating) && randInt(0, 1) == 0))
    randomMovement();

  addSleep(2);
}

AdultGrasshopper::AdultGrasshopper(StudentWorld &student_world,
                                   Coordinate coord)
    : Grasshopper(student_world, IID_ADULT_GRASSHOPPER, coord,
                  kInitialAdultGrasshopperHitPoints) {
  // Populate list of enemy insects.
  enemy_insects_.push_back(ActorType::ANT0);
  enemy_insects_.push_back(ActorType::ANT1);
  enemy_insects_.push_back(ActorType::ANT2);
  enemy_insects_.push_back(ActorType::ANT3);
  enemy_insects_.push_back(ActorType::GRASSHOPPER);
}

void AdultGrasshopper::doSomething() {
  changePoints(-1);

  if (getPoints() <= 0) {
    die();
    return;
  }

  if (sleep()) return;

  std::list<Actor *> actors_at_point =
      getStudentWorld().actorsOfTypesAt(enemy_insects_, getCoord());

  // 1/3rd chance that the adult grasshopper bites an enemy at the point, if
  // there is an enemy at the point.
  if (actors_at_point.size() > 1 && randInt(0, 2) == 0) {  // 1 in 3 chance.
    actors_at_point.remove(this);  // Make sure it does not bite itself.

    int rand_index = randInt(0, actors_at_point.size() - 1);
    std::list<Actor *>::const_iterator i = actors_at_point.begin();

    // Iterate until we have a pointer to the randomly selected enemy actor to
    // be bitten at the current coordinate.
    for (int j = 0; j < rand_index; j++) i++;

    (*i)->bite(this, kBiteStrength);
  } else if (randInt(0, 9) == 0) {  // 1/10th chance it jumps somwhere else
    Coordinate jump_to_coord(0, 0);
    int give_up = 0;
    bool gave_up = false;

    do {
      // Ensure that we don't get stuck in this loop if there are few to no
      // empty spots available to jump to.
      if (give_up++ > 200) {
        gave_up = true;
        break;
      }

      const int really_big_number = 10e6;
      int radius = randInt(0, kJumpRadius);
      double theta = M_PI * randInt(0, really_big_number) / really_big_number;

      jump_to_coord.setX(getX() + radius * cos(theta));
      jump_to_coord.setY(getY() + radius * sin(theta));
    } while (jump_to_coord.getX() < 0 || jump_to_coord.getX() >= VIEW_WIDTH ||
             jump_to_coord.getY() < 0 || jump_to_coord.getY() >= VIEW_HEIGHT ||
             getStudentWorld()
                     .actorsOfTypeAt(ActorType::PEBBLE, jump_to_coord)
                     .size() > 0);  // Keep looking until we have a good coord.

    // Jump to the coordinate only if a valid one is found.
    if (!gave_up) moveTo(jump_to_coord);
  } else {
    // Attempt to eat. (and have a 50% chance of sleeping if we did sleep)
    // Otherwise, move somewhere.
    if (!(eatFood(kMaxGrasshopperFoodPerEating) && randInt(0, 1) == 0))
      randomMovement();
  }

  addSleep(2);
}

void AdultGrasshopper::stun() {
  // Adult grasshopper do nothing when stunned.
}
void AdultGrasshopper::poison() {
  // Adult grasshoppers do nothing when poisoned.
}
void AdultGrasshopper::bite(Actor *bit_by, int damage) {
  Insect::bite(bit_by, damage);

  // 50% chance of retaliating.
  if (randInt(0, 1) == 0) {
    bit_by->bite(this, kBiteStrength);
  }
}
