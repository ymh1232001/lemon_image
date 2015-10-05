#include <QtWidgets>

#include "metadata_dialog.h"
#include "general_tab.h"
#include "spatial_reference_tab.h"
#include "metadata_tab.h"
#include "raster_layer.h"
#include "vector_layer.h"
#include "utility.h"

metadata_dialog::metadata_dialog(const layer_base *layer, QWidget *parent)
    : QDialog(parent) {
  bool is_raster = layer->is_raster_layer();

  tab_widget_ = new QTabWidget();

  lemon::metadata *metadata;

  if (is_raster) {
    lemon::raster_metadata *raster_metadata = new lemon::raster_metadata(
        *static_cast<const raster_layer *>(layer)->raster()->metadata());

    metadata = static_cast<lemon::metadata *>(raster_metadata);

    tab_widget_->addTab(new general_tab(metadata, is_raster),
                        QIcon(":/general.svg"), tr("通用"));

    if (metadata->proj_cs != nullptr || metadata->geog_cs != nullptr ||
        metadata->local_cs != nullptr || metadata->vert_cs != nullptr)
      tab_widget_->addTab(new spatial_reference_tab(metadata),
                          QIcon(":/spatial_reference.svg"), tr("空间参考"));

    if (!raster_metadata->metadata_params.default_domain.empty() ||
        !raster_metadata->metadata_params.image_structure_domain.empty() ||
        !raster_metadata->metadata_params.geolocation_domain.empty() ||
        !raster_metadata->metadata_params.rpc_domain.empty())
      tab_widget_->addTab(new metadata_tab(&raster_metadata->metadata_params),
                          QIcon(":/metadata_dialog.svg"),
                          lemon::to_qstring(raster_metadata->data_format));
  } else {
    lemon::vector_metadata *vector_metadata = new lemon::vector_metadata(
        *static_cast<const vector_layer *>(layer)->vector()->metadata());

    metadata = static_cast<lemon::metadata *>(vector_metadata);

    tab_widget_->addTab(new general_tab(metadata, is_raster),
                        QIcon(":/general.svg"), tr("通用"));

    if (metadata->proj_cs != nullptr || metadata->geog_cs != nullptr ||
        metadata->local_cs != nullptr || metadata->vert_cs != nullptr)
      tab_widget_->addTab(new spatial_reference_tab(metadata),
                          QIcon(":/spatial_reference.svg"), tr("空间参考"));
  }

  metadata_.reset(metadata);

  QHBoxLayout *main_layout = new QHBoxLayout();
  main_layout->addWidget(tab_widget_);

  setLayout(main_layout);
  setWindowTitle(tr("元数据"));
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowIcon(QIcon(":/metadata_dialog.svg"));
}
