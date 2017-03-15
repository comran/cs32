#include "MyMap.h"
#include "provided.h"

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <iomanip>
#include <math.h>
#include <unistd.h>
using namespace std;

struct TravelCost {
  StreetSegment segment;
  vector<NavSegment> navigation;
  double cost;
};

bool operator<(const TravelCost &a, const TravelCost &b) {
  return a.cost > b.cost;
}

bool operator==(const GeoCoord &a, const GeoCoord &b) {
  return a.latitude == b.latitude && a.longitude == b.longitude;
}

bool operator==(const GeoSegment &a, const GeoSegment &b) {
  return a.start == b.start && a.end == b.end;
}

void printStreetSeg(const StreetSegment &seg) {
  cout << seg.streetName << " " << seg.segment.start.latitudeText << ", "
       << seg.segment.start.longitudeText << "  "
       << seg.segment.end.latitudeText << ", " << seg.segment.end.longitudeText
       << endl;
}

void printSeg(const GeoSegment &seg) {
  cout << seg.start.latitudeText << ", " << seg.start.longitudeText << "  "
       << seg.end.latitudeText << ", " << seg.end.longitudeText << endl;
}

void printCoord(const GeoCoord &coord) {
  cout << coord.latitudeText << ", " << coord.longitudeText << endl;
}

class NavigatorImpl {
 public:
  NavigatorImpl();
  ~NavigatorImpl();
  bool loadMapData(string mapFile);
  NavResult navigate(string start, string end,
                     vector<NavSegment> &directions) const;

 private:
  string proceedAngleToString(double angle) const;
  string turnAngleToString(double angle) const;

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
    vector<NavSegment> navigation;

    GeoCoord start = src;
    GeoCoord end = init_segments.at(i).segment.end;
    GeoSegment geo_segment = GeoSegment(start, end);

    double distance = distanceEarthMiles(start, end);
    double angle = angleOfLine(geo_segment);
    string direction = proceedAngleToString(angle);
    string street_name = init_segments.at(i).streetName;

    navigation.push_back(
        NavSegment(direction, street_name, distance, geo_segment));

    to_go.push(TravelCost({init_segments.at(i), navigation, distance}));
  }

  bool found = false;
  int iterate = 0;

  while (to_go.size() > 0 && !found) {
    iterate++;
    if (iterate % 1000 == 0) cout << "Ran: " << iterate << endl;

    TravelCost segment_cost = to_go.top();
    to_go.pop();

    // See if dst exists on this street.
    for (int i = 0; i < segment_cost.segment.attractions.size(); i++) {
      if (segment_cost.segment.attractions.at(i).name == end) {
        if (segment_cost.navigation.size() == 0) {
          // TODO(comran): Re-compute navigation for same segment.
          // segment_cost.navigation.at(1).m_
        }
        found = true;
        cout << "FOUND " << segment_cost.segment.streetName << "\n";
        cout << "distance: " << segment_cost.cost << "\n";
        cout << "iterate: " << iterate << "\n";

        directions = segment_cost.navigation;
        return NAV_SUCCESS;
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
          j--;
        }
      }
    }

    for (int i = 0; i < travel_to.size(); i++) {
      have_gone.push_back(travel_to.at(i));
    }

    for (int i = 0; i < travel_to.size(); i++) {
      vector<StreetSegment> new_segments =
          segment_mapper_.getSegments(travel_to.at(i));

      for (int j = 0; j < new_segments.size(); j++) {
        if (new_segments.at(j).segment == segment_cost.segment.segment) {
          continue;
        }

        GeoCoord start = new_segments.at(i).segment.start;
        GeoCoord end = new_segments.at(i).segment.end;

        GeoSegment geo_segment = GeoSegment(start, end);
        double distance = distanceEarthMiles(start, end);
        NavSegment::NavCommand command =
            new_segments.at(j).streetName == segment_cost.segment.streetName
                ? NavSegment::PROCEED
                : NavSegment::TURN;
        double angle;
            angleBetween2Lines(geo_segment, segment_cost.segment.segment);
        string street_name = new_segments.at(i).streetName;

        if (command == NavSegment::PROCEED) {
          angle = angleBetween2Lines(geo_segment, segment_cost.segment.segment) + angleOfLine(geo_segment);
          street_name = new_segments.at(i).streetName;
        } else {
          angle = angleOfLine(segment_cost.segment.segment);
          street_name = new_segments.at(i).streetName;
        }

        string direction = command == NavSegment::PROCEED
                               ? proceedAngleToString(angle)
                               : turnAngleToString(angle);

        printSeg(geo_segment);

        vector<NavSegment> navigation;
        navigation.insert(navigation.end(), segment_cost.navigation.begin(),
                          segment_cost.navigation.end());

        if (command == NavSegment::PROCEED)
          navigation.push_back(
              NavSegment(direction, street_name, distance, geo_segment));
        else
          navigation.push_back(NavSegment(direction, street_name));

        to_go.push(TravelCost(
            {new_segments.at(j), navigation, segment_cost.cost + distance}));
      }
    }
  }

  return NAV_NO_ROUTE;
}

string NavigatorImpl::proceedAngleToString(double angle) const {
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

string NavigatorImpl::turnAngleToString(double angle) const {
  if (angle < 0 || angle >= 360) return "INVALID";

  if (angle < 180) return "left";
  return "right";
}

//******************** Navigator functions ************************************

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
  NavResult nav_return =
      nav.navigate("1061 Broxton Avenue", "Headlines!", directions);

  cout << "Navigation returned " << nav_return << endl;
  for (int i = 0; i < directions.size(); i++) {
    cout << left << "i: " << setw(3) << i << " command: " << setw(3)
         << directions.at(i).m_command << " direction: " << setw(10)
         << directions.at(i).m_direction << " street name: " << setw(30) << directions.at(i).m_streetName;
    if (directions.at(i).m_command == NavSegment::PROCEED) {
      cout << " distance: " << setw(10)
           << setprecision(2) << directions.at(i).m_distance
           << " start: " << directions.at(i).m_geoSegment.start.latitudeText
           << ", " << directions.at(i).m_geoSegment.start.longitudeText
           << "      end: " << directions.at(i).m_geoSegment.end.latitudeText
           << ", " << directions.at(i).m_geoSegment.end.longitudeText;
    }
    cout << endl;
  }
}
