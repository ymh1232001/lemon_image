#ifndef RASTER_H
#define RASTER_H

#include <map>
#include <ogr_spatialref.h>

#include "metadata.h"

class GDALDataset;

namespace lemon {
class vector;

enum class numeric_type : std::uint8_t {
  unsigned_int_8_bit,
  unsigned_int_16_bit,
  unsigned_int_32_bit,
  signed_int_16_bit,
  signed_int_32_bit,
  float_32_bit,
  float_64_bit,
  complex_int_16_bit,   // 暂不支持
  complex_int_32_bit,   // 暂不支持
  complex_float_32_bit, // 暂不支持
  complex_float_64_bit, // 暂不支持
  unknown
};

struct color_entry {
  std::int16_t r;
  std::int16_t g;
  std::int16_t b;
  std::int16_t a;
};

struct metadata_domain {
  std::map<std::string, std::string> default_domain;
  std::map<std::string, std::string> image_structure_domain;
  std::map<std::string, std::string> geolocation_domain;
  std::map<std::string, std::string> rpc_domain;
};

struct raster_band_info {
  bool statistics_computed;
  bool have_no_data_value;
  bool no_data_value_is_nan;
  int block_width;
  int block_height;
  int num_overviews;
  double min;
  double max;
  double mean;
  double std_dev;
  double no_data_value;
  std::string description;
  std::string color_interpretation;
  std::string overview_resample_method;
};

struct raster_metadata : metadata {
  int num_bands;
  int width;
  int height;
  double left;
  double right;
  double top;
  double bottom;
  double pixel_width;
  double pixel_height;
  metadata_domain metadata_params;
  std::vector<raster_band_info> band_info;
  std::string data_type;
  std::string data_order;
  std::string compression;
};

class raster {
public:
  raster();
  ~raster();

  inline const raster_metadata *metadata() const {
    return static_cast<const raster_metadata *>(&metadata_);
  }
  inline bool is_loaded() { return load_flag_; }
  inline std::string error_str() { return error_str_; }
  inline std::vector<color_entry> color_table() { return color_table_; }
  // 临时解决方案
  inline const OGRSpatialReference *ogr_spatial_ref() const {
    return &spatial_ref_;
  }

  static void init();
  static void uninit();

  void build_overview(); // TODO
  void close();
  bool load(const std::string &pathname, bool compute_statistics = false);
  bool compute_statistics(bool approximate); // TODO : 计算统计值

  template <typename T>
  T *read_block(int band_index, int start_row, int start_column, int width,
                int height, double zoom_multiple = 1.0);

private:
  static bool init_flag_;

  GDALDataset *dataset_;
  OGRSpatialReference spatial_ref_;

  bool load_flag_;
  std::string error_str_;
  std::vector<color_entry> color_table_;
  raster_metadata metadata_;
};
}

#endif // RASTER_H
