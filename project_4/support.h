#ifndef SUPPORT_INCLUDED
#define SUPPORT_INCLUDED

#include "provided.h"

bool operator<(const GeoCoord &a, const GeoCoord &b);
bool operator>(const GeoCoord &a, const GeoCoord &b);
bool operator==(const GeoCoord &a, const GeoCoord &b);

bool operator==(const GeoCoord &a, const GeoCoord &b);
bool operator==(const GeoSegment &a, const GeoSegment &b);

#endif  // SUPPORT_INCLUDED
