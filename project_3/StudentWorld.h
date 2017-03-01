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
  int getX() const;
  int getY() const;
  void setX(int x);
  void setY(int y);
  Coordinate coordInDirection(GraphObject::Direction direction);

 private:
  int x_, y_;
};

class StudentWorld : public GameWorld {
 public:
  StudentWorld(std::string assetDir);
  ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
  void addFood(Coordinate coord, int food_points);
  void addPheromone(Coordinate coord, int pheromone_points,
                    ActorType actor_type);
  void updateGameStatText();
  void updatePositionInGrid(Actor *actor, Coordinate to_coord);
  std::list<Actor *> actorsOfTypesAt(std::vector<ActorType> actor_types,
                                     Coordinate to_coord);
  std::list<Actor *> actorsOfTypeAt(ActorType actor_type, Coordinate coord);
  void addActor(Actor *actor);
  void updateScoreboard(int colony);
  std::list<int> getLeaders();

 private:
  Compiler *handleNewCompiler(int colony);

  std::map<Coordinate, std::list<Actor*>> actors_;
  int ticks_;
  int scoreboard_[4];
  std::string scoreboard_names_[4];
};

#endif  // STUDENTWORLD_H_
