#include <QtWidgets>

#include "general_tab.h"
#include "raster.h"
#include "vector.h"
#include "utility.h"

general_tab::general_tab(const lemon::metadata *metadata, bool is_raster,
                         QWidget *parent)
    : QWidget(parent), metadata_(metadata) {
  QGroupBox *file_info_group = new QGroupBox(tr("文件信息"));

  QLabel *pathname_label = new QLabel(tr("路径"));
  QLabel *pathname = new QLabel(lemon::to_qstring(metadata->pathname));

  QHBoxLayout *pathname_layout = new QHBoxLayout();
  pathname_layout->addWidget(pathname_label);
  pathname_layout->addWidget(pathname);
  pathname_layout->setSpacing(25);

  QString total_size =
      lemon::to_qstring(lemon::gen_file_size_string(metadata->total_size));

  QLabel *file_label = new QLabel(tr("文件"));
  QLabel *file_size_label = new QLabel(tr("大小"));
  file_size_ = new QLabel(total_size);

  select_file_ = new QComboBox();
  select_file_->addItem(tr("全部文件"));

  file_size_string_.push_back(total_size);

  std::size_t size = metadata->file_list.size();
  for (int i = 0; i < size; ++i) {
    select_file_->addItem(lemon::to_qstring(metadata->file_list[i]));
    file_size_string_.push_back(
        lemon::to_qstring(lemon::gen_file_size_string(metadata->file_size[i])));
  }

  connect(select_file_, static_cast<void (QComboBox::*)(int)>(
                            &QComboBox::currentIndexChanged),
          this, &general_tab::change_file);

  QHBoxLayout *file_layout = new QHBoxLayout();
  file_layout->addWidget(file_label);
  file_layout->addWidget(select_file_);
  file_layout->setSpacing(25);

  QHBoxLayout *file_size_layout = new QHBoxLayout();
  file_size_layout->addWidget(file_size_label);
  file_size_layout->addWidget(file_size_);
  file_size_layout->setSpacing(25);

  QHBoxLayout *file_size_main_layout = new QHBoxLayout();
  file_size_main_layout->addLayout(file_layout);
  file_layout->addStretch();
  file_size_main_layout->addLayout(file_size_layout);

  QVBoxLayout *file_info_layout = new QVBoxLayout();
  file_info_layout->addLayout(pathname_layout);
  file_info_layout->addLayout(file_size_main_layout);

  file_info_group->setLayout(file_info_layout);

  QLabel *data_format_label = new QLabel(tr("数据格式"));
  QLabel *data_format = new QLabel(lemon::to_qstring(metadata->data_format));

  QHBoxLayout *data_format_layout = new QHBoxLayout();
  data_format_layout->addWidget(data_format_label);
  data_format_layout->addWidget(data_format);
  data_format_layout->setSpacing(25);

  QVBoxLayout *main_layout = new QVBoxLayout();

  if (is_raster) {
    const lemon::raster_metadata *raster_metadata =
        static_cast<const lemon::raster_metadata *>(metadata);

    QLabel *select_bands_label = new QLabel(tr("当前波段"));

    select_bands_ = new QComboBox();

    std::vector<QString> band_list = lemon::gen_band_list(raster_metadata);

    for (auto &e : band_list)
      select_bands_->addItem(e);

    connect(select_bands_, static_cast<void (QComboBox::*)(int)>(
                               &QComboBox::currentIndexChanged),
            this, &general_tab::change_band);

    QHBoxLayout *select_bands_layout = new QHBoxLayout();
    select_bands_layout->addWidget(select_bands_label);
    select_bands_layout->addWidget(select_bands_);

    QLabel *num_bands_label = new QLabel(tr("波段数"));
    QLabel *num_bands = new QLabel(QString::number(raster_metadata->num_bands));

    QHBoxLayout *num_bands_layout = new QHBoxLayout();
    num_bands_layout->addWidget(num_bands_label);
    num_bands_layout->addWidget(num_bands);
    num_bands_layout->setSpacing(25);

    QHBoxLayout *num_bands_and_data_format_layout = new QHBoxLayout();
    num_bands_and_data_format_layout->addLayout(num_bands_layout);
    num_bands_and_data_format_layout->addStretch();
    num_bands_and_data_format_layout->addLayout(data_format_layout);

    file_info_layout->addLayout(num_bands_and_data_format_layout);

    main_layout->addLayout(select_bands_layout);
    main_layout->addWidget(file_info_group);

    QGroupBox *band_info_group = new QGroupBox(tr("波段信息"));

    QLabel *width_label = new QLabel(tr("宽"));
    QLabel *width = new QLabel(QString::number(raster_metadata->width));

    QHBoxLayout *width_layout = new QHBoxLayout();
    width_layout->addWidget(width_label);
    width_layout->addWidget(width);
    width_layout->setSpacing(25);

    QLabel *height_label = new QLabel(tr("高"));
    QLabel *height = new QLabel(QString::number(raster_metadata->height));

    QHBoxLayout *height_layout = new QHBoxLayout();
    height_layout->addWidget(height_label);
    height_layout->addWidget(height);
    height_layout->setSpacing(25);

    QHBoxLayout *size_layout = new QHBoxLayout();
    size_layout->addLayout(width_layout);
    size_layout->addStretch();
    size_layout->addLayout(height_layout);

    QLabel *block_width_label = new QLabel(tr("分块宽"));
    block_width_ =
        new QLabel(QString::number(raster_metadata->band_info[0].block_width));

    QHBoxLayout *block_width_layout = new QHBoxLayout();
    block_width_layout->addWidget(block_width_label);
    block_width_layout->addWidget(block_width_);
    block_width_layout->setSpacing(25);

    QLabel *block_height_label = new QLabel(tr("分块高"));
    block_height_ =
        new QLabel(QString::number(raster_metadata->band_info[0].block_height));

    QHBoxLayout *block_height_layout = new QHBoxLayout();
    block_height_layout->addWidget(block_height_label);
    block_height_layout->addWidget(block_height_);
    block_height_layout->setSpacing(25);

    QHBoxLayout *block_size_layout = new QHBoxLayout();
    block_size_layout->addLayout(block_width_layout);
    block_size_layout->addStretch();
    block_size_layout->addLayout(block_height_layout);

    QLabel *data_type_label = new QLabel(tr("数据类型"));
    QLabel *data_type =
        new QLabel(lemon::to_qstring(raster_metadata->data_type));

    QHBoxLayout *data_type_layout = new QHBoxLayout();
    data_type_layout->addWidget(data_type_label);
    data_type_layout->addWidget(data_type);
    data_type_layout->setSpacing(25);

    QLabel *no_data_value_label = new QLabel(tr("无效值"));

    if (!raster_metadata->band_info[0].have_no_data_value)
      no_data_value_ = new QLabel(tr("None"));
    else if (raster_metadata->band_info[0].no_data_value_is_nan)
      no_data_value_ = new QLabel(tr("NaN"));
    else
      no_data_value_ = new QLabel(QString::number(
          raster_metadata->band_info[0].no_data_value, 'g', 18));

    QHBoxLayout *no_data_value_layout = new QHBoxLayout();
    no_data_value_layout->addWidget(no_data_value_label);
    no_data_value_layout->addWidget(no_data_value_);
    no_data_value_layout->setSpacing(25);

    QHBoxLayout *data_type_and_no_data_value_layout = new QHBoxLayout();
    data_type_and_no_data_value_layout->addLayout(data_type_layout);
    data_type_and_no_data_value_layout->addStretch();
    data_type_and_no_data_value_layout->addLayout(no_data_value_layout);

    QLabel *data_order_label = new QLabel(tr("存储方式"));
    QLabel *data_order =
        new QLabel(lemon::to_qstring(raster_metadata->data_order));

    QHBoxLayout *data_order_layout = new QHBoxLayout();
    data_order_layout->addWidget(data_order_label);
    data_order_layout->addWidget(data_order);
    data_order_layout->setSpacing(25);

    QLabel *compression_label = new QLabel(tr("压缩方式"));
    QLabel *compression =
        new QLabel(lemon::to_qstring(raster_metadata->compression));

    QHBoxLayout *compression_layout = new QHBoxLayout();
    compression_layout->addWidget(compression_label);
    compression_layout->addWidget(compression);
    compression_layout->setSpacing(25);

    QHBoxLayout *data_order_and_compression_layout = new QHBoxLayout();
    data_order_and_compression_layout->addLayout(data_order_layout);
    data_order_and_compression_layout->addStretch();
    data_order_and_compression_layout->addLayout(compression_layout);

    QLabel *description_label = new QLabel(tr("描述"));

    description_ = new QLabel(
        lemon::to_qstring(raster_metadata->band_info[0].description));

    QHBoxLayout *description_layout = new QHBoxLayout();
    description_layout->addWidget(description_label);
    description_layout->addWidget(description_);
    description_layout->setSpacing(25);

    QLabel *color_interpretation_label = new QLabel(tr("颜色类型"));

    color_interpretation_ = new QLabel(
        lemon::to_qstring(raster_metadata->band_info[0].color_interpretation));

    QHBoxLayout *color_interpretation_layout = new QHBoxLayout();
    color_interpretation_layout->addWidget(color_interpretation_label);
    color_interpretation_layout->addWidget(color_interpretation_);
    color_interpretation_layout->setSpacing(25);

    QHBoxLayout *description_and_color_interpretation_layout =
        new QHBoxLayout();
    description_and_color_interpretation_layout->addLayout(description_layout);
    description_and_color_interpretation_layout->addStretch();
    description_and_color_interpretation_layout->addLayout(
        color_interpretation_layout);

    QLabel *num_overviews_label = new QLabel(tr("金字塔层数"));

    num_overviews_ = new QLabel(
        QString::number(raster_metadata->band_info[0].num_overviews));

    QHBoxLayout *num_overviews_layout = new QHBoxLayout();
    num_overviews_layout->addWidget(num_overviews_label);
    num_overviews_layout->addWidget(num_overviews_);
    num_overviews_layout->setSpacing(25);

    QLabel *overview_resample_method_label = new QLabel(tr("金字塔采样方式"));

    overview_resample_method_ = new QLabel(lemon::to_qstring(
        raster_metadata->band_info[0].overview_resample_method));

    QHBoxLayout *overview_resample_method_layout = new QHBoxLayout();
    overview_resample_method_layout->addWidget(overview_resample_method_label);
    overview_resample_method_layout->addWidget(overview_resample_method_);
    overview_resample_method_layout->setSpacing(25);

    QHBoxLayout *overview_layout = new QHBoxLayout();
    overview_layout->addLayout(num_overviews_layout);
    overview_layout->addStretch();
    overview_layout->addLayout(overview_resample_method_layout);

    QVBoxLayout *band_info_layout = new QVBoxLayout();
    band_info_layout->addLayout(size_layout);
    band_info_layout->addLayout(block_size_layout);
    band_info_layout->addLayout(data_type_and_no_data_value_layout);
    band_info_layout->addLayout(data_order_and_compression_layout);
    band_info_layout->addLayout(description_and_color_interpretation_layout);
    band_info_layout->addLayout(overview_layout);

    band_info_group->setLayout(band_info_layout);

    QGroupBox *statistics_info_group = new QGroupBox(tr("统计信息"));

    QLabel *min_label = new QLabel(tr("最小值"));

    min_ =
        new QLabel(QString::number(raster_metadata->band_info[0].min, 'f', 3));

    QHBoxLayout *min_layout = new QHBoxLayout();
    min_layout->addWidget(min_label);
    min_layout->addWidget(min_);
    min_layout->setSpacing(25);

    QLabel *max_label = new QLabel(tr("最大值"));

    max_ =
        new QLabel(QString::number(raster_metadata->band_info[0].max, 'f', 3));

    QHBoxLayout *max_layout = new QHBoxLayout();
    max_layout->addWidget(max_label);
    max_layout->addWidget(max_);
    max_layout->setSpacing(25);

    QHBoxLayout *min_max_layout = new QHBoxLayout();
    min_max_layout->addLayout(min_layout);
    min_max_layout->addStretch();
    min_max_layout->addLayout(max_layout);

    QLabel *mean_label = new QLabel(tr("平均值"));

    mean_ =
        new QLabel(QString::number(raster_metadata->band_info[0].mean, 'f', 3));

    QHBoxLayout *mean_layout = new QHBoxLayout();
    mean_layout->addWidget(mean_label);
    mean_layout->addWidget(mean_);
    mean_layout->setSpacing(25);

    QLabel *std_dev_label = new QLabel(tr("标准差"));

    std_dev_ = new QLabel(
        QString::number(raster_metadata->band_info[0].std_dev, 'f', 3));

    QHBoxLayout *std_dev_layout = new QHBoxLayout();
    std_dev_layout->addWidget(std_dev_label);
    std_dev_layout->addWidget(std_dev_);
    std_dev_layout->setSpacing(25);

    QHBoxLayout *mean_and_std_dev_layout = new QHBoxLayout();
    mean_and_std_dev_layout->addLayout(mean_layout);
    mean_and_std_dev_layout->addStretch();
    mean_and_std_dev_layout->addLayout(std_dev_layout);

    QVBoxLayout *statistics_info_layout = new QVBoxLayout();
    statistics_info_layout->addLayout(min_max_layout);
    statistics_info_layout->addLayout(mean_and_std_dev_layout);

    statistics_info_group->setLayout(statistics_info_layout);

    QGroupBox *map_info_group = new QGroupBox(tr("地图信息"));

    QLabel *left_x_label = new QLabel(tr("左上角横坐标"));
    QLabel *left_x =
        new QLabel(QString::number(raster_metadata->left, 'g', 16));

    QHBoxLayout *left_x_layout = new QHBoxLayout();
    left_x_layout->addWidget(left_x_label);
    left_x_layout->addWidget(left_x);
    left_x_layout->setSpacing(25);

    QLabel *top_y_label = new QLabel(tr("左上角纵坐标"));
    QLabel *top_y = new QLabel(QString::number(raster_metadata->top, 'g', 16));

    QHBoxLayout *top_y_layout = new QHBoxLayout();
    top_y_layout->addWidget(top_y_label);
    top_y_layout->addWidget(top_y);
    top_y_layout->setSpacing(25);

    QHBoxLayout *top_left_layout = new QHBoxLayout();
    top_left_layout->addLayout(left_x_layout);
    top_left_layout->addStretch();
    top_left_layout->addLayout(top_y_layout);

    QLabel *pixel_width_label = new QLabel(tr("像元宽"));
    QLabel *pixel_width =
        new QLabel(QString::number(raster_metadata->pixel_width, 'g', 15));

    QHBoxLayout *pixel_width_layout = new QHBoxLayout();
    pixel_width_layout->addWidget(pixel_width_label);
    pixel_width_layout->addWidget(pixel_width);
    pixel_width_layout->setSpacing(25);

    QLabel *pixel_height_label = new QLabel(tr("像元高"));
    QLabel *pixel_height =
        new QLabel(QString::number(raster_metadata->pixel_height, 'g', 15));

    QHBoxLayout *pixel_height_layout = new QHBoxLayout();
    pixel_height_layout->addWidget(pixel_height_label);
    pixel_height_layout->addWidget(pixel_height);
    pixel_height_layout->setSpacing(25);

    QHBoxLayout *pixel_layout = new QHBoxLayout();
    pixel_layout->addLayout(pixel_width_layout);
    pixel_layout->addStretch();
    pixel_layout->addLayout(pixel_height_layout);

    QVBoxLayout *map_info_layout = new QVBoxLayout();
    map_info_layout->addLayout(top_left_layout);
    map_info_layout->addLayout(pixel_layout);

    if (!raster_metadata->unit.empty()) {
      QLabel *unit_label = new QLabel(tr("单位"));

      QLabel *unit = new QLabel(lemon::to_qstring(raster_metadata->unit));

      QHBoxLayout *unit_layout = new QHBoxLayout();
      unit_layout->addWidget(unit_label);
      unit_layout->addWidget(unit);
      unit_layout->setSpacing(25);
      unit_layout->addStretch();

      map_info_layout->addLayout(unit_layout);
    }

    map_info_group->setLayout(map_info_layout);

    main_layout->addWidget(band_info_group);
    main_layout->addWidget(statistics_info_group);
    main_layout->addWidget(map_info_group);
  } else {
    const lemon::vector_metadata *vector_metadata =
        static_cast<const lemon::vector_metadata *>(metadata);

    QLabel *num_layers_label = new QLabel(tr("图层数"));
    QLabel *num_layers =
        new QLabel(QString::number(vector_metadata->num_layers));

    QHBoxLayout *num_layers_layout = new QHBoxLayout();
    num_layers_layout->addWidget(num_layers_label);
    num_layers_layout->addWidget(num_layers);
    num_layers_layout->setSpacing(25);

    QHBoxLayout *num_layers_and_data_format_layout = new QHBoxLayout();
    num_layers_and_data_format_layout->addLayout(num_layers_layout);
    num_layers_and_data_format_layout->addStretch();
    num_layers_and_data_format_layout->addLayout(data_format_layout);

    file_info_layout->addLayout(num_layers_and_data_format_layout);

    main_layout->addWidget(file_info_group);

    QGroupBox *layer_info_group = new QGroupBox(tr("图层信息"));

    QLabel *select_layers_label = new QLabel(tr("当前图层"));

    select_layers_ = new QComboBox();

    int n = vector_metadata->num_layers;
    for (int i = 0; i < n; ++i)
      select_layers_->addItem(
          lemon::to_qstring(vector_metadata->layer_name[i]));

    QHBoxLayout *select_layers_layout = new QHBoxLayout();
    select_layers_layout->addWidget(select_layers_label);
    select_layers_layout->addWidget(select_layers_);

    QLabel *num_features_label = new QLabel(tr("要素数"));

    num_features_ =
        new QLabel(QString::number(vector_metadata->num_features[0]));

    QHBoxLayout *num_features_layout = new QHBoxLayout();
    num_features_layout->addWidget(num_features_label);
    num_features_layout->addWidget(num_features_);
    num_features_layout->setSpacing(25);

    QLabel *geometry_type_label = new QLabel(tr("几何类型"));

    geometry_type_ =
        new QLabel(lemon::to_qstring(vector_metadata->geometry_type[0]));

    QHBoxLayout *geometry_type_layout = new QHBoxLayout();
    geometry_type_layout->addWidget(geometry_type_label);
    geometry_type_layout->addWidget(geometry_type_);
    geometry_type_layout->setSpacing(25);

    QHBoxLayout *feature_info_layout = new QHBoxLayout();
    feature_info_layout->addLayout(num_features_layout);
    feature_info_layout->addStretch();
    feature_info_layout->addLayout(geometry_type_layout);

    QVBoxLayout *layer_info_layout = new QVBoxLayout();
    layer_info_layout->addLayout(select_layers_layout);

    layer_info_group->setLayout(layer_info_layout);

    QGroupBox *extent_group = new QGroupBox(tr("范围"));

    extent_x_min_ =
        new QLabel(QString::number(vector_metadata->extents[0].MinX, 'f'));

    extent_x_max_ =
        new QLabel(QString::number(vector_metadata->extents[0].MaxX, 'f'));

    extent_y_min_ =
        new QLabel(QString::number(vector_metadata->extents[0].MinY, 'f'));

    extent_y_max_ =
        new QLabel(QString::number(vector_metadata->extents[0].MaxY, 'f'));

    QLabel *extent_x_min_label = new QLabel(tr("横坐标最小值"));

    QLabel *extent_x_max_label = new QLabel(tr("横坐标最大值"));

    QLabel *extent_y_min_label = new QLabel(tr("纵坐标最小值"));

    QLabel *extent_y_max_label = new QLabel(tr("纵坐标最大值"));

    QHBoxLayout *extent_x_min_layout = new QHBoxLayout();
    extent_x_min_layout->addWidget(extent_x_min_label);
    extent_x_min_layout->addWidget(extent_x_min_);
    extent_x_min_layout->setSpacing(25);

    QHBoxLayout *extent_x_max_layout = new QHBoxLayout();
    extent_x_max_layout->addWidget(extent_x_max_label);
    extent_x_max_layout->addWidget(extent_x_max_);
    extent_x_max_layout->setSpacing(25);

    QHBoxLayout *extent_x_layout = new QHBoxLayout();
    extent_x_layout->addLayout(extent_x_min_layout);
    extent_x_layout->addStretch();
    extent_x_layout->addLayout(extent_x_max_layout);

    QHBoxLayout *extent_y_min_layout = new QHBoxLayout();
    extent_y_min_layout->addWidget(extent_y_min_label);
    extent_y_min_layout->addWidget(extent_y_min_);
    extent_y_min_layout->setSpacing(25);

    QHBoxLayout *extent_y_max_layout = new QHBoxLayout();
    extent_y_max_layout->addWidget(extent_y_max_label);
    extent_y_max_layout->addWidget(extent_y_max_);
    extent_y_max_layout->setSpacing(25);

    QHBoxLayout *extent_y_layout = new QHBoxLayout();
    extent_y_layout->addLayout(extent_y_min_layout);
    extent_y_layout->addStretch();
    extent_y_layout->addLayout(extent_y_max_layout);

    QVBoxLayout *extent_layout = new QVBoxLayout();
    extent_layout->addLayout(extent_x_layout);
    extent_layout->addLayout(extent_y_layout);

    extent_group->setLayout(extent_layout);

    main_layout->addWidget(layer_info_group);
    main_layout->addWidget(extent_group);
  }

  setLayout(main_layout);
}

void general_tab::change_file(int index) {
  file_size_->setText(file_size_string_[index]);
}

void general_tab::change_band(int index) {
  const lemon::raster_metadata *raster_metadata =
      static_cast<const lemon::raster_metadata *>(metadata_);

  const lemon::raster_band_info *band_info = &raster_metadata->band_info[index];

  if (!band_info->have_no_data_value)
    no_data_value_->setText(tr("None"));
  else if (band_info->no_data_value_is_nan)
    no_data_value_->setText(tr("NaN"));
  else
    no_data_value_->setText(QString::number(band_info->no_data_value, 'g', 18));

  min_->setText(QString::number(band_info->min, 'f', 3));
  max_->setText(QString::number(band_info->max, 'f', 3));
  mean_->setText(QString::number(band_info->mean, 'f', 3));
  std_dev_->setText(QString::number(band_info->std_dev, 'f', 3));
  block_width_->setText(QString::number(band_info->block_width));
  block_height_->setText(QString::number(band_info->block_height));
  description_->setText(lemon::to_qstring(band_info->description));
  color_interpretation_->setText(
      lemon::to_qstring(band_info->color_interpretation));
  num_overviews_->setText(QString::number(band_info->num_overviews));
  overview_resample_method_->setText(
      lemon::to_qstring(band_info->overview_resample_method));
}
