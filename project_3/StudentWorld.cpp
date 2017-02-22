#include <string>
#include <sstream>

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
  //ticks_ = 0;

  Field f;

  string fieldFile = getFieldFilename();

  string error;
  if (f.loadField(fieldFile, error) != Field::LoadResult::load_success) {
    setError(fieldFile + " " + error);
    return GWSTATUS_LEVEL_ERROR;
  }

  for (int x = 0; x < VIEW_WIDTH; x++) {
    for (int y = 0; y < VIEW_HEIGHT; y++) {
      Field::FieldItem item = f.getContentsOf(x, y);

      Actor *new_actor = nullptr;
      if (item == Field::FieldItem::rock) {
        new_actor = new Pebble(*this, x, y);
      } else if (item == Field::FieldItem::grasshopper) {
        new_actor = new BabyGrasshopper(*this, x, y);
      }

      if (new_actor != nullptr) {
        actors_.push_back(new_actor);
        actors_map_[x][y].push_back(new_actor);
      }
    }
  }

  return GWSTATUS_NO_WINNER;
}

void StudentWorld::cleanUp() {
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
    if (!actors_[i]->dead()) actors_[i]->doSomething();
  }

  if (++ticks_ >= 2000) return GWSTATUS_NO_WINNER;
  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addFood(int x, int y, int food_points) {
  Actor *new_actor = new Food(*this, x, y, food_points);
  actors_.push_back(new_actor);
  actors_map_[x][y].push_back(new_actor);
}

void StudentWorld::updateGameStatText() {
  stringstream ticker_stream;
  ticker_stream << "Ticks: " << ticks_;
  setGameStatText(ticker_stream.str());
}

void StudentWorld::updatePositionInGrid(Actor *actor, int to_x,
                              int to_y) {
  int from_x = actor->getX();
  int from_y = actor->getY();

  actors_map_[from_x][from_y].remove(actor);
  actors_map_[to_x][to_y].push_back(actor);
}
