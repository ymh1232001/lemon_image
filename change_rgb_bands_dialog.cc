#include <QtWidgets>

#include "raster.h"
#include "utility.h"
#include "change_rgb_bands_dialog.h"

change_rgb_bands_dialog::change_rgb_bands_dialog(
    const lemon::raster_metadata *metadata, int red_band_index,
    int green_band_index, int blue_band_index, QWidget *parent)
    : QDialog(parent) {
  QVBoxLayout *main_layout = new QVBoxLayout();

  // FIXME : 避免使用固定像素值

  std::vector<QString> band_list = lemon::gen_band_list(metadata);

  QComboBox *select_red_band = new QComboBox();
  connect(select_red_band, static_cast<void (QComboBox::*)(int)>(
                               &QComboBox::currentIndexChanged),
          this, &change_rgb_bands_dialog::red_band_changed);

  QComboBox *select_green_band = new QComboBox();
  connect(select_green_band, static_cast<void (QComboBox::*)(int)>(
                               &QComboBox::currentIndexChanged),
          this, &change_rgb_bands_dialog::green_band_changed);

  QComboBox *select_blue_band = new QComboBox();
  connect(select_blue_band, static_cast<void (QComboBox::*)(int)>(
                               &QComboBox::currentIndexChanged),
          this, &change_rgb_bands_dialog::blue_band_changed);

  for (auto &e : band_list) {
    select_red_band->addItem(e);
    select_green_band->addItem(e);
    select_blue_band->addItem(e);
  }

  select_red_band->setCurrentIndex(red_band_index);
  select_green_band->setCurrentIndex(green_band_index);
  select_blue_band->setCurrentIndex(blue_band_index);

  QIcon red(":/red.svg");
  QLabel *red_band = new QLabel(tr("red"));
  red_band->setPixmap(red.pixmap(QSize(32, 32)));

  QHBoxLayout *red_band_layout = new QHBoxLayout();
  red_band_layout->addWidget(red_band);
  red_band_layout->addWidget(select_red_band);

  QIcon green(":/green.svg");
  QLabel *green_band = new QLabel(tr("green"));
  green_band->setPixmap(green.pixmap(QSize(32, 32)));

  QHBoxLayout *green_band_layout = new QHBoxLayout();
  green_band_layout->addWidget(green_band);
  green_band_layout->addWidget(select_green_band);

  QIcon blue(":/blue.svg");
  QLabel *blue_band = new QLabel();
  blue_band->setPixmap(blue.pixmap(QSize(32, 32)));

  QHBoxLayout *blue_band_layout = new QHBoxLayout();
  blue_band_layout->addWidget(blue_band);
  blue_band_layout->addWidget(select_blue_band);

  main_layout->addLayout(red_band_layout);
  main_layout->addLayout(green_band_layout);
  main_layout->addLayout(blue_band_layout);

  setLayout(main_layout);
  setWindowTitle(tr("改变RGB波段"));
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowIcon(QIcon(":/rgb.svg"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void change_rgb_bands_dialog::closeEvent(QCloseEvent *event) { emit closed(); }
