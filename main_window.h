#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <list>
#include <vector>
#include <memory>

#include <QMainWindow>

#include "layer_base.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QLabel;
class QMdiArea;
class QTreeWidget;
class QTreeWidgetItem;
class QTreeView;
class QTableWidget;
QT_END_NAMESPACE

namespace lemon {
class vector;
}

class view_2d;
class catalog_view;

enum class view_mode { SELECT, ZOOM_IN, ZOOM_OUT, PAN };

class main_window : public QMainWindow {
  Q_OBJECT

public:
  main_window();
  ~main_window();

protected:
  virtual void closeEvent(QCloseEvent *event) override;

private:
  void read_settings();
  void write_settings();

  void create_actions();
  void create_menus();
  void create_toolbars();
  void create_statusbar();
  void create_view();
  void create_dock_windows();

  void open();
  void save();
  void exit();
  void select();
  void pan();
  void zoom_in();
  void zoom_out();
  void add_raster_layers();
  void add_vector_layers();
  void about();

  void remove_layer();
  void show_metadata_dialog();
  void change_rgb_bands();
  void change_style();
  void show_attribute_table();

  view_2d *active_view();

  void item_clicked(QTreeWidgetItem *item, int column);
  void show_item_context_menu(const QPoint &pos);

  QAction *new_action_;
  QAction *open_action_;
  QAction *save_action_;
  QAction *quit_action_;
  QAction *select_action_;
  QAction *pan_action_;
  QAction *zoom_in_action_;
  QAction *zoom_out_action_;
  QAction *add_raster_layers_action_;
  QAction *add_vector_layers_action_;
  QAction *cascade_action_;
  QAction *tile_action_;
  QAction *about_action_;
  QAction *remove_layer_action_;
  QAction *show_metadata_dialog_action_;
  QAction *change_rgb_bands_action_;
  QAction *change_style_action_;
  QAction *show_attribute_table_action_;
  QMenu *file_menu_;
  QMenu *view_menu_;
  QMenu *layer_menu_;
  QMenu *window_menu_;
  QMenu *help_menu_;
  QLabel *coordinate_label_;
  QToolBar *file_toolbar_;
  QToolBar *view_toolbar_;
  QToolBar *layer_toolbar_;
  QTreeWidget *layer_tree_;
  QTreeWidget *toolbox_tree_;
  QTreeWidgetItem *current_item_;
  QDockWidget *layer_dock_;
  QDockWidget *catalog_dock_;
  QDockWidget *toolbox_dock_;
  QDockWidget *attribute_table_dock_;
  QTableWidget *attribute_table_;

  QMdiArea *mdi_area_;

  catalog_view *catalog_tree_;

  lemon::vector *previous_vector_;

  view_mode view_mode_;
};

#endif // MAIN_WINDOW_H
