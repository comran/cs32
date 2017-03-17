#include "MyMap.h"
#include "provided.h"
#include "support.h"

#include <string>
#include <vector>
#include <queue>
using namespace std;

struct TravelCost {
  StreetSegment segment;
  vector<NavSegment> navigation;
  double cost;       // Integrated cost, used for distance traveled so far.
  double temp_cost;  // Temp cost, used for straight distance from end to dst.
};

bool operator<(const TravelCost &a, const TravelCost &b) {
  return a.cost + a.temp_cost > b.cost + b.temp_cost;
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

  // Sanitize the given start/end attractions based on whether they exist.
  if (!attraction_mapper_.getGeoCoord(start, src)) return NAV_BAD_SOURCE;
  if (!attraction_mapper_.getGeoCoord(end, dst)) return NAV_BAD_DESTINATION;

  // Priority queue is sorted by the integral of distance traveled by the
  // element up to the point that the element was popped, plus the distance
  // between the end of the segment and the destination coordinate.
  // This will prioritize shorter segments, and also favor segments that trend
  // towards the destination.
  priority_queue<TravelCost> to_go;
  vector<StreetSegment> init_segments = segment_mapper_.getSegments(src);
  MyMap<GeoCoord, bool> visited;  // Treat MyMap like a std::set (bool unused).

  // Populate our priority queue with any segments associated with the start
  // coordinate.
  for (int i = 0; i < init_segments.size(); i++) {
    vector<NavSegment> navigation;

    GeoCoord start = src;
    GeoSegment geo_segment = GeoSegment(start, GeoCoord());

    double distance = 0;
    string direction = "";
    string street_name = init_segments.at(i).streetName;

    navigation.push_back(NavSegment("", street_name, 0, geo_segment));

    // Initial cost is total distance traveled in the current step and the
    // distance that would be required to get to the destination.
    to_go.push(TravelCost({init_segments.at(i), navigation, distance,
                           distanceEarthMiles(geo_segment.end, dst)}));
  }

  while (to_go.size() > 0) {
    TravelCost segment_cost = to_go.top();
    to_go.pop();

    // See if dst exists on this street.
    for (int i = 0; i < segment_cost.segment.attractions.size(); i++) {
      if (segment_cost.segment.attractions.at(i).name == end) {
        // Finalize the last segment in the NavSegment history for the
        // successful element popped from the priority_queue.
        NavSegment &last_segment = segment_cost.navigation.back();
        last_segment.m_geoSegment.end =
            segment_cost.segment.attractions.at(i).geocoordinates;
        last_segment.m_distance = distanceEarthMiles(
            last_segment.m_geoSegment.start, last_segment.m_geoSegment.end);

        finalizeNavSegments(segment_cost.navigation);

        directions = segment_cost.navigation;
        return NAV_SUCCESS;
      }
    }

    // Consider both ends of the street.
    vector<GeoCoord> travel_to;
    travel_to.push_back(segment_cost.segment.segment.start);
    travel_to.push_back(segment_cost.segment.segment.end);

    for (int i = 0; i < travel_to.size(); i++) {
      // Don't re-examine geocoords that we already visited.
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

        // Finish off the last NavSegment and start a new one containing just
        // the name of the current street and the current coordinate.
        // This makes each NavSegment a continuation of the one before it.
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

        // Sort this possible route in the priority_queue based on its current
        // length and the distance between the end of the head of the route and
        // the destination.
        // This prioritizes shorter routes and routes that will get closer to
        // the destination faster.
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
        // Turn found, so fill in the appropriate fields and insert a turn
        // segment.
        double angle = angleBetween2Lines(segments.at(i - 1).m_geoSegment,
                                          segments.at(i).m_geoSegment);

        NavSegment turn_seg =
            NavSegment(turnAngleToString(angle), segments.at(i).m_streetName);
        segments.insert(segments.begin() + i, turn_seg);

        continue;
      } else {
        // Fill in the direction that the proceed segment faces.
        segments.at(i).m_direction = proceedAngleToString(trueAngle(
            segments.at(i - 1).m_geoSegment, segments.at(i).m_geoSegment));
      }
    } else {
      // Start segment, so base the direction off of the raw angle of the
      // segment rather than basing the angle off the previous segment.
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
