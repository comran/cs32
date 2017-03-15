#include "provided.h"
#include "MyMap.h"

#include <iostream>
#include <string>
using namespace std;

class AttractionMapperImpl {
 public:
  AttractionMapperImpl();
  ~AttractionMapperImpl();
  void init(const MapLoader &ml);
  bool getGeoCoord(string attraction, GeoCoord &gc) const;

 private:
  string toLower(string input) const;

  MyMap<string, GeoCoord> attraction_map_;
};

AttractionMapperImpl::AttractionMapperImpl() {}

AttractionMapperImpl::~AttractionMapperImpl() {}

void AttractionMapperImpl::init(const MapLoader &ml) {
  for (int i = 0; i < ml.getNumSegments(); i++) {
    StreetSegment current_segment;
    if (!ml.getSegment(i, current_segment))
      cerr << "Street DNE @ num " << i << endl;

    for (int i = 0; i < current_segment.attractions.size(); i++) {
      attraction_map_.associate(
          toLower(current_segment.attractions.at(i).name),
          current_segment.attractions.at(i).geocoordinates);
    }
  }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord &gc) const {
  const GeoCoord *geo_coord = attraction_map_.find(toLower(attraction));
  if(geo_coord == nullptr) return false;
  gc = *geo_coord;

  return true;
}

string AttractionMapperImpl::toLower(string input) const {
  for(int j = 0;j < input.size();j++) {
    input[j] = tolower(input[j]);
  }

  return input;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper() { m_impl = new AttractionMapperImpl; }

AttractionMapper::~AttractionMapper() { delete m_impl; }

void AttractionMapper::init(const MapLoader &ml) { m_impl->init(ml); }

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord &gc) const {
  return m_impl->getGeoCoord(attraction, gc);
}
