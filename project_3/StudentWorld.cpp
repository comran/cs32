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

StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir), ticks_(0) {}

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

      if (item == Field::FieldItem::rock) {
        addActor(new Pebble(*this, x, y));
      } else if (item == Field::FieldItem::grasshopper) {
        addActor(new BabyGrasshopper(*this, x, y));
      } else if (item == Field::FieldItem::poison) {
        addActor(new Poison(*this, x, y));
      } else if (item == Field::FieldItem::water) {
        addActor(new WaterPool(*this, x, y));
      } else if (item == Field::FieldItem::food) {
        addActor(new Food(*this, x, y, 6000));
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

        addActor(new AntHill(*this, colony, x, y, compiler[colony]));
      }
    }
  }

  return GWSTATUS_NO_WINNER;
}

void StudentWorld::cleanUp() {
  ticks_ = 0;

  for (int i = 0; i < actors_.size(); i++) {
    delete actors_[i];
  }
  actors_.clear();

  for (int x = 0; x < VIEW_WIDTH; x++) {
    for (int y = 0; y < VIEW_HEIGHT; y++) {
      actors_map_[x][y].clear();
    }
  }
}

int StudentWorld::move() {
  updateGameStatText();

  for (int i = 0; i < actors_.size(); i++) {
    if (!actors_[i]->dead()) {
      actors_[i]->doSomething();
    } else {
      int x = actors_[i]->getX(), y = actors_[i]->getY();
      actors_map_[x][y].remove(actors_[i]);
      delete actors_[i];
      actors_.erase(actors_.begin() + i);
    }
  }

  if (++ticks_ > 2000) {
    std::list<int> leaders = getLeaders();
    if(leaders.size() > 1) return GWSTATUS_NO_WINNER;
    setWinner(scoreboard_names_[*leaders.begin()]);

    return GWSTATUS_PLAYER_WON;
  }
  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addFood(int x, int y, int food_points) {
  Actor *food_actor = nullptr;
  std::list<Actor *> food_at_point = actorsOfTypeAt(ActorType::FOOD, x, y);

  if (food_at_point.size() > 0) food_actor = *food_at_point.begin();

  if (food_actor == nullptr) {
    Actor *new_actor = new Food(*this, x, y, food_points);
    actors_.push_back(new_actor);
    actors_map_[x][y].push_back(new_actor);
    return;
  }

  food_actor->changePoints(food_points);
}

void StudentWorld::addPheromone(int x, int y, int pheromone_points,
                                ActorType actor_type) {
  Actor *pheromone_actor = nullptr;
  std::list<Actor *> pheromone_at_point = actorsOfTypeAt(actor_type, x, y);

  if (pheromone_at_point.size() > 0)
    pheromone_actor = *pheromone_at_point.begin();

  if (pheromone_actor == nullptr) {
    Actor *new_actor = new Pheromone(*this, x, y, pheromone_points);
    actors_.push_back(new_actor);
    actors_map_[x][y].push_back(new_actor);
    return;
  }

  int extra = std::min(0, 768 - pheromone_points);
  pheromone_actor->changePoints(pheromone_points - extra);
}

void StudentWorld::updateGameStatText() {
  std::list<int> leaders = getLeaders();

  stringstream ticker_stream;
  ticker_stream << "Ticks: " << std::right << std::setw(5) << ticks_ << " -";
  for (int i = 0; i < 4; i++) {
    ticker_stream << "  " << scoreboard_names_[i];

    for (std::list<int>::const_iterator j = leaders.begin();
         j != leaders.end(); j++) {
      if(i == *j) {
        ticker_stream << "*";
        break;
      }
    }

    ticker_stream << ": " << std::setw(2) << std::setfill('0') << scoreboard_[i];
  }
  setGameStatText(ticker_stream.str());
}

void StudentWorld::updatePositionInGrid(Actor *actor, int to_x, int to_y) {
  int from_x = actor->getX(), from_y = actor->getY();

  actors_map_[from_x][from_y].remove(actor);
  actors_map_[to_x][to_y].push_back(actor);

  actor->GraphObject::moveTo(to_x, to_y);
}

std::list<Actor *> StudentWorld::actorsOfTypeAt(ActorType actor_type, int x,
                                                int y) {
  std::list<Actor *> actors_of_type;

  std::list<Actor *> actors_at_point = actors_map_[x][y];
  for (std::list<Actor *>::const_iterator i = actors_at_point.begin();
       i != actors_at_point.end(); i++) {
    if ((*i)->checkForObjectMatch(actor_type)) actors_of_type.push_back(*i);
  }

  return actors_of_type;
}

std::list<Actor *> StudentWorld::actorsOfTypesAt(
    std::vector<ActorType> actor_types, int x, int y) {
  std::list<Actor *> actors_of_types;

  for (int i = 0; i < actor_types.size(); i++) {
    std::list<Actor *> actors_of_type = actorsOfTypeAt(actor_types[i], x, y);
    actors_of_types.insert(actors_of_types.end(), actors_of_type.begin(),
                           actors_of_type.end());
  }

  return actors_of_types;
}

void StudentWorld::addActor(Actor *actor) {
  actors_.push_back(actor);
  actors_map_[actor->getX()][actor->getY()].push_back(actor);
}

void StudentWorld::updateScoreboard(int colony) {
  if (colony < 0 || colony > 3) {
    std::cerr << "INVALID COLONY: " << colony << std::endl;
    colony = 0;
  }

  scoreboard_[colony]++;
}

std::list<int> StudentWorld::getLeaders() {
  std::list<int> leaders;
  leaders.push_back(0);
  for(int i = 1;i < 4;i++) {
    if(scoreboard_[i] > scoreboard_[*leaders.begin()]) {
      leaders.clear();
      leaders.push_back(i);
    } else if(scoreboard_[i] == scoreboard_[*leaders.begin()]) {
      leaders.push_back(i);
    }
  }

  return leaders;
}
