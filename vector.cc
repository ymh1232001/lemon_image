// #include <iostream>
#include <ogr_api.h>
#include <ogrsf_frmts.h>

#include "vector.h"
#include "utility.h"
#include "spatial_ref.h"

namespace lemon {
bool vector::init_flag_ = false;

vector::vector() { load_flag_ = false; }

vector::~vector() {
  if (load_flag_)
    if (data_source_ != nullptr)
      OGRDataSource::DestroyDataSource(data_source_);
}

void vector::init() {
  if (!init_flag_) {
    OGRRegisterAll();
    init_flag_ = true;
  } else {
    // std::cerr << "[警告]vector : 无效的重复初始化" << //std::cout;
  }
}

void vector::uninit() {
  if (init_flag_) {
    OGRCleanupAll();
    init_flag_ = false;
  } else {
    // std::cerr << "[警告]vector : 无效的重复反初始化" << //std::cout;
  }
}

bool vector::load(const std::string &pathname) {
  if (!init_flag_) {
    error_str_ = "vector : 未初始化";
    // std::cerr << "[错误]" << error_str_ << std::endl;
    return false;
  }

  if (load_flag_) {
    error_str_ = "vector : 重复加载";
    // std::cerr << "[错误]" << error_str_ << std::endl;
    return false;
  }

  CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
  CPLSetConfigOption("SHAPE_ENCODING", "");

  OGRDataSource *data_source = OGRSFDriverRegistrar::Open(pathname.c_str());
  if (nullptr == data_source)
    return false;

  int num_layers = data_source->GetLayerCount();

  vector_metadata metadata;

  metadata.layer_name.resize(num_layers);
  metadata.geometry_type.resize(num_layers);
  metadata.num_features.resize(num_layers);
  metadata.extents.resize(num_layers);

  std::vector<std::vector<std::string>> field_name(num_layers);

  std::vector<std::vector<std::vector<std::string>>> attribute_table(
      num_layers);

  int feature_index = 0;

  OGRLayer *layer;
  OGRFeature *feature;
  OGRFieldDefn *field_defn;
  OGRGeometry *geometry;

  for (int layer_index = 0; layer_index < num_layers; ++layer_index) {
    layer = data_source->GetLayer(layer_index);
    if (nullptr == layer) {
      // print error
      OGRDataSource::DestroyDataSource(data_source);
      return false;
    }

    int &&num_features = layer->GetFeatureCount();

    metadata.layer_name[layer_index] = layer->GetName();
    layer->GetExtent(&metadata.extents[layer_index]);
    metadata.geometry_type[layer_index] =
        OGRGeometryTypeToName(layer->GetGeomType());
    metadata.num_features[layer_index] = num_features;
    attribute_table[layer_index].resize(num_features);

    OGRFeatureDefn *feature_defn = layer->GetLayerDefn();

    int &&num_fields = feature_defn->GetFieldCount();

    layer->ResetReading();

    for (int field_index = 0; field_index < num_fields; ++field_index)
      field_name[layer_index].emplace_back(
          feature_defn->GetFieldDefn(field_index)->GetNameRef());

    while ((feature = layer->GetNextFeature()) != nullptr) {
      for (int field_index = 0; field_index < num_fields; ++field_index) {
        field_defn = feature_defn->GetFieldDefn(field_index);

        attribute_table[layer_index][feature_index].emplace_back(
            feature->GetFieldAsString(field_index));
      }

      OGRFeature::DestroyFeature(feature);

      ++feature_index;
    }
  }

  metadata.num_layers = num_layers;
  metadata.pathname = pathname;
  metadata.filename = get_filename(pathname);
  metadata.data_format = data_source->GetDriver()->GetName();
  metadata.total_size = 0;

  std::vector<std::string> &&file_list = get_vector_pathname_list(pathname);
  for (auto &e : file_list) {
    metadata.file_list.emplace_back(get_filename(e));
    metadata.file_size.emplace_back(get_filesize(e));
    metadata.total_size += metadata.file_size.back();
  }

  attribute_table_ = std::move(attribute_table);

  field_name_ = std::move(field_name);

  spatial_ref_ = layer->GetSpatialRef();
  get_spatial_ref(spatial_ref_, static_cast<lemon::metadata *>(&metadata));

  data_source_ = data_source;
  metadata_ = metadata;

  load_flag_ = true;

  return true;
}

void vector::close() {
  if (load_flag_) {
    load_flag_ = false;
    OGRDataSource::DestroyDataSource(data_source_);
  } else {
    // std::cerr << "[警告]vector : 无效的关闭操作" << // std::endl;
  }
}

bool vector::is_loaded() { return load_flag_; }

std::string vector::error_str() { return error_str_; }
}
