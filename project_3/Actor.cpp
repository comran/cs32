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
ActorType Actor::getActorType() const { return actor_type_; }
int Actor::getPoints() const { return points_; };
StudentWorld &Actor::getStudentWorld() const { return student_world_; }
Coordinate Actor::getCoord() const { return Coordinate(getX(), getY()); }
bool Actor::checkForObjectMatch(ActorType type) const {
  return actor_type_ == type;
}
Actor::Direction Actor::randomDirection() const {
  return static_cast<Actor::Direction>(randInt(1, 4));
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


void Actor::poison() {}
void Actor::stun() {}
bool Actor::dead() { return dead_; }
void Actor::die() { dead_ = true; }
void Actor::feed(int &available_food) {}
void Actor::bite(Actor *bit_by, int damage) {}

// //////////////////////// OBJECT CLASSES /////////////////////////////////////
Food::Food(StudentWorld &student_world, Coordinate coord, int food_points)
    : Actor(student_world, ActorType::FOOD, IID_FOOD, coord, right, 2,
            food_points) {}

void Food::changePoints(int delta) {
  Actor::changePoints(delta);

  if (getPoints() <= 0) {
    die();
    setVisible(false);
  }
}

void Food::doSomething() {}

Pebble::Pebble(StudentWorld &student_world, Coordinate coord)
    : Actor(student_world, ActorType::PEBBLE, IID_ROCK, coord, right, 1, 1) {}
void Pebble::doSomething() {}

Poison::Poison(StudentWorld &student_world, Coordinate coord)
    : Actor(student_world, ActorType::POISON, IID_POISON, coord, right, 2, 1) {}

void Poison::doSomething() {
  std::vector<ActorType> insects;
  insects.push_back(ActorType::GRASSHOPPER);
  for (int i = 0; i < 4; i++) {
    insects.push_back(Ant::getActorTypeFromColony(i));
  }

  std::list<Actor *> insects_at_point =
      getStudentWorld().actorsOfTypesAt(insects, getCoord());

  for (std::list<Actor *>::const_iterator i = insects_at_point.begin();
       i != insects_at_point.end(); i++) {
    if ((*i)->dead()) continue;
    (*i)->poison();
  }
}

WaterPool::WaterPool(StudentWorld &student_world, Coordinate coord)
    : Actor(student_world, ActorType::WATER_POOL, IID_WATER_POOL, coord, right,
            2, 1) {}

void WaterPool::doSomething() {
  std::vector<ActorType> insects;
  for (int i = 0; i < 4; i++) {
    insects.push_back(Ant::getActorTypeFromColony(i));
  }

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
            coord, right, 2, 256) {}

void Pheromone::doSomething() {
  changePoints(-1);
  if (getPoints() <= 0) die();
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
            right, 2, 8999),
      compiler_(compiler),
      colony_(colony) {}

void AntHill::doSomething() {
  changePoints(-1);
  if (getPoints() <= 0) {
    die();
    return;
  }

  // Eat food at this square, up to 10000.
  std::list<Actor *> food_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getCoord());
  for (std::list<Actor *>::const_iterator i = food_at_point.begin();
       i != food_at_point.end(); i++) {
    int food_available = std::min((*i)->getPoints(), 10000);
    changePoints(food_available);
    (*i)->changePoints(-1 * food_available);

    break;
  }

  if (getPoints() >= 2000) {
    giveBirth();
    changePoints(-1500);
  }
}

void AntHill::giveBirth() {
  getStudentWorld().addActor(
      new Ant(getStudentWorld(), colony_, getCoord(), compiler_, *this));
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
  getStudentWorld().addFood(getCoord(), 100);
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

void Insect::bite(Actor *actor, int damage) { changePoints(-1 * damage); }
void Insect::poison() { changePoints(-150); }
bool Insect::eatFood(int max_food) {
  std::list<Actor *> food_at_point =
      getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getCoord());

  for (std::list<Actor *>::const_iterator i = food_at_point.begin();
       i != food_at_point.end(); i++) {
    if ((*i)->getPoints() < 1) continue;

    int food_available = (*i)->getPoints();
    food_available = std::min(food_available, max_food);
    (*i)->changePoints(-1 * food_available);
    feed(food_available);
    if (food_available > 0) return true;
  }

  return false;
}

Ant::Ant(StudentWorld &student_world, int colony, Coordinate coord,
         Compiler *compiler, AntHill &my_ant_hill)
    : Insect(student_world, getImageForColony(colony), coord,
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
      instruction_counter_ = stoi(c.operand1);
      return true;
    }
    case Compiler::if_command: {
      switch (stoi(c.operand1)) {
        case Compiler::i_smell_danger_in_front_of_me: {
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
          Coordinate coord_in_front =
              getCoord().coordInDirection(getDirection());
          std::list<Actor *> pheromone = getStudentWorld().actorsOfTypeAt(
              Pheromone::getActorType(getColonyFromActorType(getActorType())),
              coord_in_front);

          if (pheromone.size() > 0) {
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
          if (getPoints() <= 25) instruction_counter_ = stoi(c.operand2);
          break;
        }
        case Compiler::i_am_standing_on_my_anthill: {
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
          std::list<Actor *> food =
              getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getCoord());
          if (food.size() > 0) {
            if ((*food.begin())->getPoints() < 1) break;
            instruction_counter_ = stoi(c.operand2);
          }
          break;
        }
        case Compiler::i_am_standing_with_an_enemy: {
          if (getStudentWorld()
                  .actorsOfTypesAt(other_insects_, getCoord())
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
          getCoord(), 256, getColonyFromActorType(Actor::getActorType()));
      return false;
    }
    case Compiler::dropFood: {
      getStudentWorld().addFood(getCoord(), food_carried_);
      food_carried_ = 0;
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
          getStudentWorld().actorsOfTypeAt(ActorType::PEBBLE, Coordinate(x, y));
      if (pebbles_in_direction.size() > 0) {
        blocked_by_pebble_ = true;
        return false;
      }

      moveTo(Coordinate(x, y));
      resetStunned();
      was_bit_ = false;
      return false;
    }
    case Compiler::label:
      std::cout << "LABEL: " << c.operand1 << " - " << c.operand2 << std::endl;
      // TODO(comran): Implement.
      return false;
    case Compiler::bite: {
      std::list<Actor *> other_insects_at_point =
          getStudentWorld().actorsOfTypesAt(other_insects_, getCoord());

      for (std::list<Actor *>::const_iterator i =
               other_insects_at_point.begin();
           i != other_insects_at_point.end(); i++) {
        (*i)->bite(this, 15);
      }

      return false;
    }
    case Compiler::pickupFood: {
      std::list<Actor *> food_at_point =
          getStudentWorld().actorsOfTypeAt(ActorType::FOOD, getCoord());
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

int Ant::getImageForColony(int colony) {
  int ant_iids[] = {IID_ANT_TYPE0, IID_ANT_TYPE1, IID_ANT_TYPE2, IID_ANT_TYPE3};
  return ant_iids[colony];
}

void Ant::bite(Actor *bit_by, int damage) {
  Insect::bite(bit_by, damage);
  was_bit_ = true;
}

Grasshopper::Grasshopper(StudentWorld &student_world, int iid, Coordinate coord,
                         int points)
    : Insect(student_world, iid, coord, ActorType::GRASSHOPPER,
             randomDirection(), 1, points),
      distance_(randInt(2, 10)) {}

void Grasshopper::randomMovement() {
  if (distance_ <= 0) {
    setDirection(randomDirection());
    distance_ = randInt(2, 10);
  }

  Coordinate coord_in_direction = getCoord().coordInDirection(getDirection());

  std::list<Actor *> pebbles_in_direction =
      getStudentWorld().actorsOfTypeAt(ActorType::PEBBLE, coord_in_direction);
  if (pebbles_in_direction.size() > 0) distance_ = 0;

  if (distance_-- > 0) {
    moveTo(coord_in_direction);
    resetStunned();
  }
}

void Grasshopper::feed(int &available_food) {
  int food_eaten = std::min(available_food, 200);

  changePoints(food_eaten);
  available_food -= food_eaten;
}

BabyGrasshopper::BabyGrasshopper(StudentWorld &student_world, Coordinate coord)
    : Grasshopper(student_world, IID_BABY_GRASSHOPPER, coord, 500) {}

void BabyGrasshopper::doSomething() {
  changePoints(-1);

  if (getPoints() <= 0) {
    die();
    return;
  } else if (getPoints() >= 1600) {
    getStudentWorld().addActor(
        new AdultGrasshopper(getStudentWorld(), getCoord()));
    die();
  }

  if (sleep()) return;
  if (!(eatFood(200) && randInt(0, 1) == 0)) randomMovement();

  addSleep(2);
}

AdultGrasshopper::AdultGrasshopper(StudentWorld &student_world,
                                   Coordinate coord)
    : Grasshopper(student_world, IID_ADULT_GRASSHOPPER, coord, 1600) {
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

  if (actors_at_point.size() > 1 && randInt(0, 2) == 0) {  // 1 in 3 chance.
    int rand_index = randInt(0, actors_at_point.size() - 1);
    std::list<Actor *>::const_iterator i = actors_at_point.begin();
    if(*i != this) {
      for (int j = 0; j < rand_index; j++) i++;
      (*i)->bite(this, 50);
    }
  } else if (randInt(0, 9) == 0) {
    Coordinate jump_to_coord(0, 0);
    int give_up = 0;
    bool gave_up = false;

    do {
      if (give_up++ > 100) {
        gave_up = true;
        break;  // So that we don't get stuck in a loop.
      }

      const int really_big_number = 10e6;
      int radius = randInt(0, 10);
      double theta =
          3.14159 * randInt(0, really_big_number) / really_big_number;

      jump_to_coord.setX(getX() + radius * cos(theta));
      jump_to_coord.setY(getY() + radius * sin(theta));
    } while (jump_to_coord.getX() < 0 || jump_to_coord.getX() >= VIEW_WIDTH ||
             jump_to_coord.getY() < 0 || jump_to_coord.getY() >= VIEW_HEIGHT ||
             getStudentWorld()
                     .actorsOfTypeAt(ActorType::PEBBLE, jump_to_coord)
                     .size() > 0);
    if (!gave_up) moveTo(jump_to_coord);
  } else {
    if (!(eatFood(200) && randInt(0, 1) == 0)) randomMovement();
  }
  addSleep(2);
}

void AdultGrasshopper::stun() {}
void AdultGrasshopper::poison() {}

void AdultGrasshopper::bite(Actor *bit_by, int damage) {
  Insect::bite(bit_by, damage);

  if (randInt(0, 1) == 0) {
    bit_by->bite(this, 50);
  }
}
