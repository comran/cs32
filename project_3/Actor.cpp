#include <algorithm>

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// TODO(comran): Finish adult grasshopper.
// TODO(comran): Finish ant class.

// ////////////////////////// BASE CLASS ///////////////////////////////////////
Actor::Actor(StudentWorld &student_world, ActorType actor_type, int iid, int x,
             int y, Actor::Direction dir, int depth, int initial_points)
    : GraphObject(iid, x, y, dir, depth),
      actor_type_(actor_type),
      student_world_(student_world),
      dead_(false),
      points_(initial_points) {}
void Actor::poison() {}
void Actor::stun() {}
bool Actor::dead() { return dead_; }
void Actor::die() { dead_ = true; }
void Actor::feed(int &available_food) {}
void Actor::bite() {}
int Actor::getPoints() { return points_; };
void Actor::changePoints(int delta) { points_ += delta; }
bool Actor::checkForObjectMatch(ActorType type) { return actor_type_ == type; }
ActorType Actor::getActorType() { return actor_type_; }
StudentWorld &Actor::getStudentWorld() { return student_world_; }
Actor::Direction Actor::randomDirection() {
  return static_cast<Actor::Direction>(randInt(1, 4));
}

void Actor::moveTo(int x, int y) {
  student_world_.updatePositionInGrid(this, x, y);
}

// //////////////////////// OBJECT CLASSES /////////////////////////////////////
Food::Food(StudentWorld &student_world, int x, int y, int food_points)
    : Actor(student_world, ActorType::FOOD, IID_FOOD, x, y, right, 2,
            food_points) {}
void Food::increaseFood(int food_points) { changePoints(food_points); }
void Food::doSomething() {
  if (getPoints() <= 0) {
    setVisible(false);
    return;
  }
  setVisible(true);

  std::list<Actor *> insects_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::GRASSHOPPER, getX(), getY());
  for (std::list<Actor *>::const_iterator i = insects_at_point.begin();
       i != insects_at_point.end(); i++) {
    if ((*i)->dead()) continue;

    int food_available = getPoints();
    (*i)->feed(food_available);
    changePoints(food_available - getPoints());
  }
}

Pebble::Pebble(StudentWorld &student_world, int x, int y)
    : Actor(student_world, ActorType::PEBBLE, IID_ROCK, x, y, right, 1, 0) {}
void Pebble::doSomething() {}

Poison::Poison(StudentWorld &student_world, int x, int y)
    : Actor(student_world, ActorType::POISON, IID_POISON, x, y, right, 2, 0) {}

void Poison::doSomething() {
  std::list<Actor *> insects_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::GRASSHOPPER, getX(), getY());
  for (std::list<Actor *>::const_iterator i = insects_at_point.begin();
       i != insects_at_point.end(); i++) {
    if ((*i)->dead()) continue;
    (*i)->poison();
  }
}

WaterPool::WaterPool(StudentWorld &student_world, int x, int y)
    : Actor(student_world, ActorType::WATER_POOL, IID_WATER_POOL, x, y, right,
            2, 0) {}

void WaterPool::doSomething() {
  std::list<Actor *> insects_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::GRASSHOPPER, getX(), getY());
  for (std::list<Actor *>::const_iterator i = insects_at_point.begin();
       i != insects_at_point.end(); i++) {
    if ((*i)->dead()) continue;
    (*i)->stun();
  }
}

Pheromone::Pheromone(StudentWorld &student_world, int colony, int x, int y)
    : Actor(student_world, getActorType(colony), getImageForColony(colony), x,
            y, right, 2, 256) {}

void Pheromone::doSomething() {
  changePoints(-1);
  if (getPoints() <= 0) die();
}

ActorType Pheromone::getActorType(int colony) {
  switch (colony) {
    case 0:
      break;
    case 1:
      return ActorType::PHEROMONE1;
    case 2:
      return ActorType::PHEROMONE2;
    case 3:
      return ActorType::PHEROMONE3;
    default:
      std::cerr << "UNKNOWN COLONY #: " << colony << std::endl;
  }

  return ActorType::PHEROMONE0;
}

int Pheromone::getImageForColony(int colony) {
  switch (colony) {
    case 0:
      break;
    case 1:
      return IID_PHEROMONE_TYPE1;
    case 2:
      return IID_PHEROMONE_TYPE2;
    case 3:
      return IID_PHEROMONE_TYPE3;
    default:
      std::cerr << "UNKNOWN COLONY #: " << colony << std::endl;
  }

  return IID_PHEROMONE_TYPE0;
}

AntHill::AntHill(StudentWorld &student_world, int colony, int x, int y,
                 Compiler *compiler)
    : Actor(student_world, ActorType::ANT_HILL, IID_ANT_HILL, x, y, right, 2,
            8999),
      compiler_(compiler),
      colony_(colony),
      food_eaten_(0) {}

void AntHill::doSomething() {
  changePoints(-1);
  if (getPoints() <= 0) {
    die();
    return;
  }

  // Eat food at this square, up to 10000.
  std::list<Actor *> food_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getX(), getY());
  for (std::list<Actor *>::const_iterator i = food_at_point.begin();
       i != food_at_point.end(); i++) {
    int food_available = (*i)->getPoints();
    food_available = std::min(food_available, 10000 - food_eaten_);
    changePoints(food_available);
    food_eaten_ += food_available;
    (*i)->changePoints(-1 * food_available);
    // TODO(comran): Fix all this food count stuff.

    return;
  }

  if (getPoints() >= 2000) {
    giveBirth();
    changePoints(-1500);
    // TODO(comran): Tell student world to increase # of ants for this colony.
  }
}

void AntHill::giveBirth() {
  getStudentWorld().addActor(
      new Ant(getStudentWorld(), colony_, getX(), getY(), compiler_, *this));
  getStudentWorld().updateScoreboard(colony_);
}

// //////////////////////// INSECT CLASSES /////////////////////////////////////
Insect::Insect(StudentWorld &student_world, int iid, int x, int y,
               ActorType actor_type, Actor::Direction direction, int depth,
               int points)
    : Actor(student_world, actor_type, iid, x, y, direction, depth, points),
      sleep_ticks_(0),
      moved_from_stunned_point_(true) {}

void Insect::die() {
  getStudentWorld().addFood(getX(), getY(), 100);
  Actor::die();
  setVisible(false);
}

void Insect::stun() {
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

Ant::Ant(StudentWorld &student_world, int colony, int x, int y,
         Compiler *compiler, AntHill &my_ant_hill)
    : Insect(student_world, getImageForColony(colony), x, y,
             getActorTypeFromColony(colony), randomDirection(), 1, 1500),
      compiler_(compiler),
      my_ant_hill_(my_ant_hill),
      blocked_by_pebble_(false),
      was_bit_(false),
      last_random_number_(0),
      food_carried_(0),
      instruction_counter_(0) {
  other_insects_.push_back(ActorType::ANT0);
  other_insects_.push_back(ActorType::ANT1);
  other_insects_.push_back(ActorType::ANT2);
  other_insects_.push_back(ActorType::ANT3);
  other_insects_.push_back(ActorType::GRASSHOPPER);

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

  Compiler::Command command;
  do {
    if (!compiler_->getCommand(instruction_counter_++, command)) {
      die();
      return;
    }

  } while (runCommand(command));
}

ActorType Ant::getActorTypeFromColony(int colony) {
  switch (colony) {
    case 0:
      break;
    case 1:
      return ActorType::ANT1;
    case 2:
      return ActorType::ANT2;
    case 3:
      return ActorType::ANT3;
    default:
      std::cerr << "UNKNOWN COLONY #: " << colony << std::endl;
  }

  return ActorType::ANT0;
}

int Ant::getColonyFromActorType(ActorType actor_type) {
  switch (actor_type) {
    case ActorType::ANT0:
      break;
    case ActorType::ANT1:
      return 1;
    case ActorType::ANT2:
      return 2;
    case ActorType::ANT3:
      return 3;
    default:
      std::cerr << "UNKNOWN COLONY ACTOR_TYPE" << std::endl;
  }

  return 0;
}

bool Ant::runCommand(const Compiler::Command &c) {
  switch (c.opcode) {
    case Compiler::goto_command: {
      instruction_counter_ = stoi(c.operand1);
      return true;
    }
    case Compiler::if_command: {
      switch (stoi(c.operand1)) {
        case Compiler::i_smell_danger_in_front_of_me: {
          // TODO(comran): Implement the following.
          // instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::i_smell_pheromone_in_front_of_me: {
          // TODO(comran): Implement the following.
          // instruction_counter_ = stoi(c.operand2);
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
          if (getPoints() <= 25) instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::i_am_standing_on_my_anthill: {
          if (my_ant_hill_.getX() == getX() && my_ant_hill_.getY() == getY()) {
            std::cout << "on my hill\n";
            instruction_counter_ = stoi(c.operand2);
          }
          break;
        }
        case Compiler::i_am_standing_on_food: {
          if (getStudentWorld()
                  .actorsOfTypeAt(ActorType::FOOD, getX(), getY())
                  .size() > 0) {
            instruction_counter_ = stoi(c.operand2);
          }
          break;
        }
        case Compiler::i_am_standing_with_an_enemy: {
          if (getStudentWorld()
                  .actorsOfTypesAt(other_insects_, getX(), getY())
                  .size() > 0) {
            instruction_counter_ = stoi(c.operand2);
          }
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
      int direction = getDirection();
      if (++direction == 5) direction = 1;
      setDirection(static_cast<Actor::Direction>(randInt(1, 4)));
      return false;
    }
    case Compiler::rotateCounterClockwise: {
      int direction_int = static_cast<int>(getDirection());
      if (--direction_int == -1) direction_int = 4;
      setDirection(static_cast<Actor::Direction>(direction_int));
      return false;
    }
    case Compiler::faceRandomDirection: {
      setDirection(randomDirection());
      return false;
    }
    case Compiler::emitPheromone: {
      getStudentWorld().addPheromone(
          getX(), getY(), 256, Pheromone::getActorType(getColonyFromActorType(
                                   Actor::getActorType())));
      return false;
    }
    case Compiler::dropFood: {
      std::cout << "dropping " << food_carried_ << " units of food.\n";
      getStudentWorld().addFood(getX(), getY(), food_carried_);
      food_carried_ = 0;
      if (my_ant_hill_.getX() == getX() && my_ant_hill_.getY() == getY())
        std::cout << "dropping food on my anthill.\n";
      return false;
    }
    case Compiler::moveForward: {
      blocked_by_pebble_ = false;
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

      std::list<Actor *> pebbles_in_direction =
          getStudentWorld().actorsOfTypeAt(ActorType::PEBBLE, x, y);
      if (pebbles_in_direction.size() > 0) {
        blocked_by_pebble_ = true;
        return false;
      }

      moveTo(x, y);
      resetStunned();
      was_bit_ = false;
      return false;
    }
    case Compiler::label:
      // TODO(comran): Implement.
      return false;
    case Compiler::bite: {
      std::list<Actor *> other_insects_at_point =
          getStudentWorld().actorsOfTypesAt(other_insects_, getX(), getY());
      for (std::list<Actor *>::const_iterator i =
               other_insects_at_point.begin();
           i != other_insects_at_point.end(); i++) {
        (*i)->bite();
      }

      return false;
    }
    case Compiler::pickupFood: {
      std::list<Actor *> food_at_point =
          getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getX(), getY());
      if (food_at_point.size() < 1) return false;  // No food at point.
      int held_food = (*food_at_point.begin())->getPoints();
      held_food = std::min(held_food, 400);  // Cap at 400 units per pickup.
      held_food = std::min(held_food, 1800 - food_carried_);  // Max cap @ 1800.
      food_carried_ += held_food;
      (*food_at_point.begin())->changePoints(-1 * held_food);
      return false;
    }
    case Compiler::eatFood: {
      int food_eaten = std::min(100, food_carried_);
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

void Ant::bite() {
  std::cout << "I WAS BIT!\n";
  was_bit_ = true;
  changePoints(-15);
}

int Ant::getImageForColony(int colony) {
  switch (colony) {
    case 1:
      return IID_ANT_TYPE1;
    case 2:
      return IID_ANT_TYPE2;
    case 3:
      return IID_ANT_TYPE3;
    case 0:
      break;
    default:
      std::cerr << "UNKNOWN COLONY #: " << colony << std::endl;
  }

  return IID_ANT_TYPE0;
}

Grasshopper::Grasshopper(StudentWorld &student_world, int iid, int x, int y,
                         int points)
    : Insect(student_world, iid, x, y, ActorType::GRASSHOPPER,
             randomDirection(), 1, points),
      distance_(randInt(2, 10)) {}

void Grasshopper::randomMovement() {
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
    resetStunned();
  }
}

bool Grasshopper::withinBounds(Direction dir, int distance) {
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

void Grasshopper::feed(int &available_food) {
  int food_eaten = std::min(available_food, 200);

  changePoints(food_eaten);
  available_food -= food_eaten;
}

BabyGrasshopper::BabyGrasshopper(StudentWorld &student_world, int x, int y)
    : Grasshopper(student_world, IID_BABY_GRASSHOPPER, x, y, 500) {}

void BabyGrasshopper::doSomething() {
  changePoints(-1);  // Subtract one hit point.
  if (getPoints() <= 0) {
    die();
    return;
  } else if (getPoints() >= 1600) {
    getStudentWorld().addActor(
        new AdultGrasshopper(getStudentWorld(), getX(), getY()));
    die();
  }

  if (sleep()) return;

  randomMovement();
  addSleep(2);
}

void BabyGrasshopper::poison() { changePoints(-150); }

AdultGrasshopper::AdultGrasshopper(StudentWorld &student_world, int x, int y)
    : Grasshopper(student_world, IID_ADULT_GRASSHOPPER, x, y, 1600) {}

void AdultGrasshopper::doSomething() {
  // TODO(comran): Make sure they can die too.
  if (sleep()) return;

  randomMovement();
  addSleep(2);
}

void AdultGrasshopper::stun() {}
