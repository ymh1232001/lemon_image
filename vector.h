#ifndef VECTOR_H
#define VECTOR_H

#include <ogr_core.h>

#include "metadata.h"

class OGRDataSource;
class OGRSpatialReference;

namespace lemon {
class raster;

struct vector_metadata : public metadata {
  int num_layers;
  std::vector<int> num_features;
  std::vector<OGREnvelope> extents;
  std::vector<std::string> layer_name;
  std::vector<std::string> geometry_type;
};

class vector {
public:
  vector();
  ~vector();

  inline const vector_metadata *metadata() const {
    return static_cast<const vector_metadata *>(&metadata_);
  }
  inline const std::vector<std::string> *field_name(const int &index) const {
    return &field_name_[index];
  }
  inline const std::vector<std::vector<std::string>> *
  attribute_table(const int &index) const {
    return &attribute_table_[index];
  }
  // 临时解决方案
  inline const OGRSpatialReference *ogr_spatial_ref() const {
    return spatial_ref_;
  }

  static void init();
  static void uninit();

  bool load(const std::string &pathname);
  void close();
  bool is_loaded();
  std::string error_str();

private:
  static bool init_flag_;

  OGRDataSource *data_source_;
  OGRSpatialReference *spatial_ref_;

  bool load_flag_;
  std::string error_str_;
  vector_metadata metadata_;
  std::vector<std::vector<std::string>> field_name_;
  std::vector<std::vector<std::vector<std::string>>> attribute_table_;
};
}

#endif // VECTOR_H
