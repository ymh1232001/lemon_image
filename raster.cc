// #include <iostream>
#include <gdal_priv.h>

#include "raster.h"
#include "utility.h"
#include "spatial_ref.h"

namespace lemon {
bool raster::init_flag_ = false;

raster::raster() { load_flag_ = false; }

raster::~raster() {
  if (load_flag_) {
    if (dataset_ != nullptr)
      GDALClose((GDALDatasetH)dataset_);
  }
}

void raster::init() {
  if (!init_flag_) {
    GDALAllRegister();
    init_flag_ = true;
  } else {
    // std::cerr << "[警告]raster : 无效的重复初始化" << std::cout;
  }
}

void raster::uninit() {
  if (init_flag_) {
    GDALDestroyDriverManager();
    init_flag_ = false;
  } else {
    // std::cerr << "[警告]raster : 无效的重复反初始化" << //std::cout;
  }
}

bool raster::load(const std::string &pathname, bool compute_statistics) {
  if (!init_flag_) {
    error_str_ = "raster : 未初始化";
    // std::cerr << "[错误]" << error_str_ << std::endl;
    return false;
  }

  if (load_flag_) {
    error_str_ = "raster : 重复加载";
    // std::cerr << "[错误]" << error_str_ << std::endl;
    return false;
  }

  CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

  GDALDataset *dataset = (GDALDataset *)GDALOpen(pathname.c_str(), GA_ReadOnly);
  if (nullptr == dataset)
    return false;

  int &&num_bands = dataset->GetRasterCount();

  metadata_.pathname = pathname;
  metadata_.filename = get_filename(pathname);
  metadata_.total_size = 0;
  metadata_.num_bands = num_bands;
  metadata_.width = dataset->GetRasterXSize();
  metadata_.height = dataset->GetRasterYSize();

  char **file_list = dataset->GetFileList();

  char *path = nullptr;
  int file_index = 0;

  while (true) {
    path = file_list[file_index];
    if (nullptr == path)
      break;

    metadata_.file_list.emplace_back(
        lemon::get_filename(static_cast<const char *>(path)));
    metadata_.file_size.emplace_back(
        get_filesize(static_cast<const char *>(path)));
    metadata_.total_size += metadata_.file_size.back();

    ++file_index;
  }

  CSLDestroy(file_list);

  // FIXME : 未考虑变换
  double geo_transform[6] = {0};
  dataset->GetGeoTransform(geo_transform);
  metadata_.left = geo_transform[0];
  metadata_.right = geo_transform[0] + metadata_.width * geo_transform[1];
  metadata_.top = geo_transform[3];
  metadata_.bottom =
      geo_transform[3] + metadata_.height * std::abs(geo_transform[5]);
  metadata_.pixel_width = geo_transform[1];
  metadata_.pixel_height = geo_transform[5];

  metadata_.band_info.resize(num_bands);

  GDALRasterBand *band = nullptr;

  raster_band_info *band_info = nullptr;

  int got_no_data;

  for (int band_index = 0; band_index < num_bands; ++band_index) {
    band = dataset->GetRasterBand(band_index + 1);

    band_info = &metadata_.band_info[band_index];

    band_info->color_interpretation =
        GDALGetColorInterpretationName(band->GetColorInterpretation());

    band->GetBlockSize(&band_info->block_width, &band_info->block_height);

    std::string description = band->GetDescription();
    band_info->description = (description.empty()) ? "None" : description;

    band_info->no_data_value = band->GetNoDataValue(&got_no_data);
    if (got_no_data) {
      band_info->have_no_data_value = true;

      if (CPLIsNan(band_info->no_data_value))
        band_info->no_data_value_is_nan = true;
      else
        band_info->no_data_value_is_nan = false;
    } else {
      band_info->have_no_data_value = false;
      band_info->no_data_value_is_nan = false;
    }

    if (compute_statistics) {
      if (CE_None !=
          band->GetStatistics(TRUE, FALSE, &band_info->min, &band_info->max,
                              &band_info->mean, &band_info->std_dev))
        band->ComputeStatistics(TRUE, &band_info->min, &band_info->max,
                                &band_info->mean, &band_info->std_dev, nullptr,
                                nullptr);

      // TODO : 传入Callable Object用于显示进度

      band_info->statistics_computed = true;
    } else {
      band_info->statistics_computed = false;
    }

    band_info->num_overviews = band->GetOverviewCount();
    if (band_info->num_overviews > 0) {
      GDALRasterBand *overview = band->GetOverview(0);
      const char *resample_method = overview->GetMetadataItem("RESAMPLING", "");
      band_info->overview_resample_method =
          (nullptr == resample_method) ? "Unknown" : resample_method;
    } else {
      band_info->overview_resample_method = "No overview present";
    }
  }

  GDALDataType &&data_type = band->GetRasterDataType();
  metadata_.data_type = GDALGetDataTypeName(data_type);

  metadata_.data_format = GDALGetDriverLongName(dataset->GetDriver());

  std::string::size_type pos;
  auto split =
      [&pos](const std::string &text) -> std::pair<std::string, std::string> {
        pos = text.find("=");

        return std::make_pair(text.substr(0, pos), text.substr(pos + 1));
      };

  char **metadata = dataset->GetMetadata(nullptr);
  if (CSLCount(metadata) > 0)
    for (int i = 0; metadata[i] != nullptr; ++i)
      metadata_.metadata_params.default_domain.emplace(split(metadata[i]));

  metadata = dataset->GetMetadata("IMAGE_STRUCTURE");
  if (CSLCount(metadata) > 0)
    for (int i = 0; metadata[i] != nullptr; ++i)
      metadata_.metadata_params.image_structure_domain.emplace(
          split(metadata[i]));

  metadata = dataset->GetMetadata("GEOLOCATION");
  if (CSLCount(metadata) > 0)
    for (int i = 0; metadata[i] != nullptr; ++i)
      metadata_.metadata_params.geolocation_domain.emplace(split(metadata[i]));

  metadata = dataset->GetMetadata("RPC");
  if (CSLCount(metadata) > 0)
    for (int i = 0; metadata[i] != nullptr; ++i)
      metadata_.metadata_params.rpc_domain.emplace(split(metadata[i]));

  auto search =
      metadata_.metadata_params.image_structure_domain.find("INTERLEAVE");
  if (search != metadata_.metadata_params.image_structure_domain.end()) {
    std::string value = search->second;
    if ("LINE" == value)
      metadata_.data_order = "BIL";
    else if ("BAND" == value)
      metadata_.data_order = "BSQ";
    else
      metadata_.data_order = "BIP";
  } else {
    metadata_.data_order = "Unknown";
  }

  search = metadata_.metadata_params.image_structure_domain.find("COMPRESSION");

  metadata_.compression = metadata_.compression =
      (search == metadata_.metadata_params.image_structure_domain.end())
          ? "None"
          : search->second;

  OGRSpatialReference spatial_ref(dataset->GetProjectionRef());
  get_spatial_ref(&spatial_ref, static_cast<lemon::metadata *>(&metadata_));
  spatial_ref_ = spatial_ref;

  if (1 == num_bands) {
    GDALColorTable *color_table = band->GetColorTable();
    if (color_table != nullptr) {
      const GDALColorEntry *color_entry = nullptr;

      int num_color_entries = color_table->GetColorEntryCount();
      for (int i = 0; i < num_color_entries; ++i) {
        color_entry = color_table->GetColorEntry(i);
        color_table_.emplace_back(
            lemon::color_entry{color_entry->c1, color_entry->c2,
                               color_entry->c3, color_entry->c4});
      }
    }
  }

  dataset_ = dataset;

  load_flag_ = true;

  return true;
}

void raster::close() {
  if (load_flag_) {
    load_flag_ = false;
    GDALClose((GDALDatasetH)dataset_);
  } else {
    // std::cerr << "[警告]raster : 无效的关闭操作" << //std::endl;
  }
}

template <>
std::uint8_t *raster::read_block<std::uint8_t>(int band_index, int start_row,
                                               int start_column, int width,
                                               int height,
                                               double zoom_multiple) {
  int buffer_width = static_cast<int>(width * zoom_multiple);
  int buffer_height = static_cast<int>(height * zoom_multiple);

  std::uint8_t *data = new std::uint8_t[buffer_width * buffer_height];

  dataset_->GetRasterBand(++band_index)
      ->RasterIO(GF_Read, start_row, start_column, width, height, data,
                 buffer_width, buffer_height, GDT_Byte, 0, 0);

  return data;
}

template <>
std::uint16_t *raster::read_block<std::uint16_t>(int band_index, int start_row,
                                                 int start_column, int width,
                                                 int height,
                                                 double zoom_multiple) {
  int buffer_width = static_cast<int>(width * zoom_multiple);
  int buffer_height = static_cast<int>(height * zoom_multiple);

  std::uint16_t *data = new std::uint16_t[buffer_width * buffer_height];

  dataset_->GetRasterBand(++band_index)
      ->RasterIO(GF_Read, start_row, start_column, width, height, data,
                 buffer_width, buffer_height, GDT_UInt16, 0, 0);

  return data;
}

template <>
std::int16_t *raster::read_block<std::int16_t>(int band_index, int start_row,
                                               int start_column, int width,
                                               int height,
                                               double zoom_multiple) {
  int buffer_width = static_cast<int>(width * zoom_multiple);
  int buffer_height = static_cast<int>(height * zoom_multiple);

  std::int16_t *data = new std::int16_t[buffer_width * buffer_height];

  dataset_->GetRasterBand(++band_index)
      ->RasterIO(GF_Read, start_row, start_column, width, height, data,
                 buffer_width, buffer_height, GDT_Int16, 0, 0);

  return data;
}

template <>
std::uint32_t *raster::read_block<std::uint32_t>(int band_index, int start_row,
                                                 int start_column, int width,
                                                 int height,
                                                 double zoom_multiple) {
  int buffer_width = static_cast<int>(width * zoom_multiple);
  int buffer_height = static_cast<int>(height * zoom_multiple);

  std::uint32_t *data = new std::uint32_t[buffer_width * buffer_height];

  dataset_->GetRasterBand(++band_index)
      ->RasterIO(GF_Read, start_row, start_column, width, height, data,
                 buffer_width, buffer_height, GDT_UInt32, 0, 0);

  return data;
}

template <>
std::int32_t *raster::read_block<std::int32_t>(int band_index, int start_row,
                                               int start_column, int width,
                                               int height,
                                               double zoom_multiple) {
  int buffer_width = static_cast<int>(width * zoom_multiple);
  int buffer_height = static_cast<int>(height * zoom_multiple);

  std::int32_t *data = new std::int32_t[buffer_width * buffer_height];

  dataset_->GetRasterBand(++band_index)
      ->RasterIO(GF_Read, start_row, start_column, width, height, data,
                 buffer_width, buffer_height, GDT_Int32, 0, 0);

  return data;
}

template <>
float *raster::read_block<float>(int band_index, int start_row,
                                 int start_column, int width, int height,
                                 double zoom_multiple) {
  int buffer_width = static_cast<int>(width * zoom_multiple);
  int buffer_height = static_cast<int>(height * zoom_multiple);

  float *data = new float[buffer_width * buffer_height];

  dataset_->GetRasterBand(++band_index)
      ->RasterIO(GF_Read, start_row, start_column, width, height, data,
                 buffer_width, buffer_height, GDT_Float32, 0, 0);

  return data;
}

template <>
double *raster::read_block<double>(int band_index, int start_row,
                                   int start_column, int width, int height,
                                   double zoom_multiple) {
  int buffer_width = static_cast<int>(width * zoom_multiple);
  int buffer_height = static_cast<int>(height * zoom_multiple);

  double *data = new double[buffer_width * buffer_height];

  dataset_->GetRasterBand(++band_index)
      ->RasterIO(GF_Read, start_row, start_column, width, height, data,
                 buffer_width, buffer_height, GDT_Float64, 0, 0);

  return data;
}
}
