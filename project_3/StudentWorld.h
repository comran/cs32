#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <string>
#include <vector>
#include <list>

#include "GameWorld.h"
#include "GameConstants.h"

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
  void updateGameStatText();
  void updatePositionInGrid(Actor *actor, int to_x, int to_y);
  std::list<Actor *> actorsOfTypeAt(ActorType actor_type, int x, int y);

 private:
  std::vector<Actor *> actors_;
  std::list<Actor *> actors_map_[VIEW_WIDTH][VIEW_HEIGHT];
  int ticks_;
};

#endif  // STUDENTWORLD_H_
