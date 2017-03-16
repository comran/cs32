#include "support.h"

bool operator<(const GeoCoord &a, const GeoCoord &b) {
  if(a.latitude == b.latitude) return a.longitude < b.longitude;
  return a.latitude < b.latitude;
}

bool operator>(const GeoCoord &a, const GeoCoord &b){
  if(a.latitude == b.latitude) return a.longitude > b.longitude;
  return a.latitude > b.latitude;
}

bool operator==(const GeoCoord &a, const GeoCoord &b) {
  return a.latitude == b.latitude && a.longitude == b.longitude;
}

bool operator==(const GeoSegment &a, const GeoSegment &b) {
  return a.start == b.start && a.end == b.end;
}
