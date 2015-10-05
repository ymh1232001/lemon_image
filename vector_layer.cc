#include <QTreeWidgetItem>

#include "vector_layer.h"
#include "vector.h"

vector_layer::~vector_layer() {
  if (layer_item_ != nullptr)
      layer_item_->parent()->removeChild(layer_item_);
}

void vector_layer::draw(QGraphicsScene &scene, double left, double top, double pixel_width, double pixel_height)
{

}

void vector_layer::set_vector(lemon::vector *vector) { vector_.reset(vector); }
