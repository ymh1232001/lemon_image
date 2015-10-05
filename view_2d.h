#ifndef VIEW_2D_H
#define VIEW_2D_H

#include <memory>
#include <QGraphicsView>

#include "layer_base.h"
#include "scene_2d.h"
#include "utility.h"

QT_BEGIN_NAMESPACE
class QAction;
class QTreeWidgetItem;
QT_END_NAMESPACE

class scene_2d;
class OGRSpatialReference;

class view_2d : public QGraphicsView {
  Q_OBJECT

public:
  view_2d();
  ~view_2d();

  virtual void mouseMoveEvent(QMouseEvent *event) override;
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void wheelEvent(QWheelEvent *event);
  virtual void resizeEvent(QResizeEvent *event) override;

  inline void set_view_item(QTreeWidgetItem *item) { view_item_ = item; }
  inline QTreeWidgetItem *view_item() { return view_item_; }
  inline std::vector<std::unique_ptr<layer_base>> *layers() { return &layers_; }

  void reset();
  void redraw();
  void create_actions();
  void show_context_menu();
  void change_background_color();

  void add_raster_layers();
  void add_vector_layers();
  void remove_layer(int index);

  QString gen_cs_name(const lemon::metadata *metadata);

signals:
  void coordinate_changed(const QString &text);

private:
  bool mid_button_pressed;
  double pixel_width_;
  double pixel_height_;
  double zoom_multiples_;
  lemon::point_2d<double> top_left_;

  QString cs_name_;
  QPoint prev_position_;
  QPoint original_mouse_pos_;
  QTreeWidgetItem *view_item_;
  QAction *change_background_color_action_;

  const OGRSpatialReference *spatial_ref_;

  scene_2d scene_;

  std::vector<std::unique_ptr<layer_base>> layers_;
};

#endif // VIEW_2D_H
