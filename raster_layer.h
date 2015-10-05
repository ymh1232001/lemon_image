#ifndef RASTER_LAYER_H
#define RASTER_LAYER_H

#include <memory>
#include <QVector>
#include <QRgb>

#include "raster.h"
#include "layer_base.h"

class change_rgb_bands_dialog;

class raster_layer : public layer_base {
public:
  ~raster_layer();

  virtual void draw(QGraphicsScene &scene, double left, double top,
                    double pixel_width, double pixel_height) override;

  inline virtual bool is_raster_layer() const override { return true; }
  inline const lemon::raster *raster() const {
    return static_cast<const lemon::raster *>(raster_.get());
  }
  inline int red_band_index() const { return red_band_index_; }
  inline int green_band_index() const { return green_band_index_; }
  inline int blue_band_index() const { return blue_band_index_; }
  inline const change_rgb_bands_dialog *change_rgb_bands_dlg() const {
    return change_rgb_bands_dialog_;
  }
  inline void set_change_rgb_bands_dialog(change_rgb_bands_dialog *dialog) {
    change_rgb_bands_dialog_ = dialog;
  }
  inline void change_rgb_bands_dialog_closed() {
    change_rgb_bands_dialog_ = nullptr;
  }

  void set_raster(lemon::raster *raster);
  void set_red_band_index(int index);
  void set_green_band_index(int index);
  void set_blue_band_index(int index);
  void show_change_rgb_bands_dialog();

private:
  int red_band_index_ = 0;
  int green_band_index_ = 1;
  int blue_band_index_ = 2;

  change_rgb_bands_dialog *change_rgb_bands_dialog_ = nullptr;

  QVector<QRgb> color_table_;

  std::vector<std::pair<double, double>> stretch_min_max_;

  std::unique_ptr<lemon::raster> raster_;
};

#endif // RASTER_LAYER_H
