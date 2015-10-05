#ifndef METADATA_H
#define METADATA_H

#include <array>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace lemon {
struct geographic_coordinate_system {
  bool have_towgs84_node;
  double semimajor;
  double semiminor;
  double meridian_longitude;
  double inverse_flattening;
  double radians_per_unit;
  std::array<double, 7> towgs84_params;
  std::string name;
  std::string datum;
  std::string spheroid;
  std::string angular_unit;
  std::string authority_name;
  std::string authority_code;
  std::string prime_meridian;
};

struct projected_coordinate_system {
  geographic_coordinate_system geog_cs;
  double meters_per_unit;
  std::unordered_map<std::string, double> projection_params;
  std::string name;
  std::string projection_name;
  std::string authority_name;
  std::string authority_code;
  std::string linear_unit;
};

struct local_coordinate_system {};

struct vertical_coordinate_system {
  double meters_per_unit;
  std::string name;
  std::string datum;
  std::string axis_name;
  std::string axis_orientation;
  std::string authority_name;
  std::string authority_code;
  std::string linear_unit;
};

struct metadata {
  std::shared_ptr<projected_coordinate_system> proj_cs;
  std::shared_ptr<geographic_coordinate_system> geog_cs;
  std::shared_ptr<local_coordinate_system> local_cs;
  std::shared_ptr<vertical_coordinate_system> vert_cs;
  std::uintmax_t total_size;
  std::vector<std::uintmax_t> file_size;
  std::vector<std::string> file_list;
  std::string unit;
  std::string pathname;
  std::string filename;
  std::string data_format;
};
}

#endif // METADATA_H
