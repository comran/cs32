#include "provided.h"
#include "MyMap.h"

#include <iostream>
#include <vector>
using namespace std;

// TODO(comran): Delete segment map properly on deconstruction.

class SegmentMapperImpl {
 public:
  SegmentMapperImpl();
  ~SegmentMapperImpl();
  void init(const MapLoader &ml);
  vector<StreetSegment> getSegments(const GeoCoord &gc) const;

 private:
  void addPOI(const GeoCoord &gc, const StreetSegment &segment);

  MyMap<double, MyMap<double, vector<StreetSegment>> *> segments_map_;
  vector<MyMap<double, vector<StreetSegment>> *> segments_vector_;
};

SegmentMapperImpl::SegmentMapperImpl() {}

SegmentMapperImpl::~SegmentMapperImpl() {
  for(int i = 0;i < segments_vector_.size();i++) delete segments_vector_.at(i);
}

void SegmentMapperImpl::init(const MapLoader &ml) {
  for (int i = 0; i < ml.getNumSegments(); i++) {
    StreetSegment current_segment;
    if (!ml.getSegment(i, current_segment))
      cerr << "Street DNE @ num " << i << endl;

    addPOI(current_segment.segment.start, current_segment);
    addPOI(current_segment.segment.end, current_segment);
    for (int i = 0; i < current_segment.attractions.size(); i++) {
      addPOI(current_segment.attractions.at(i).geocoordinates, current_segment);
    }
  }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord &gc) const {
  MyMap<double, vector<StreetSegment>> *const*latitude =
      segments_map_.find(gc.latitude);
  if(latitude == nullptr) return vector<StreetSegment>();
  vector<StreetSegment> *longitude = (*latitude)->find(gc.longitude);
  if(longitude == nullptr) return vector<StreetSegment>();
  return *longitude;
}

void SegmentMapperImpl::addPOI(const GeoCoord &gc,
                               const StreetSegment &segment) {
  MyMap<double, vector<StreetSegment>> **latitude =
      segments_map_.find(gc.latitude);

  if (latitude != nullptr) {
    vector<StreetSegment> *segments = (*latitude)->find(gc.longitude);

    if (segments != nullptr) {
      segments->push_back(segment);
      return;
    }

    vector<StreetSegment> new_segments;
    new_segments.push_back(segment);
    (*latitude)->associate(gc.longitude, new_segments);
  }

  vector<StreetSegment> new_segments;
  new_segments.push_back(segment);

  MyMap<double, vector<StreetSegment>> *new_latitude =
      new MyMap<double, vector<StreetSegment>>();
  new_latitude->associate(gc.longitude, new_segments);

  segments_map_.associate(gc.latitude, new_latitude);
  segments_vector_.push_back(new_latitude);
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper() { m_impl = new SegmentMapperImpl; }

SegmentMapper::~SegmentMapper() { delete m_impl; }

void SegmentMapper::init(const MapLoader &ml) { m_impl->init(ml); }

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord &gc) const {
  return m_impl->getSegments(gc);
}
