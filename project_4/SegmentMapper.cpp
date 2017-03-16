#include "provided.h"
#include "support.h"
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

  MyMap<GeoCoord, vector<StreetSegment>> segments_map_;
};

SegmentMapperImpl::SegmentMapperImpl() {}

SegmentMapperImpl::~SegmentMapperImpl() {}

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
  const vector<StreetSegment> *segments = segments_map_.find(gc);

  if (segments == nullptr) return vector<StreetSegment>();
  return *segments;
}

void SegmentMapperImpl::addPOI(const GeoCoord &gc,
                               const StreetSegment &segment) {
  vector<StreetSegment> *segments = segments_map_.find(gc);

  if (segments == nullptr) {
    vector<StreetSegment> new_segments;
    new_segments.push_back(segment);
    segments_map_.associate(gc, new_segments);

    return;
  }

  segments->push_back(segment);
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
