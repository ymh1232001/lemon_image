#ifndef CHANGE_RGB_BANDS_DIALOG_H
#define CHANGE_RGB_BANDS_DIALOG_H

#include <QDialog>

namespace lemon {
class raster_metadata;
}

class change_rgb_bands_dialog : public QDialog {
  Q_OBJECT

public:
  change_rgb_bands_dialog(const lemon::raster_metadata *metadata,
                          int red_band_index, int green_band_index,
                          int blue_band_index, QWidget *parent = nullptr);

signals:
  void closed();
  void red_band_changed(int index);
  void green_band_changed(int index);
  void blue_band_changed(int index);

protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // CHANGE_RGB_BANDS_DIALOG_H
