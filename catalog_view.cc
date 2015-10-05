#include <QHeaderView>
#include <QFileSystemModel>

#include "catalog_view.h"

catalog_view::catalog_view(QWidget *parent) : QTreeView(parent) {
  model_.setRootPath("");

  setModel(&model_);

  header()->setHidden(true);
  header()->setSectionHidden(1, true);
  header()->setSectionHidden(2, true);
  header()->setSectionHidden(3, true);

  setRootIndex(model_.index(QDir::homePath()));
}
