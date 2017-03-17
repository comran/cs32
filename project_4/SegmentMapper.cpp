#include "provided.h"
#include "support.h"
#include "MyMap.h"

#include <vector>
using namespace std;

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
  // Associate all street segments and attraction geocoords with any
  // geocoordinates that they are associated with.
  for (int i = 0; i < ml.getNumSegments(); i++) {
    StreetSegment current_segment;
    if (!ml.getSegment(i, current_segment))
      cerr << "Street DNE @ num " << i << endl;

    // Associate both sides of the street segment with the street.
    addPOI(current_segment.segment.start, current_segment);
    addPOI(current_segment.segment.end, current_segment);

    // Also associate all coordinates of attractions at that street segment with
    // the street segment.
    for (int i = 0; i < current_segment.attractions.size(); i++) {
      addPOI(current_segment.attractions.at(i).geocoordinates, current_segment);
    }
  }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord &gc) const {
  const vector<StreetSegment> *segments = segments_map_.find(gc);

  // Geocoord not found in map, so return empty vector.
  if (segments == nullptr) return vector<StreetSegment>();

  // Found segments associated with geocoord, so return the vector of street
  // segments.
  return *segments;
}

void SegmentMapperImpl::addPOI(const GeoCoord &gc,
                               const StreetSegment &segment) {
  vector<StreetSegment> *segments = segments_map_.find(gc);

  // If no street segments exists at the given coordinate, create a vector and
  // push back the given street segment.
  if (segments == nullptr) {
    vector<StreetSegment> new_segments;
    new_segments.push_back(segment);
    segments_map_.associate(gc, new_segments);

    return;
  }

  // Street segments already exist at the given coordinate (intersection?), so
  // append this new segment to that list.
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
