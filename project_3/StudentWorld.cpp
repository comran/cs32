#include <string>
#include <sstream>
#include <iomanip>

#include "Actor.h"
#include "StudentWorld.h"
#include "Field.h"
using namespace std;

GameWorld *createStudentWorld(string assetDir) {
  return new StudentWorld(assetDir);
}

Coordinate::Coordinate(int x, int y) : x_(x), y_(y) {}
bool Coordinate::operator<(const Coordinate &l) const {
  return getX() + getY() * VIEW_WIDTH < l.getX() + l.getY() * VIEW_WIDTH;
}
int Coordinate::getX() const { return x_; }
int Coordinate::getY() const { return y_; }
void Coordinate::setX(int x) { x_ = x; }
void Coordinate::setY(int y) { y_ = y; }
Coordinate Coordinate::coordInDirection(GraphObject::Direction direction) {
  int x = x_, y = y_;

  switch (direction) {
    case GraphObject::right:
      x++;
      break;
    case GraphObject::left:
      x--;
      break;
    case GraphObject::up:
      y++;
      break;
    case GraphObject::down:
      y--;
      break;
    case GraphObject::none:
      break;
  }

  return Coordinate(x, y);
}

StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir), ticks_(0), leader_(-1) {}

StudentWorld::~StudentWorld() { cleanUp(); }

int StudentWorld::init() {
  cleanUp();

  Field f;

  string fieldFile = getFieldFilename();
  std::vector<std::string> file_names = getFilenamesOfAntPrograms();

  string error;
  if (f.loadField(fieldFile, error) != Field::LoadResult::load_success) {
    setError(fieldFile + " " + error);
    return GWSTATUS_LEVEL_ERROR;
  }

  Compiler *compiler[4];
  for (int i = 0; i < 4; i++) {
    compiler[i] = new Compiler();
    if (!compiler[i]->compile(file_names[i], error)) {
      setError(file_names[i] + " " + error);
      return GWSTATUS_LEVEL_ERROR;
    }
    scoreboard_names_[i] = compiler[i]->getColonyName();
  }

  for (int x = 0; x < VIEW_WIDTH; x++) {
    for (int y = 0; y < VIEW_HEIGHT; y++) {
      Field::FieldItem item = f.getContentsOf(x, y);
      Coordinate coord(x, y);

      if (item == Field::FieldItem::rock) {
        addActor(new Pebble(*this, coord));
      } else if (item == Field::FieldItem::grasshopper) {
        addActor(new BabyGrasshopper(*this, coord));
      } else if (item == Field::FieldItem::poison) {
        addActor(new Poison(*this, coord));
      } else if (item == Field::FieldItem::water) {
        addActor(new WaterPool(*this, coord));
      } else if (item == Field::FieldItem::food) {
        addActor(new Food(*this, coord, 6000));
      } else if (item == Field::FieldItem::anthill0 ||
                 item == Field::FieldItem::anthill1 ||
                 item == Field::FieldItem::anthill2 ||
                 item == Field::FieldItem::anthill3) {
        int colony = 0;
        switch (item) {
          case Field::FieldItem::anthill0:
            break;
          case Field::FieldItem::anthill1:
            colony = 1;
            break;
          case Field::FieldItem::anthill2:
            colony = 2;
            break;
          case Field::FieldItem::anthill3:
            colony = 3;
            break;
          default:
            std::cerr << "UNKNOWN COLONY #: " << colony << std::endl;
        }

        addActor(new AntHill(*this, colony, coord, compiler[colony]));
      }
    }
  }

  return GWSTATUS_NO_WINNER;
}

void StudentWorld::cleanUp() {
  ticks_ = 0;

  for (std::map<Coordinate, std::list<Actor *>>::iterator i = actors_.begin();
       i != actors_.end(); i++) {
    for (std::list<Actor *>::iterator j = i->second.begin();
         j != i->second.end(); j++) {
      delete *j;
    }
  }
}

int StudentWorld::move() {
  updateGameStatText();

  for (std::map<Coordinate, std::list<Actor *>>::iterator i = actors_.begin();
       i != actors_.end(); i++) {
    i->second.remove(nullptr);

    int original_size = i->second.size(), k = 0;
    for (std::list<Actor *>::iterator j = i->second.begin(); k < original_size;
         j++, k++) {
      if (*j == nullptr) continue;
      if (!(*j)->dead()) {
        (*j)->doSomething();
      } else {
        delete *j;
        *j = nullptr;
      }
    }

    if (i->second.size() < 1) actors_.erase(i);
  }

  if (++ticks_ > 2000) {
    if (leader_ < 0) return GWSTATUS_NO_WINNER;

    setWinner(scoreboard_names_[leader_]);
    return GWSTATUS_PLAYER_WON;
  }

  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addFood(Coordinate coord, int food_points) {
  Actor *food_actor = nullptr;
  std::list<Actor *> food_at_point = actorsOfTypeAt(ActorType::FOOD, coord);

  if (food_at_point.size() > 0) food_actor = *food_at_point.begin();

  if (food_actor == nullptr) {
    Actor *new_actor = new Food(*this, coord, food_points);
    addActor(new_actor);
    return;
  }

  food_actor->changePoints(food_points);
}

void StudentWorld::addPheromone(Coordinate coord, int pheromone_points,
                                int colony) {
  Actor *pheromone_actor = nullptr;
  std::list<Actor *> pheromone_at_point =
      actorsOfTypeAt(Pheromone::getActorType(colony), coord);

  if (pheromone_at_point.size() > 0)
    pheromone_actor = *pheromone_at_point.begin();

  if (pheromone_actor == nullptr) {
    Actor *new_actor = new Pheromone(*this, coord, colony);
    addActor(new_actor);
    return;
  }

  pheromone_points =
      std::min(pheromone_points, 768 - pheromone_actor->getPoints());
  pheromone_points = std::max(0, pheromone_points);
  pheromone_actor->changePoints(pheromone_points);
}

void StudentWorld::updateGameStatText() {
  stringstream ticker_stream;
  ticker_stream << "Ticks: " << std::right << std::setw(5) << ticks_ << " - ";
  for (int i = 0; i < 4; i++) {
    if (i > 0) ticker_stream << "  ";
    ticker_stream << scoreboard_names_[i];

    if (leader_ == i) {
      ticker_stream << "*";
    }

    ticker_stream << ": " << std::setw(2) << std::setfill('0')
                  << scoreboard_[i];
  }
  setGameStatText(ticker_stream.str());
}

std::list<Actor *> StudentWorld::actorsOfTypeAt(ActorType actor_type,
                                                Coordinate coord) {
  std::list<Actor *> actors_of_type;

  std::list<Actor *> actors_at_point = actors_[coord];
  for (std::list<Actor *>::const_iterator i = actors_at_point.begin();
       i != actors_at_point.end(); i++) {
    if (*i == nullptr) continue;
    if ((*i)->checkForObjectMatch(actor_type)) actors_of_type.push_back(*i);
  }

  return actors_of_type;
}

std::list<Actor *> StudentWorld::actorsOfTypesAt(
    std::vector<ActorType> actor_types, Coordinate coord) {
  std::list<Actor *> actors_of_types;

  for (int i = 0; i < actor_types.size(); i++) {
    std::list<Actor *> actors_of_type = actorsOfTypeAt(actor_types[i], coord);
    actors_of_types.insert(actors_of_types.end(), actors_of_type.begin(),
                           actors_of_type.end());
  }

  return actors_of_types;
}

void StudentWorld::updatePositionInGrid(Actor *actor, Coordinate to_coord) {
  Coordinate from_coord(actor->getX(), actor->getY());
  std::list<Actor *>::iterator i =
      std::find(actors_[from_coord].begin(), actors_[from_coord].end(), actor);
  (*i) = nullptr;

  actor->GraphObject::moveTo(to_coord.getX(), to_coord.getY());
  addActor(actor);
}

void StudentWorld::addActor(Actor *actor) {
  std::map<Coordinate, std::list<Actor *>>::iterator i;
  i = actors_.find(actor->getCoord());

  if (i != actors_.end()) {
    i->second.push_back(actor);
  } else {
    std::list<Actor *> actors_at_point;
    actors_at_point.push_back(actor);
    actors_[actor->getCoord()] = actors_at_point;
  }
}

void StudentWorld::updateScoreboard(int colony) {
  if (colony < 0 || colony > 3) {
    std::cerr << "INVALID COLONY: " << colony << std::endl;
    colony = 0;
  }

  scoreboard_[colony]++;
  if (leader_ < 0 && scoreboard_[colony] > 5) {
    leader_ = colony;
  }

  if (scoreboard_[colony] > scoreboard_[leader_] && scoreboard_[colony] > 5)
    leader_ = colony;
}
