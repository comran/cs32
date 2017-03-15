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

  vector<GeoCoord> have_gone;
  priority_queue<TravelCost> to_go;
  vector<StreetSegment> init_segments = segment_mapper_.getSegments(src);

  for (int i = 0; i < init_segments.size(); i++) {
    vector<NavSegment> navigation;

    GeoCoord start = src;
    GeoSegment geo_segment = GeoSegment(start, GeoCoord());

    double distance = 0;
    string direction = "";
    string street_name = init_segments.at(i).streetName;

    navigation.push_back(NavSegment("", street_name, 0, geo_segment));

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
        NavSegment &last_segment = segment_cost.navigation.back();
        last_segment.m_geoSegment.end =
            segment_cost.segment.attractions.at(i).geocoordinates;
        last_segment.m_distance = distanceEarthMiles(
            last_segment.m_geoSegment.start, last_segment.m_geoSegment.end);

        finalizeNavSegments(segment_cost.navigation);

        found = true;
        cout << "FOUND " << segment_cost.segment.streetName << "\n";
        cout << "distance: " << segment_cost.cost << "\n";
        cout << "iterate: " << iterate << "\n";
        printCoord(dst);

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
        // Analyze only the street segments at GeoCoord junctions that are not
        // the same as the segment currently being analyzed.
        if (new_segments.at(j).segment == segment_cost.segment.segment)
          continue;

        vector<NavSegment> new_nav;
        new_nav.insert(new_nav.end(), segment_cost.navigation.begin(),
                       segment_cost.navigation.end());

        NavSegment &from_segment = new_nav.back();
        from_segment.m_geoSegment.end = travel_to.at(i);
        from_segment.m_distance = distanceEarthMiles(
            from_segment.m_geoSegment.start, from_segment.m_geoSegment.end);
        from_segment.m_streetName = new_segments.at(i).streetName;

        NavSegment to_segment("", new_segments.at(i).streetName, 0,
                              GeoSegment(travel_to.at(i), GeoCoord()));
        new_nav.push_back(to_segment);

        to_go.push(TravelCost({new_segments.at(j), new_nav,
                               segment_cost.cost + from_segment.m_distance}));
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
        double angle = angleBetween2Lines(
                segments.at(i - 1).m_geoSegment, segments.at(i).m_geoSegment);
        NavSegment turn_seg = NavSegment(
            turnAngleToString(angle),
            segments.at(i).m_streetName);
        cout << setw(8) << turnAngleToString(angle) << ": " << setw(10) << angle << endl;

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

// TODO(comran):
// REMOVE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int main() {
  Navigator nav;
  nav.loadMapData("./mapdata.txt");

  vector<NavSegment> directions;
  NavResult nav_return =
      nav.navigate("Brentwood Country Mart", "Theta Delta Chi Fraternity", directions);

  cout << "Navigation returned " << nav_return << endl;
  for (int i = 0; i < directions.size(); i++) {
    cout << left << "i: " << setw(3) << i << " command: " << setw(3)
         << directions.at(i).m_command << " direction: " << setw(10)
         << directions.at(i).m_direction << " street name: " << setw(30)
         << directions.at(i).m_streetName;

    if (directions.at(i).m_command == NavSegment::PROCEED) {
      cout << " distance: " << setw(10) << setprecision(2)
           << directions.at(i).m_distance
           << " start: " << directions.at(i).m_geoSegment.start.latitudeText
           << ", " << directions.at(i).m_geoSegment.start.longitudeText
           << "      end: " << directions.at(i).m_geoSegment.end.latitudeText
           << ", " << directions.at(i).m_geoSegment.end.longitudeText;
    }
    cout << endl;
  }

  cout << endl << "And now some raw coordinates that can be plotted using "
    << "https://www.darrinward.com/lat-long/?id=2738666" << endl;
  for(int i = 0;i < directions.size();i++) {
    if(directions.at(i).m_geoSegment.start.latitudeText == "0") continue;
    cout
           << directions.at(i).m_geoSegment.start.latitudeText
           << ", " << directions.at(i).m_geoSegment.start.longitudeText << endl;
  }
}

// This is the BruinNav main routine.  If the executable built from this file
// and the other .cpp files you write is named BruinNav (or BruinNav.exe on
// Windows), then you can run it with
//  ./BruinNav theMapDataFileName "Start Attraction" "End Attraction"
// to get the turn-by-turn instructions a user wants to see, or
//  ./BruinNav theMapDataFileName "Start Attraction" "End Attraction" -raw
// to see the sequence of NavSegments produced by Navigator::navigate()
// (Under Windows, say "BruinNav" instead of "./BruinNav")
// For example, with the mapdata.txt file we supplied you,
//  ./BruinNav mapdata.txt "Harvard-Westlake Middle School" "GreyStone Mansion"
// should produce something like
//   Routing...                     
//   You are starting at: Harvard-Westlake Middle School
//   Proceed 0.47 miles southeast on Brooklawn Drive
//   Turn right onto Angelo Drive
//   Proceed 0.43 miles east on Angelo Drive
//   Turn right onto Benedict Canyon Drive
//   Proceed 0.13 miles southeast on Benedict Canyon Drive
//   Turn left onto Hartford Way
//   Proceed 0.13 miles east on Hartford Way
//   Turn left onto Lexington Road
//   Proceed 0.63 miles east on Lexington Road
//   Turn right onto Alpine Drive
//   Proceed 0.07 miles southeast on Alpine Drive
//   Turn left onto West Sunset Boulevard
//   Proceed 0.20 miles northeast on West Sunset Boulevard
//   Turn left onto Mountain Drive
//   Proceed 0.15 miles northeast on Mountain Drive
//   Turn left onto Schuyler Road
//   Proceed 0.19 miles north on Schuyler Road
//   Turn right onto Stonewood Drive
//   Proceed 0.15 miles northeast on Stonewood Drive
//   You have reached your destination: GreyStone Mansion
//   Total travel distance: 2.5 miles
// and
//  ./BruinNav mapdata.txt "Harvard-Westlake Middle School" "GreyStone Mansion" -raw
// might produce 133 lines starting
//   Start: Harvard-Westlake Middle School
//   End:   GreyStone Mansion
//   34.0904161,-118.4344198 34.0905309,-118.4343340 northeast 0.0093 Brooklawn Drive
//   34.0905309,-118.4343340 34.0904815,-118.4341398 east 0.0116 Brooklawn Drive
//   34.0904815,-118.4341398 34.0903824,-118.4339467 southeast 0.0130 Brooklawn Drive
//   34.0903824,-118.4339467 34.0902310,-118.4337702 southeast 0.0145 Brooklawn Drive
//   34.0902310,-118.4337702 34.0900681,-118.4335630 southeast 0.0163 Brooklawn Drive
//   34.0900681,-118.4335630 34.0899633,-118.4334635 southeast 0.0092 Brooklawn Drive
//   34.0899633,-118.4334635 34.0897917,-118.4333739 southeast 0.0129 Brooklawn Drive
//   34.0897917,-118.4333739 34.0894654,-118.4333087 south 0.0229 Brooklawn Drive
// and ending
//   34.0904163,-118.4036377 34.0905573,-118.4036590 north 0.0098 Schuyler Road
//   34.0905573,-118.4036590 34.0908428,-118.4038080 northwest 0.0215 Schuyler Road
//   turn right Stonewood Drive
//   34.0908428,-118.4038080 34.0908832,-118.4036471 east 0.0096 Stonewood Drive
//   34.0908832,-118.4036471 34.0908732,-118.4034846 east 0.0093 Stonewood Drive
//   34.0908732,-118.4034846 34.0908807,-118.4033732 east 0.0064 Stonewood Drive
//   34.0908807,-118.4033732 34.0909505,-118.4031144 east 0.0156 Stonewood Drive
//   34.0909505,-118.4031144 34.0909630,-118.4030512 east 0.0037 Stonewood Drive
//   34.0909630,-118.4030512 34.0909256,-118.4029338 east 0.0072 Stonewood Drive
//   34.0909256,-118.4029338 34.0919749,-118.4018226 northeast 0.0964 Stonewood Drive
//
// If you build the program as is, you'll notice the turn-by-turn instructions
// say IN_SOME_DIRECTION instead of east or southwest or some actual direction.
// That's because of the template appearing a few lines below; read the comment 
// before it.
/*
#include "provided.h"
//#include "support.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
using namespace std;

// START OF WHAT YOU CAN REMOVE ONCE YOU'VE IMPLEMENTED string directionOfLine(const GeoSegment& gs)
// If you want the turn-by-turn directions to give a real direction like
// east or southwest instead of IN_SOME_DIRECTION, you'll need to
// implement the ordinary function
//    string directionOfLine(const GeoSegment& gs)
// to return a string like "east" or "southwest" based on the angle of the
// GeoSegment gs according to the table at the bottom of page 20 of the spec.
// When you do that, you can delete this comment and the template function
// below that appears here solely to allow this main.cpp to build. 
// Why didn't we just write the real function for you?  Because it's also
// a function you'd find useful in producing the NavSegments in the navigate()
// method.  Since it's useful in more than one .cpp file, its declaration
// should go in support.h and its implementation in support.cpp.

template<typename T>
string directionOfLine(const T& t)
{
  double angle = angleOfLine(t);
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
// END OF WHAT YOU CAN REMOVE ONCE YOU'VE IMPLEMENTED string directionOfLine(const GeoSegment& gs)

void printDirectionsRaw(string start, string end, vector<NavSegment>& navSegments);
void printDirections(string start, string end, vector<NavSegment>& navSegments);

int main(int argc, char *argv[])
{
  bool raw = false; 
  if (argc == 5  &&  strcmp(argv[4], "-raw") == 0)
  {
    raw = true;
    argc--;
  }
  if (argc != 4)
  {
    cout << "Usage: BruinNav mapdata.txt \"start attraction\" \"end attraction name\"" << endl
       << "or" << endl
       << "Usage: BruinNav mapdata.txt \"start attraction\" \"end attraction name\" -raw" << endl;
    return 1;
  }
  
  Navigator nav;

  if ( ! nav.loadMapData(argv[1]))
  {
    cout << "Map data file was not found or has bad format: " << argv[1] << endl;
    return 1;
  }

  if ( ! raw)
    cout << "Routing..." << flush;

  string start = argv[2];
  string end = argv[3];
  vector<NavSegment> navSegments;

  NavResult result = nav.navigate(start, end, navSegments);
  if ( ! raw)
    cout << endl;

  switch (result)
  {
    case NAV_NO_ROUTE:
    cout << "No route found between " << start << " and " << end << endl;
    break;
    case NAV_BAD_SOURCE:
    cout << "Start attraction not found: " << start << endl;
    break;
    case NAV_BAD_DESTINATION:
    cout << "End attraction not found: " << end << endl;
    break;
    case NAV_SUCCESS:
    if (raw)
      printDirectionsRaw(start, end, navSegments);
    else
      printDirections(start, end, navSegments);
    break;
  }
}

void printDirectionsRaw(string start, string end, vector<NavSegment>& navSegments)
{
  cout << "Start: " << start << endl;
  cout << "End:   " << end << endl;
  cout.setf(ios::fixed);
  cout.precision(4);
  for (auto ns : navSegments)
  {
    switch (ns.m_command)
    {
      case NavSegment::PROCEED:
      cout << ns.m_geoSegment.start.latitudeText << ","
           << ns.m_geoSegment.start.longitudeText << " "
           << ns.m_geoSegment.end.latitudeText << ","
           << ns.m_geoSegment.end.longitudeText << " "
           << ns.m_direction << " "
           << ns.m_distance << " "
           << ns.m_streetName << endl;
      break;
      case NavSegment::TURN:
      cout << "turn " << ns.m_direction << " " << ns.m_streetName << endl;
      break;
    }
  }
}

void printDirections(string start, string end, vector<NavSegment>& navSegments)
{
  cout.setf(ios::fixed);
  cout.precision(2);

  cout << "You are starting at: " << start << endl;

  double totalDistance = 0;
  string thisStreet;
  GeoSegment effectiveSegment;
  double distSinceLastTurn = 0;

  for (auto ns : navSegments)
  {
    switch (ns.m_command)
    {
      case NavSegment::PROCEED:
      if (thisStreet.empty())
      {
        thisStreet = ns.m_streetName;
        effectiveSegment.start = ns.m_geoSegment.start;
      }
      effectiveSegment.end = ns.m_geoSegment.end;
      distSinceLastTurn += ns.m_distance;
      totalDistance += ns.m_distance;
      break;
      case NavSegment::TURN:
      if (distSinceLastTurn > 0)
      {
        cout << "Proceed " << distSinceLastTurn << " miles "
             << directionOfLine(effectiveSegment) << " on " << thisStreet << endl;
        thisStreet.clear();
        distSinceLastTurn = 0;
      }
      cout << "Turn " << ns.m_direction << " onto " << ns.m_streetName << endl;
      break;
    }
  }

  if (distSinceLastTurn > 0)
    cout << "Proceed " << distSinceLastTurn << " miles "
         << directionOfLine(effectiveSegment) << " on " << thisStreet << endl;
  cout << "You have reached your destination: " << end << endl;
  cout.precision(1);
  cout << "Total travel distance: " << totalDistance << " miles" << endl;
}*/
