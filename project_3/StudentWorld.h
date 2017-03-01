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

class StudentWorld : public GameWorld {
 public:
  StudentWorld(std::string assetDir);
  ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
  void addFood(int x, int y, int food_points);
  void addPheromone(int x, int y, int pheromone_points, ActorType actor_type);
  void updateGameStatText();
  void updatePositionInGrid(Actor *actor, int to_x, int to_y);
  std::list<Actor *> actorsOfTypesAt(std::vector<ActorType> actor_types, int x,
                                     int y);
  std::list<Actor *> actorsOfTypeAt(ActorType actor_type, int x, int y);
  void addActor(Actor *actor);
  void updateScoreboard(int colony);
  std::list<int> getLeaders();

 private:
  Compiler *handleNewCompiler(int colony);

  std::vector<Actor *> actors_;
  std::list<Actor *> actors_map_[VIEW_WIDTH][VIEW_HEIGHT];
  int ticks_;
  int scoreboard_[4];
  std::string scoreboard_names_[4];
};

#endif  // STUDENTWORLD_H_
