#ifndef CATALOG_VIEW_H
#define CATALOG_VIEW_H

#include <QTreeView>

QT_BEGIN_NAMESPACE
class QFileSystemModel;
QT_END_NAMESPACE

class catalog_view : public QTreeView {
public:
  explicit catalog_view(QWidget *parent = nullptr);

private:
  QFileSystemModel model_;
};

#endif // CATALOG_VIEW_H
