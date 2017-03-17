#include "provided.h"
#include "MyMap.h"

#include <string>
#include <fstream>
#include <vector>
using namespace std;

class MapLoaderImpl {
 public:
  MapLoaderImpl();
  ~MapLoaderImpl();
  bool load(string mapFile);
  size_t getNumSegments() const;
  bool getSegment(size_t segNum, StreetSegment &seg) const;

 private:
  vector<GeoCoord> findCoords(string text);

  vector<StreetSegment> street_segments_;
  enum LoadState { STREET_NAME, GEO_COORD, NUM_ATTRACTIONS, ATTRACTIONS };
};

MapLoaderImpl::MapLoaderImpl() {}
MapLoaderImpl::~MapLoaderImpl() {}

bool MapLoaderImpl::load(string mapFile) {
  ifstream in(mapFile);
  if (!in) {
    cerr << "Error: Cannot open data.txt!" << endl;
    return false;
  }

  // State machine for loading in the geocoords from the given map file.
  string line;
  LoadState state = STREET_NAME;
  StreetSegment current_segment;
  int num_attractions = 0;

  while (getline(in, line)) {
    switch (state) {
      case STREET_NAME: {
        // First part of the street segment information that just contains the
        // street name, so store this in the appropriate variable.
        current_segment.streetName = line;

        // Next line should always be the street's geo segment.
        state = GEO_COORD;
        break;
      }
      case GEO_COORD: {
        // Process the street's starting and ending coordinates, and store them
        // as a geo segment.
        vector<GeoCoord> coords = findCoords(line);
        current_segment.segment = GeoSegment(coords.at(0), coords.at(1));

        // Next line should always be a number indicating the number of
        // attractions on this street segment.
        state = NUM_ATTRACTIONS;
        break;
      }

      case NUM_ATTRACTIONS: {
        num_attractions = stoi(line.substr(0, line.size()));

        if (num_attractions > 0)
          // If there are attractions at the current street segment, then the
          // next num_attractions number of lines will be attractions on the
          // street.
          state = ATTRACTIONS;
        else
          // No attractions, so restart since next line should be a new street
          // segment.
          state = STREET_NAME;
        break;
      }

      case ATTRACTIONS: {
        // Divide the line between the name of the attraction and its
        // coordinates.
        int split = line.find_first_of("|");
        string name = line.substr(0, split);

        // Handle attraction's coordinate.
        GeoCoord coord =
            findCoords(line.substr(split + 1, line.size() - split)).at(0);

        current_segment.attractions.push_back(Attraction{name, coord});

        // Repeat until there are no longer any attractions, then proceed to
        // process the next street segment.
        if (--num_attractions <= 0) state = STREET_NAME;
        break;
      }
    }

    // If the next state is a reset, record the current segment.
    if (state == STREET_NAME) {
      street_segments_.push_back(current_segment);

      current_segment = StreetSegment();
    }
  }

  return true;
}

size_t MapLoaderImpl::getNumSegments() const { return street_segments_.size(); }

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const {
  // Return false on nonexistent segment number.
  if (segNum >= getNumSegments()) return false;

  seg = street_segments_.at(segNum);
  return true;
}

vector<GeoCoord> MapLoaderImpl::findCoords(string text) {
  int cur_coord = 0;
  vector<GeoCoord> coords;

  // Coordinate components are divided by either a comma or a space, so consider
  // any switches between a space/comma and any other character as a sign of a
  // new coordinate component. Repeat this process for the two coordinate
  // components.
  string first_component;
  bool in_coord = true;
  int start = 0, end = 0;

  // Traverse entire input string.
  for (int i = 0; i <= text.size(); i++) {
    // End last coordinate component at end of string.
    bool is_coordinate_component = i < text.size();

    // If not end of string, then check whether we should split due to a
    // delimiter that indicates a new coordinate component.
    if (is_coordinate_component)
      is_coordinate_component = text.at(i) != ' ' && text.at(i) != ',';

    if (!is_coordinate_component && in_coord) {
      end = i;  // Establish edge at the place where we noticed a comma/space.

      if (first_component == "") {
        // If first component is empty, then it needs to be filled before
        // filling the second component.
        first_component = text.substr(start, end - start);
      } else {
        coords.push_back(
            GeoCoord(first_component, text.substr(start, end - start)));
        first_component = "";
        cur_coord++;
      }

      in_coord = false;
    } else if (is_coordinate_component && !in_coord) {
      // Establish start when we notice an edge indicating the start of a new
      // coordinate component.
      start = i;

      in_coord = true;
    }
  }

  return coords;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader() { m_impl = new MapLoaderImpl; }

MapLoader::~MapLoader() { delete m_impl; }

bool MapLoader::load(string mapFile) { return m_impl->load(mapFile); }

size_t MapLoader::getNumSegments() const { return m_impl->getNumSegments(); }

bool MapLoader::getSegment(size_t segNum, StreetSegment &seg) const {
  return m_impl->getSegment(segNum, seg);
}
