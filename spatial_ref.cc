#include <memory>

#include "spatial_ref.h"
#include "raster.h"
#include "vector.h"

namespace lemon {
geographic_coordinate_system *
get_geographic_cs(OGRSpatialReference *spatial_ref) {
  geographic_coordinate_system *geogcs = new geographic_coordinate_system();

  geogcs->name = spatial_ref->GetAttrValue("GEOGCS");

  const char *authority_name = spatial_ref->GetAuthorityName("GEOGCS");
  const char *authority_code = spatial_ref->GetAuthorityCode("GEOGCS");
  if (authority_name != nullptr || authority_code != nullptr) {
    geogcs->authority_name = authority_name;
    geogcs->authority_code = authority_code;
  }

  geogcs->datum = spatial_ref->GetAttrValue("DATUM");
  geogcs->spheroid = spatial_ref->GetAttrValue("SPHEROID");
  geogcs->semimajor = spatial_ref->GetSemiMajor(nullptr);
  geogcs->semiminor = spatial_ref->GetSemiMinor(nullptr);
  geogcs->inverse_flattening = spatial_ref->GetInvFlattening(nullptr);

  char *unit = nullptr;

  geogcs->radians_per_unit = spatial_ref->GetAngularUnits(&unit);
  geogcs->angular_unit = unit;

  char *meridian = nullptr;
  geogcs->meridian_longitude = spatial_ref->GetPrimeMeridian(&meridian);
  geogcs->prime_meridian = meridian;

  double coefficients[7];
  if (OGRERR_NONE == spatial_ref->GetTOWGS84(coefficients)) {
    geogcs->have_towgs84_node = true;

    for (int i = 0; i < 7; ++i)
      geogcs->towgs84_params[i] = coefficients[i];
  } else {
    geogcs->have_towgs84_node = false;
  }

  return geogcs;
}

projected_coordinate_system *
get_projected_cs(OGRSpatialReference *spatial_ref) {
  projected_coordinate_system *proj_cs = new projected_coordinate_system();

  proj_cs->name = spatial_ref->GetAttrValue("PROJCS");
  proj_cs->projection_name = spatial_ref->GetAttrValue("PROJECTION");

  const char *authority_name = spatial_ref->GetAuthorityName("PROJCS");
  const char *authority_code = spatial_ref->GetAuthorityCode("PROJCS");
  if (authority_name != nullptr || authority_code != nullptr) {
    proj_cs->authority_name = authority_name;
    proj_cs->authority_code = authority_code;
  }

  char *unit = nullptr;

  proj_cs->meters_per_unit = spatial_ref->GetLinearUnits(&unit);
  proj_cs->linear_unit = unit;

  auto get_projection_param = [&proj_cs, &spatial_ref](const char *key) {
    if (spatial_ref->FindProjParm(key) != -1)
      proj_cs->projection_params.emplace(std::make_pair<std::string, double>(
          key, spatial_ref->GetNormProjParm(key)));
  };

  get_projection_param(SRS_PP_CENTRAL_MERIDIAN);
  get_projection_param(SRS_PP_SCALE_FACTOR);
  get_projection_param(SRS_PP_STANDARD_PARALLEL_1);
  get_projection_param(SRS_PP_STANDARD_PARALLEL_2);
  get_projection_param(SRS_PP_PSEUDO_STD_PARALLEL_1);
  get_projection_param(SRS_PP_LONGITUDE_OF_CENTER);
  get_projection_param(SRS_PP_LATITUDE_OF_CENTER);
  get_projection_param(SRS_PP_LONGITUDE_OF_ORIGIN);
  get_projection_param(SRS_PP_LATITUDE_OF_ORIGIN);
  get_projection_param(SRS_PP_FALSE_EASTING);
  get_projection_param(SRS_PP_FALSE_NORTHING);
  get_projection_param(SRS_PP_AZIMUTH);
  get_projection_param(SRS_PP_LONGITUDE_OF_POINT_1);
  get_projection_param(SRS_PP_LATITUDE_OF_POINT_1);
  get_projection_param(SRS_PP_LONGITUDE_OF_POINT_2);
  get_projection_param(SRS_PP_LATITUDE_OF_POINT_2);
  get_projection_param(SRS_PP_LONGITUDE_OF_POINT_3);
  get_projection_param(SRS_PP_LATITUDE_OF_POINT_3);
  get_projection_param(SRS_PP_RECTIFIED_GRID_ANGLE);
  get_projection_param(SRS_PP_LANDSAT_NUMBER);
  get_projection_param(SRS_PP_PATH_NUMBER);
  get_projection_param(SRS_PP_PERSPECTIVE_POINT_HEIGHT);
  get_projection_param(SRS_PP_SATELLITE_HEIGHT);
  get_projection_param(SRS_PP_FIPSZONE);
  get_projection_param(SRS_PP_ZONE);
  get_projection_param(SRS_PP_LATITUDE_OF_1ST_POINT);
  get_projection_param(SRS_PP_LONGITUDE_OF_1ST_POINT);
  get_projection_param(SRS_PP_LATITUDE_OF_2ND_POINT);
  get_projection_param(SRS_PP_LONGITUDE_OF_2ND_POINT);

  std::unique_ptr<OGRSpatialReference> geog_cs(spatial_ref->CloneGeogCS());

  std::unique_ptr<geographic_coordinate_system> ptr(
      get_geographic_cs(geog_cs.get()));
  proj_cs->geog_cs = *ptr;

  return proj_cs;
}

// 暂不支持地方坐标系统
local_coordinate_system *get_local_cs(OGRSpatialReference *spatial_ref) {
  local_coordinate_system *local_cs = new local_coordinate_system();

  return local_cs;
}

vertical_coordinate_system *get_vertical_cs(OGRSpatialReference *spatial_ref) {
  vertical_coordinate_system *vert_cs = new vertical_coordinate_system();

  vert_cs->name = spatial_ref->GetAttrValue("VERT_CS");

  const char *authority_name = spatial_ref->GetAuthorityName("VERT_CS");
  const char *authority_code = spatial_ref->GetAuthorityCode("VERT_CS");
  if (authority_name != nullptr || authority_code != nullptr) {
    vert_cs->authority_name = authority_name;
    vert_cs->authority_code = authority_code;
  }

  vert_cs->datum = spatial_ref->GetAttrValue("VERT_DATUM");

  char *unit = nullptr;

  vert_cs->meters_per_unit = spatial_ref->GetLinearUnits(&unit);
  vert_cs->linear_unit = unit;

  OGRAxisOrientation axis_orientation;

  const char *axis_name = spatial_ref->GetAxis("VERT_CS", 0, &axis_orientation);
  if (axis_name != nullptr) {
    vert_cs->axis_name = axis_name;
    vert_cs->axis_orientation = OSRAxisEnumToName(axis_orientation);
  }

  return vert_cs;
}

void get_spatial_ref(OGRSpatialReference *spatial_ref, metadata *metadata) {
  if (spatial_ref != nullptr) {
    spatial_ref->AutoIdentifyEPSG();
    if (spatial_ref->IsCompound()) {
      const OGR_SRSNode *proj_cs_node = spatial_ref->GetAttrNode("PROJCS");
      if (proj_cs_node != nullptr) {
        std::unique_ptr<OGRSpatialReference> proj_cs =
            std::make_unique<OGRSpatialReference>();
        proj_cs->SetRoot(proj_cs_node->Clone());

        metadata->proj_cs.reset(get_projected_cs(proj_cs.get()));
      }

      if (spatial_ref->GetAttrNode("GEOGCS") != nullptr ||
          spatial_ref->GetAttrNode("GEOCCS") != nullptr) {
        std::unique_ptr<OGRSpatialReference> geog_cs(
            spatial_ref->CloneGeogCS());

        metadata->geog_cs.reset(get_geographic_cs(geog_cs.get()));
      }

      const OGR_SRSNode *local_cs_node = spatial_ref->GetAttrNode("LOCAL_CS");
      if (local_cs_node != nullptr) {
        std::unique_ptr<OGRSpatialReference> local_cs =
            std::make_unique<OGRSpatialReference>();
        local_cs->SetRoot(local_cs_node->Clone());

        metadata->local_cs.reset(get_local_cs(local_cs.get()));
      }

      const OGR_SRSNode *vert_cs_node = spatial_ref->GetAttrNode("VERT_CS");
      if (vert_cs_node != nullptr) {
        std::unique_ptr<OGRSpatialReference> vert_cs =
            std::make_unique<OGRSpatialReference>();
        vert_cs->SetRoot(vert_cs_node->Clone());

        metadata->vert_cs.reset(get_vertical_cs(vert_cs.get()));
      }
    } else if (spatial_ref->IsProjected()) {
      metadata->proj_cs.reset(get_projected_cs(spatial_ref));
    } else if (spatial_ref->IsGeographic() || spatial_ref->IsGeocentric()) {
      metadata->geog_cs.reset(get_geographic_cs(spatial_ref));
    } else if (spatial_ref->IsLocal()) {
      metadata->local_cs.reset(get_local_cs(spatial_ref));
    } else if (spatial_ref->IsVertical()) {
      metadata->vert_cs.reset(get_vertical_cs(spatial_ref));
    }
  }
}

bool is_same_spatial_ref(const raster *raster, const vector *vector) {
  return raster->ogr_spatial_ref()->IsSame(vector->ogr_spatial_ref());
}

bool is_same_spatial_ref(const raster *raster_1, const raster *raster_2) {
  return raster_1->ogr_spatial_ref()->IsSame(raster_2->ogr_spatial_ref());
}

bool is_same_spatial_ref(const vector *vector_1, const vector *vector_2) {
  return vector_1->ogr_spatial_ref()->IsSame(vector_2->ogr_spatial_ref());
}

bool is_same_spatial_ref(const raster *raster,
                         const OGRSpatialReference *ogr_spatial_ref) {
  return raster->ogr_spatial_ref()->IsSame(ogr_spatial_ref);
}

bool is_same_spatial_ref(const vector *vector,
                         const OGRSpatialReference *ogr_spatial_ref) {
  return vector->ogr_spatial_ref()->IsSame(ogr_spatial_ref);
}
}
