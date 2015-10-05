#ifndef LAYER_BASE_H
#define LAYER_BASE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
class QGraphicsScene;
QT_END_NAMESPACE

class view_2d;

class layer_base : public QObject {
public:
  virtual ~layer_base(){};
  virtual void draw(QGraphicsScene &scene, double left, double top,
                    double pixel_width, double pixel_height) = 0;
  virtual bool is_raster_layer() const = 0;

  inline bool visible() const { return visible_; }
  inline void set_visible(bool visible) { visible_ = visible; }
  inline void set_layer_item(QTreeWidgetItem *layer_item) {
    layer_item_ = layer_item;
  }

  view_2d *view();

protected:
  bool visible_ = true;
  QTreeWidgetItem *layer_item_ = nullptr;
};

#endif // LAYER_BASE_H
