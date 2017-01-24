#include "ScoreList.h"

ScoreList::ScoreList() : sequence_() {}

bool ScoreList::add(unsigned long score) {
  if (score > 100) return false;

  return sequence_.insert(score) != -1 ? true : false;
}

bool ScoreList::remove(unsigned long score) {
  int index = sequence_.find(score);
  if(index == -1) return false;

  sequence_.erase(index);
  return true;
}

int ScoreList::size() const { return sequence_.size(); }

unsigned long ScoreList::minimum() const {
  unsigned long value;
  sequence_.get(0, value);

  return value == -1 ? NO_SCORE : value;
}

unsigned long ScoreList::maximum() const {
  unsigned long value;
  sequence_.get(sequence_.size() - 1, value);

  return value == -1 ? NO_SCORE : value;
}
