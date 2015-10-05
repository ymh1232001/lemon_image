#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTreeWidgetItem>

#include "raster_layer.h"
#include "algorithm.h"
#include "raster.h"
#include "utility.h"
#include "view_2d.h"
#include "change_rgb_bands_dialog.h"

raster_layer::~raster_layer() {
  if (layer_item_ != nullptr)
    layer_item_->parent()->removeChild(layer_item_);

  if (change_rgb_bands_dialog_ != nullptr)
    change_rgb_bands_dialog_->close();
}

void raster_layer::draw(QGraphicsScene &scene, double left, double top,
                        double pixel_width, double pixel_height) {
  int scene_width = static_cast<int>(scene.width());
  int scene_height = static_cast<int>(scene.height());

  double right = left + pixel_width * scene_width;
  double bottom = top + pixel_height * scene_height;

  const lemon::raster_metadata *metadata = raster_->metadata();

  if (left >= metadata->right || right <= metadata->left ||
      top >= metadata->bottom || bottom <= metadata->top)
    return;

  int x_offset = 0;
  int y_offset = 0;
  int read_x = 0;
  int read_y = 0;
  int read_width;
  int read_height;

  if (left <= metadata->left) {
    read_width = (right - metadata->left) / metadata->pixel_width;

    x_offset = scene_width - read_width;

    if (read_width > metadata->width)
      read_width = metadata->width;
  } else {
    read_x = static_cast<int>((left - metadata->left) / metadata->pixel_width);
    read_width = (right - left) / metadata->pixel_width;

    if ((read_x + read_width) > metadata->width)
      read_width = metadata->width - read_x;
  }

  if (top <= metadata->top) {
    read_height = (bottom - metadata->top) / std::abs(metadata->pixel_height);

    y_offset = scene_height - read_height;

    if (read_height > metadata->height)
      read_height = metadata->height;
  } else {
    read_y = static_cast<int>((top - metadata->top) /
                              std::abs(metadata->pixel_height));
    read_height = (bottom - top) / std::abs(metadata->pixel_height);

    if ((read_y + read_height) > metadata->height)
      read_height = metadata->height - read_y;
  }

  // double型数据可能有问题，暂不支持复数数据
  if (metadata->num_bands >= 3) {
    std::unique_ptr<float[]> data_r(raster_->read_block<float>(
        red_band_index_, read_x, read_y, read_width, read_height));
    std::unique_ptr<float[]> data_g(raster_->read_block<float>(
        green_band_index_, read_x, read_y, read_width, read_height));
    std::unique_ptr<float[]> data_b(raster_->read_block<float>(
        blue_band_index_, read_x, read_y, read_width, read_height));

    std::unique_ptr<std::uint8_t[]> image_data(lemon::build_image_data(
        data_r.get(), data_g.get(), data_b.get(), read_width, read_height,
        stretch_min_max_[red_band_index_].first,
        stretch_min_max_[red_band_index_].second,
        stretch_min_max_[green_band_index_].first,
        stretch_min_max_[green_band_index_].second,
        stretch_min_max_[blue_band_index_].first,
        stretch_min_max_[blue_band_index_].second));

    QImage image(image_data.get(), read_width, read_height,
                 (read_width * 24 + 31) / 8, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(std::move(image));

    QGraphicsPixmapItem *pixmap_item = scene.addPixmap(pixmap);
    pixmap_item->setPos(x_offset, y_offset);
  } else { // 暂未考虑双波段的情况
    std::unique_ptr<float[]> data(
        raster_->read_block<float>(0, read_x, read_y, read_width, read_height));

    std::unique_ptr<std::uint8_t[]> image_data(lemon::build_image_data(
        !color_table_.empty(), data.get(), read_width, read_height,
        stretch_min_max_[0].first, stretch_min_max_[0].second));

    QImage image(image_data.get(), read_width, read_height,
                 (read_width * 8 + 31) / 8, QImage::Format_Indexed8);

    if (!color_table_.empty())
      image.setColorTable(color_table_);

    QPixmap pixmap = QPixmap::fromImage(std::move(image));

    QGraphicsPixmapItem *pixmap_item = scene.addPixmap(pixmap);
    pixmap_item->setPos(x_offset, y_offset);
  }
}

void raster_layer::set_raster(lemon::raster *raster) {
  stretch_min_max_.clear();
  stretch_min_max_.shrink_to_fit();

  const std::vector<lemon::raster_band_info> *band_info =
      &raster->metadata()->band_info;

  int num_bands = raster->metadata()->num_bands;
  if (num_bands >= 3) {
    stretch_min_max_.resize(num_bands);
    for (int i = 0; i < num_bands; ++i)
      stretch_min_max_[i] = std::make_pair(
          band_info->at(i).mean - 2.0 * band_info->at(i).std_dev,
          band_info->at(i).mean + 2.0 * band_info->at(i).std_dev);
  } else { // TODO : 增加对双波段图像的处理
    stretch_min_max_.resize(1);
    stretch_min_max_[0] =
        std::make_pair(band_info->at(0).mean - 2.0 * band_info->at(0).std_dev,
                       band_info->at(0).mean + 2.0 * band_info->at(0).std_dev);

    color_table_ = std::move(lemon::to_qt_color_table(raster->color_table()));
  }

  raster_.reset(raster);
}

void raster_layer::set_red_band_index(int index) {
  if (index != red_band_index_) {
    red_band_index_ = index;
    view()->redraw();
  }
}

void raster_layer::set_green_band_index(int index) {
  if (index != green_band_index_) {
    green_band_index_ = index;
    view()->redraw();
  }
}

void raster_layer::set_blue_band_index(int index) {
  if (index != blue_band_index_) {
    blue_band_index_ = index;
    view()->redraw();
  }
}

void raster_layer::show_change_rgb_bands_dialog() {
  change_rgb_bands_dialog_->showNormal();
  change_rgb_bands_dialog_->setWindowState(Qt::WindowActive);
}
