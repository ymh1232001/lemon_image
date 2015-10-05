#include <QTreeWidgetItem>

#include "layer_base.h"
#include "view_2d.h"

Q_DECLARE_METATYPE(view_2d *)

view_2d *layer_base::view() {
  return layer_item_->parent()->data(0, 1).value<view_2d *>();
}
