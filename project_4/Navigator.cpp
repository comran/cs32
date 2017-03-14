#include "MyMap.h"
#include "provided.h"

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <math.h>
#include <unistd.h>
using namespace std;

struct TravelCost {
  StreetSegment segment;
  double cost;
};

bool operator<(const TravelCost &a, const TravelCost &b) {
  return a.cost > b.cost;
}

bool operator==(const GeoCoord &a, const GeoCoord &b) {
  return a.latitudeText == b.latitudeText && a.longitudeText == b.longitudeText;
}

bool operator==(const GeoSegment &a, const GeoSegment &b) {
  return a.start == b.start && a.end == b.end;
}

void printSeg(const StreetSegment &seg) {
  cout << seg.streetName << " " << seg.segment.start.latitudeText << ", "
       << seg.segment.start.longitudeText << "  "
       << seg.segment.end.latitudeText << ", " << seg.segment.end.longitudeText
       << endl;
}

class NavigatorImpl {
 public:
  NavigatorImpl();
  ~NavigatorImpl();
  bool loadMapData(string mapFile);
  NavResult navigate(string start, string end,
                     vector<NavSegment> &directions) const;

 private:
  string proceedAngleToString(double angle);
  string turnAngleToString(double angle);

  AttractionMapper attraction_mapper_;
  SegmentMapper segment_mapper_;
};

NavigatorImpl::NavigatorImpl() {}

NavigatorImpl::~NavigatorImpl() {}

bool NavigatorImpl::loadMapData(string mapFile) {
  MapLoader map_loader;
  map_loader.load(mapFile);
  attraction_mapper_.init(map_loader);
  segment_mapper_.init(map_loader);

  return true;
}

NavResult NavigatorImpl::navigate(string start, string end,
                                  vector<NavSegment> &directions) const {
  GeoCoord src, dst;
  if (!attraction_mapper_.getGeoCoord(start, src)) return NAV_BAD_SOURCE;
  if (!attraction_mapper_.getGeoCoord(end, dst)) return NAV_BAD_DESTINATION;

  vector<GeoCoord> have_gone;
  priority_queue<TravelCost> to_go;
  vector<StreetSegment> init_segments = segment_mapper_.getSegments(src);

  for (int i = 0; i < init_segments.size(); i++) {
    double distance = distanceEarthMiles(init_segments.at(i).segment.start,
                                         init_segments.at(i).segment.end);
    to_go.push(TravelCost({init_segments.at(i), distance}));
  }

  cout << "Init size: " << to_go.size() << endl;

  bool found = false;
  int iterate = 0;
  while (to_go.size() > 0 && !found) {
    cout << "Iterate: " << iterate++ << endl;
    cout << to_go.size();
    cout << "_______________________________________________________________\n";

    TravelCost segment_cost = to_go.top();
    to_go.pop();

    printSeg(segment_cost.segment);
    cout << segment_cost.cost << endl;

    // See if dst exists on this street.
    for (int i = 0; i < segment_cost.segment.attractions.size(); i++) {
      if (segment_cost.segment.attractions.at(i).name == end) {
        found = true;
        cout << "FOUND " << segment_cost.segment.streetName << "\n";
        break;
      }
    }

    if (found) break;

    vector<GeoCoord> travel_to;
    travel_to.push_back(segment_cost.segment.segment.start);
    travel_to.push_back(segment_cost.segment.segment.end);

    for (int i = 0; i < have_gone.size(); i++) {
      for (int j = 0; j < travel_to.size(); j++) {

        if (have_gone.at(i) == travel_to.at(j)) {
          travel_to.erase(travel_to.begin() + j);
        }
      }
    }

    for(int i = 0;i < travel_to.size();i++) {
      have_gone.push_back(travel_to.at(i));
    }

    for (int i = 0; i < travel_to.size(); i++) {
      vector<StreetSegment> new_segments =
          segment_mapper_.getSegments(travel_to.at(i));

      for (int j = 0; j < new_segments.size(); j++) {
        if (new_segments.at(j).segment == segment_cost.segment.segment) {
          continue;
        }
        printSeg(new_segments.at(j));

        double distance = distanceEarthMiles(new_segments.at(i).segment.start,
                                             new_segments.at(i).segment.end);

        to_go.push(TravelCost(
            {new_segments.at(j), segment_cost.cost + distance}));
      }
    }
  }

  return NAV_NO_ROUTE;
}

string NavigatorImpl::proceedAngleToString(double angle) {
  angle = fmod(angle, 360.0);

  if (angle < 0 || angle >= 360)
    return "INVALID";
  else if (angle <= 22.5)
    return "east";
  else if (angle <= 67.5)
    return "northeast";
  else if (angle <= 112.5)
    return "north";
  else if (angle <= 157.5)
    return "northwest";
  else if (angle <= 202.5)
    return "west";
  else if (angle <= 247.5)
    return "southwest";
  else if (angle <= 292.5)
    return "south";
  else if (angle <= 337.5)
    return "southeast";
  else if (angle < 360)
    return "east";

  return "INVALID";
}

string NavigatorImpl::turnAngleToString(double angle) {
  if (angle < 0 || angle >= 360) return "INVALID";

  if (angle < 180) return "left";
  return "right";
}

//******************** Navigator functions
//************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator() { m_impl = new NavigatorImpl; }

Navigator::~Navigator() { delete m_impl; }

bool Navigator::loadMapData(string mapFile) {
  return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end,
                              vector<NavSegment> &directions) const {
  return m_impl->navigate(start, end, directions);
}

// TODO(comran):
// REMOVE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int main() {
  Navigator nav;
  nav.loadMapData("./mapdata.txt");

  vector<NavSegment> directions;
  cout << nav.navigate("Diddy Riese", "Theta Delta Chi Fraternity", directions) << endl;
}
