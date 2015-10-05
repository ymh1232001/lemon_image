#ifndef VECTOR_LAYER_H
#define VECTOR_LAYER_H

#include <memory>

#include "vector.h"
#include "layer_base.h"

class vector_layer : public layer_base {
public:
  ~vector_layer();

  virtual void draw(QGraphicsScene &scene, double left, double top,
                    double pixel_width, double pixel_height) override;

  inline virtual bool is_raster_layer() const override { return false; }
  inline const lemon::vector *vector() const {
    return static_cast<const lemon::vector *>(vector_.get());
  }

  void set_vector(lemon::vector *vector);

private:
  std::unique_ptr<lemon::vector> vector_;
};

#endif // VECTOR_LAYER_H
