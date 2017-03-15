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

  string line;
  LoadState state = STREET_NAME;
  StreetSegment current_segment;
  int num_attractions = 0;

  while (getline(in, line)) {
    switch (state) {
      case STREET_NAME: {
        current_segment.streetName = line;
        state = GEO_COORD;
        break;
      }
      case GEO_COORD: {
        vector<GeoCoord> coords = findCoords(line);
        current_segment.segment = GeoSegment(coords.at(0), coords.at(1));
        state = NUM_ATTRACTIONS;
        break;
      }

      case NUM_ATTRACTIONS: {
        num_attractions = stoi(line.substr(0, line.size()));
        if (num_attractions > 0)
          state = ATTRACTIONS;
        else
          state = STREET_NAME;
        break;
      }

      case ATTRACTIONS: {
        int split = line.find_first_of("|");
        string name = line.substr(0, split);
        GeoCoord coord =
            findCoords(line.substr(split + 1, line.size() - split)).at(0);

        current_segment.attractions.push_back(Attraction{name, coord});
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
  if (segNum >= getNumSegments()) return false;
  seg = street_segments_.at(segNum);

  return true;
}

vector<GeoCoord> MapLoaderImpl::findCoords(string text) {
  int cur_coord = 0;
  vector<GeoCoord> coords;

  string first_component;
  bool in_coord = true;
  int start = 0, end = 0;
  for (int i = 0; i <= text.size(); i++) {
    bool is_coordinate_component = i < text.size();
    if (is_coordinate_component)
      is_coordinate_component = text.at(i) != ' ' && text.at(i) != ',';

    if (!is_coordinate_component && in_coord) {
      end = i;

      if (first_component == "") {
        first_component = text.substr(start, end - start);
      } else {
        coords.push_back(
            GeoCoord(first_component, text.substr(start, end - start)));
        first_component = "";
        cur_coord++;
      }

      in_coord = false;
    } else if (is_coordinate_component && !in_coord) {
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
