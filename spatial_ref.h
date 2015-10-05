#ifndef SPATIAL_REF_H
#define SPATIAL_REF_H

#include <ogr_spatialref.h>

#include "metadata.h"

namespace lemon {
class raster;
class vector;

// 仅库内部使用
void get_spatial_ref(OGRSpatialReference *spatial_ref,
                     lemon::metadata *metadata);

// 临时解决方案
bool is_same_spatial_ref(const raster *raster,
                         const OGRSpatialReference *ogr_spatial_ref);
bool is_same_spatial_ref(const vector *vector,
                         const OGRSpatialReference *ogr_spatial_ref);
bool is_same_spatial_ref(const raster *raster, const vector *vector);
bool is_same_spatial_ref(const raster *raster_1, const raster *raster_2);
bool is_same_spatial_ref(const vector *vector_1, const vector *vector_2);
}

#endif // SPATIAL_REF_H
