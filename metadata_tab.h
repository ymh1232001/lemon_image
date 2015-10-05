#ifndef METADATA_TAB_H
#define METADATA_TAB_H

#include <QWidget>

namespace lemon {
class metadata_domain;
}

class metadata_tab : public QWidget {
public:
  metadata_tab(const lemon::metadata_domain *metadata,
               QWidget *parent = nullptr);
};

#endif // METADATA_TAB_H
