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
  // Operator for the map to sort the coordinates by, where higher rows are
  // always greater than lower rows and higher columns in the same row are
  // greater than lower columns in that row.
  return getX() + getY() * VIEW_WIDTH < l.getX() + l.getY() * VIEW_WIDTH;
}
int Coordinate::getX() const { return x_; }
int Coordinate::getY() const { return y_; }
void Coordinate::setX(int x) { x_ = x; }
void Coordinate::setY(int y) { y_ = y; }
Coordinate Coordinate::coordInDirection(GraphObject::Direction direction) {
  int x = x_, y = y_;

  // Update x and y properly according to the direction parameter.
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
    : GameWorld(assetDir),
      ticks_(0),
      leader_(-1),
      scoreboard_{0, 0, 0, 0},
      ant_hills_on_field_(0) {}

StudentWorld::~StudentWorld() { cleanUp(); }

int StudentWorld::init() {
  cleanUp();

  // Load the field.
  Field f;
  string fieldFile = getFieldFilename(), error;
  if (f.loadField(fieldFile, error) != Field::LoadResult::load_success) {
    setError(fieldFile + " " + error);
    return GWSTATUS_LEVEL_ERROR;
  }

  // Load the bug files.
  std::vector<std::string> file_names = getFilenamesOfAntPrograms();
  ant_hills_on_field_ = file_names.size();
  // Cap maximum number of bugs allowed on the field to four.
  if (ant_hills_on_field_ > 4) return GWSTATUS_LEVEL_ERROR;

  // Create compilers for bugs.
  Compiler *compiler[4];
  for (int i = 0; i < file_names.size(); i++) {
    compiler[i] = new Compiler();

    // Check for success in interpreting the code, and die on any errors.
    if (!compiler[i]->compile(file_names[i], error)) {
      setError(file_names[i] + " " + error);
      return GWSTATUS_LEVEL_ERROR;
    }

    // Store the name to be displayed on the scoreboard.
    scoreboard_names_[i] = compiler[i]->getColonyName();
  }

  // Iterate through every single square on the grid of the field in the file
  // loaded and add in any actors that appear in the squares to the field
  // data structure in their appropriate forms.
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
        // Initial food on the field defaults to a total of 6000 units.
        addActor(new Food(*this, coord, 6000));
      } else if (item == Field::FieldItem::anthill0 ||
                 item == Field::FieldItem::anthill1 ||
                 item == Field::FieldItem::anthill2 ||
                 item == Field::FieldItem::anthill3) {
        // Map field items for anthill to their raw colony numbers, for use in
        // initialization of the anthills.
        int colony = 0;
        map<Field::FieldItem, int> field_item_anthill_to_colony;
        field_item_anthill_to_colony[Field::FieldItem::anthill0] = 0;
        field_item_anthill_to_colony[Field::FieldItem::anthill1] = 1;
        field_item_anthill_to_colony[Field::FieldItem::anthill2] = 2;
        field_item_anthill_to_colony[Field::FieldItem::anthill3] = 3;

        addActor(new AntHill(*this, field_item_anthill_to_colony[item], coord,
                             compiler[colony]));
      }
    }
  }

  return GWSTATUS_NO_WINNER;
}

void StudentWorld::cleanUp() {
  ticks_ = 0;

  // Go through every key in the map and delete all objects within their
  // respective
  // lists.
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

  // Iterate through every key in the map, where there are actors existing at
  // a coordinate.
  for (std::map<Coordinate, std::list<Actor *>>::iterator i = actors_.begin();
       i != actors_.end(); i++) {
    // Garbage collect all nullptrs remaining from deleted objects.
    i->second.remove(nullptr);

    // Iterate through all items in the list, using the original size to avoid
    // issues where objects are deleted while iterating through the list.
    int original_size = i->second.size(), k = 0;
    for (std::list<Actor *>::iterator j = i->second.begin(); k < original_size;
         j++, k++) {
      if (*j == nullptr) continue;  // Ensure that actor was not deleted.

      if (!(*j)->dead()) {
        (*j)->doSomething();
      } else {
        // Delete all dead actors.
        delete *j;
        *j = nullptr;
      }
    }

    // Remove keys where there are no actors at a given coordinate.
    if (i->second.size() < 1) actors_.erase(i);
  }

  // End game after 2000 ticks.
  if (++ticks_ > 2000) {
    // Leader_ was initialized as -1 so return no winner if there was never a
    // score update that established a clear winner among the ant colonies.
    if (leader_ < 0) return GWSTATUS_NO_WINNER;

    // Leader is equal to a colony number, so set that colony as the winner.
    setWinner(scoreboard_names_[leader_]);
    return GWSTATUS_PLAYER_WON;
  }

  // Continue game if there are still ticks to go through.
  return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addFood(Coordinate coord, int food_points) {
  // Attempt to find a food actor that already exists at the coordinate.
  Actor *food_actor = nullptr;
  std::list<Actor *> food_at_point = actorsOfTypeAt(ActorType::FOOD, coord);
  if (food_at_point.size() > 0) food_actor = *food_at_point.begin();

  if (food_actor == nullptr) {
    // Create a new food object if no food already exists at the coordinate.
    Actor *new_actor = new Food(*this, coord, food_points);
    addActor(new_actor);
    return;
  }

  food_actor->changePoints(food_points);
}

void StudentWorld::addPheromone(Coordinate coord, int pheromone_points,
                                int colony) {
  // Attempt to find a pheromone actor that already exists at the coordinate.
  Actor *pheromone_actor = nullptr;
  std::list<Actor *> pheromone_at_point =
      actorsOfTypeAt(Pheromone::getActorType(colony), coord);

  if (pheromone_at_point.size() > 0)
    pheromone_actor = *pheromone_at_point.begin();

  if (pheromone_actor == nullptr) {
    // Create a new pheromone object if no food already exists at the
    // coordinate.
    Actor *new_actor = new Pheromone(*this, coord, colony);
    addActor(new_actor);
    return;
  }

  // Cap pheromone points when updating a pheromone that already exists at the
  // coordinate to between 0 and 768 (inclusive).
  pheromone_points =
      std::min(pheromone_points, 768 - pheromone_actor->getPoints());
  pheromone_points = std::max(0, pheromone_points);
  pheromone_actor->changePoints(pheromone_points);
}

void StudentWorld::updateGameStatText() {
  stringstream ticker_stream;

  // Show tick counter.
  ticker_stream << "Ticks: " << std::right << std::setw(5) << 2000 - ticks_;

  // Only show dash if there are ant hills on the field.
  if (ant_hills_on_field_ > 0) ticker_stream << " - ";

  // For every ant hill on the field, display their name and score.
  for (int i = 0; i < ant_hills_on_field_; i++) {
    // No double space between counter and first ant hill.
    if (i > 0) ticker_stream << "  ";
    ticker_stream << scoreboard_names_[i];

    if (leader_ == i) {
      ticker_stream << "*";  // Star the ant hill currently in the lead.
    }

    // Show number of ants generated by that ant hill.
    ticker_stream << ": " << std::setw(2) << std::setfill('0')
                  << scoreboard_[i];
  }

  setGameStatText(ticker_stream.str());
}

std::list<Actor *> StudentWorld::actorsOfTypeAt(ActorType actor_type,
                                                Coordinate coord) {
  // List to be populated with the found actors of the given type at the
  // coordinate.
  std::list<Actor *> actors_of_type;

  // List of all actors at the coordinate.
  std::list<Actor *> actors_at_point = actors_[coord];
  for (std::list<Actor *>::const_iterator i = actors_at_point.begin();
       i != actors_at_point.end(); i++) {
    if (*i == nullptr) continue;  // Ignore deleted actors.

    // If the actor is of the type given in the parameter, add it to the return
    // list.
    if ((*i)->checkForObjectMatch(actor_type)) actors_of_type.push_back(*i);
  }

  return actors_of_type;
}

std::list<Actor *> StudentWorld::actorsOfTypesAt(
    std::vector<ActorType> actor_types, Coordinate coord) {
  std::list<Actor *> actors_of_types;

  // Iterate over every type given.
  for (int i = 0; i < actor_types.size(); i++) {
    // Concatenate the actors of the current type to the return list to build
    // a bigger, complete list of all actors of the given type at the given
    // coordinate.
    std::list<Actor *> actors_of_type = actorsOfTypeAt(actor_types[i], coord);

    actors_of_types.insert(actors_of_types.end(), actors_of_type.begin(),
                           actors_of_type.end());
  }

  return actors_of_types;
}

void StudentWorld::updatePositionInGrid(Actor *actor, Coordinate to_coord) {
  Coordinate from_coord = actor->getCoord();

  // Replace the pointer at the old coordinate for the actor with a nullptr,
  // which will be garbage collected (removed from this list) by the next tick.
  std::list<Actor *>::iterator i =
      std::find(actors_[from_coord].begin(), actors_[from_coord].end(), actor);
  (*i) = nullptr;

  // Update internal x and y values for the actor, and re-add actor to grid
  // with the new coordinates.
  actor->GraphObject::moveTo(to_coord.getX(), to_coord.getY());
  addActor(actor);
}

void StudentWorld::addActor(Actor *actor) {
  // Grab the map element for the coordinate that the actor will be added to.
  std::map<Coordinate, std::list<Actor *>>::iterator i;
  i = actors_.find(actor->getCoord());

  // If coordinate key exists, add actor to the list for that coordinate.
  if (i != actors_.end()) {
    i->second.push_back(actor);
    return;
  }

  // Key does not exist, so create a new coordinate key and add the actor to
  // that list.
  std::list<Actor *> actors_at_point;
  actors_at_point.push_back(actor);
  actors_[actor->getCoord()] = actors_at_point;
}

void StudentWorld::updateScoreboard(int colony) {
  // Cap colony number to avoid memory access errors.
  if (colony < 0 || colony > 3) {
    std::cerr << "INVALID COLONY: " << colony << std::endl;
    colony = 0;
  }

  // Increase total ants for the given colony.
  scoreboard_[colony]++;

  // Declare an initial leader once a colony has at least 6 ants.
  if (leader_ < 0 && scoreboard_[colony] > 5) leader_ = colony;

  // Set this colony as the leader only if it has more ants than the current
  // leader and has met the minimum 6 ant condition for being a leader in the
  // first place.
  if (scoreboard_[colony] > scoreboard_[leader_] && scoreboard_[colony] > 5)
    leader_ = colony;
}
