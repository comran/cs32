#include "MyMap.h"
#include "provided.h"
#include "support.h"

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
  double temp_cost;
};

bool operator<(const TravelCost &a, const TravelCost &b) {
  return a.cost + a.temp_cost > b.cost + b.temp_cost;
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
  void finalizeNavSegments(vector<NavSegment> &segments) const;
  double trueAngle(GeoSegment &seg1, GeoSegment &seg2) const;

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

  priority_queue<TravelCost> to_go;
  vector<StreetSegment> init_segments = segment_mapper_.getSegments(src);
  MyMap<GeoCoord, bool> visited;

  for (int i = 0; i < init_segments.size(); i++) {
    vector<NavSegment> navigation;

    GeoCoord start = src;
    GeoSegment geo_segment = GeoSegment(start, GeoCoord());

    double distance = 0;
    string direction = "";
    string street_name = init_segments.at(i).streetName;

    navigation.push_back(NavSegment("", street_name, 0, geo_segment));

    to_go.push(TravelCost({init_segments.at(i), navigation, distance, 0}));
  }

  bool found = false;

  int integ = 0;
  while (to_go.size() > 0 && !found) {
    TravelCost segment_cost = to_go.top();

    to_go.pop();

    // See if dst exists on this street.
    for (int i = 0; i < segment_cost.segment.attractions.size(); i++) {
      if (segment_cost.segment.attractions.at(i).name == end) {
        NavSegment &last_segment = segment_cost.navigation.back();
        last_segment.m_geoSegment.end =
            segment_cost.segment.attractions.at(i).geocoordinates;
        last_segment.m_distance = distanceEarthMiles(
            last_segment.m_geoSegment.start, last_segment.m_geoSegment.end);

        finalizeNavSegments(segment_cost.navigation);

        found = true;

        directions = segment_cost.navigation;
        return NAV_SUCCESS;
      }
    }

    if (found) break;

    vector<GeoCoord> travel_to;
    travel_to.push_back(segment_cost.segment.segment.start);
    travel_to.push_back(segment_cost.segment.segment.end);

    for (int i = 0; i < travel_to.size(); i++) {
      bool *gc = visited.find(travel_to.at(i));
      if (gc != nullptr) continue;
      visited.associate(travel_to.at(i), true);

      vector<StreetSegment> new_segments =
          segment_mapper_.getSegments(travel_to.at(i));

      for (int j = 0; j < new_segments.size(); j++) {
        // Analyze only the street segments at GeoCoord junctions that are not
        // the same as the segment currently being analyzed.
        if (new_segments.at(j).segment == segment_cost.segment.segment)
          continue;

        vector<NavSegment> new_nav;
        new_nav.insert(new_nav.end(), segment_cost.navigation.begin(),
                       segment_cost.navigation.end());

        NavSegment &from_segment = new_nav.back();
        from_segment.m_geoSegment.end = travel_to.at(i);

        double distance = distanceEarthMiles(from_segment.m_geoSegment.start,
                                             from_segment.m_geoSegment.end);
        from_segment.m_distance = distance;

        NavSegment to_segment("", new_segments.at(j).streetName, 0,
                              GeoSegment(travel_to.at(i), GeoCoord()));
        new_nav.push_back(to_segment);

        to_go.push(TravelCost({new_segments.at(j), new_nav,
                               segment_cost.cost + distance,
                               distanceEarthMiles(travel_to.at(i), dst)}));
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

void NavigatorImpl::finalizeNavSegments(vector<NavSegment> &segments) const {
  for (int i = 0; i < segments.size(); i++) {
    if (i > 0) {
      if (segments.at(i).m_streetName != segments.at(i - 1).m_streetName) {
        double angle = angleBetween2Lines(segments.at(i - 1).m_geoSegment,
                                          segments.at(i).m_geoSegment);

        NavSegment turn_seg =
            NavSegment(turnAngleToString(angle), segments.at(i).m_streetName);

        segments.insert(segments.begin() + i, turn_seg);

        continue;
      } else {
        segments.at(i).m_direction = proceedAngleToString(trueAngle(
            segments.at(i - 1).m_geoSegment, segments.at(i).m_geoSegment));
      }
    } else {
      GeoSegment &geo_segment = segments.at(i).m_geoSegment;
      double distance = distanceEarthMiles(geo_segment.start, geo_segment.end);

      segments.at(i).m_distance = distance;
      segments.at(i).m_direction =
          proceedAngleToString(angleOfLine(segments.at(i).m_geoSegment));
    }
  }
}

double NavigatorImpl::trueAngle(GeoSegment &seg1, GeoSegment &seg2) const {
  return angleOfLine(seg1) + angleBetween2Lines(seg1, seg2);
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
