#ifndef GENERAL_TAB_H
#define GENERAL_TAB_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
QT_END_NAMESPACE

namespace lemon {
struct metadata;
}

class general_tab : public QWidget {
  Q_OBJECT

public:
  general_tab(const lemon::metadata *metadata, bool is_raster,
              QWidget *parent = nullptr);

private:
  void change_file(int index);
  void change_band(int index);

  QLabel *file_size_;
  QLabel *num_features_;
  QLabel *geometry_type_;
  QLabel *extent_x_min_;
  QLabel *extent_x_max_;
  QLabel *extent_y_min_;
  QLabel *extent_y_max_;
  QLabel *block_width_;
  QLabel *block_height_;
  QLabel *min_;
  QLabel *max_;
  QLabel *mean_;
  QLabel *std_dev_;
  QLabel *no_data_value_;
  QLabel *description_;
  QLabel *color_interpretation_;
  QLabel *unit_;
  QLabel *num_overviews_;
  QLabel *overview_resample_method_;

  QComboBox *select_file_;
  QComboBox *select_bands_;
  QComboBox *select_layers_;

  const lemon::metadata *metadata_;
  std::vector<QString> file_size_string_;
};

#endif // GENERAL_TAB_H
