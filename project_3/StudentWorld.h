#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <string>
#include <vector>
#include <list>

#include "GameWorld.h"
#include "GameConstants.h"
#include "Compiler.h"

class Actor;
enum class ActorType;

class Coordinate {
 public:
  Coordinate(int x, int y);
  bool operator<(const Coordinate& l) const;

  // Getters/setters for scalars of coordinate.
  int getX() const;
  int getY() const;
  void setX(int x);
  void setY(int y);

  // Returns the coordinate one step in the given direction.
  Coordinate coordInDirection(GraphObject::Direction direction);

 private:
  int x_, y_;
};

class StudentWorld : public GameWorld {
 public:
  StudentWorld(std::string assetDir);
  ~StudentWorld();

  // Overrided methods required for GameWorld.
  virtual int init();
  virtual int move();
  virtual void cleanUp();

  // Add items to the field at their proper positions in the STL map data
  // structure used in this project.
  void addActor(Actor *actor);
  void addFood(Coordinate coord, int food_points);
  void addPheromone(Coordinate coord, int pheromone_points,
                    int colony);

  // Updater to sync up actor coordinates with the order of actors in the STL
  // map.
  void updatePositionInGrid(Actor *actor, Coordinate to_coord);

  // Methods of searching for specific types of actors at a given coordinate.
  std::list<Actor *> actorsOfTypeAt(ActorType actor_type, Coordinate coord);
  std::list<Actor *> actorsOfTypesAt(std::vector<ActorType> actor_types,
                                     Coordinate to_coord);  // Plural type form.
  // Updater for game ticker text and scoreboard.
  void updateGameStatText();
  void updateScoreboard(int colony);

 private:
  Compiler *handleNewCompiler(int colony);
  std::map<Coordinate, std::list<Actor*>> actors_;
  std::string scoreboard_names_[4];

  int ticks_;
  int scoreboard_[4];
  int leader_;
  int ant_hills_on_field_;
};

#endif  // STUDENTWORLD_H_
