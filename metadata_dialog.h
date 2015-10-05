#ifndef METADATA_DIALOG_H
#define METADATA_DIALOG_H

#include <memory>
#include <QDialog>

#include "metadata.h"

QT_BEGIN_NAMESPACE
class QTabWidget;
QT_END_NAMESPACE

class layer_base;

class metadata_dialog : public QDialog {
public:
  metadata_dialog(const layer_base *layer, QWidget *parent = nullptr);

private:
  QTabWidget *tab_widget_;

  std::unique_ptr<lemon::metadata> metadata_;
};

#endif // METADATA_DIALOG_H
