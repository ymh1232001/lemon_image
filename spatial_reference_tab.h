#ifndef SPATIAL_REFERENCE_TAB_H
#define SPATIAL_REFERENCE_TAB_H

#include <QWidget>

namespace lemon {
struct metadata;
struct projected_coordinate_system;
struct geographic_coordinate_system;
struct local_coordinate_system;
struct vertical_coordinate_system;
}

class spatial_reference_tab : public QWidget {
public:
  spatial_reference_tab(const lemon::metadata *metadata,
                        QWidget *parent = nullptr);
};

#endif // SPATIAL_REFERENCE_TAB_H
