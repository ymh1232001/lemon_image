#include <QtWidgets>

// #ifndef QT_NO_OPENGL
// #include <QOpenGLWidget>
// #endif

#include "view_2d.h"
#include "raster_layer.h"
#include "vector_layer.h"
#include "utility.h"
#include "file_dialog.h"
#include "message_box.h"
#include "spatial_ref.h"

Q_DECLARE_METATYPE(const layer_base *)

view_2d::view_2d() {
  setScene(&scene_);
  setCacheMode(QGraphicsView::CacheNone);                   // ?
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // ?
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &view_2d::customContextMenuRequested, this,
          &view_2d::show_context_menu);

  // #ifndef QT_NO_OPENGL
  // QOpenGLWidget *gl_widget = new QOpenGLWidget(this);
  // gl_widget->setAutoFillBackground(true);
  // setViewport(gl_widget);
  // #endif

  setAttribute(Qt::WA_DeleteOnClose);
  setMouseTracking(true);

  create_actions();

  zoom_multiples_ = 1.0;
  view_item_ = nullptr;
  spatial_ref_ = nullptr;
  mid_button_pressed = false;
}

view_2d::~view_2d() {
  QTreeWidget *tree = view_item_->treeWidget();
  tree->takeTopLevelItem(tree->indexOfTopLevelItem(view_item_));
}

void view_2d::mouseMoveEvent(QMouseEvent *event) {
  if (layers_.empty()) {
    emit coordinate_changed(
        QString("%1.00, %2.00").arg(event->x()).arg(event->y()));
  } else {
    if (QGraphicsView::ScrollHandDrag == dragMode()) {
      qreal x_offset = event->x() - prev_position_.x();
      qreal y_offset = event->y() - prev_position_.y();

      top_left_.offset(-x_offset * pixel_width_, -y_offset * pixel_height_);

      prev_position_ = event->pos();

      scene_.setSceneRect(original_mouse_pos_.x() - event->x(),
                          original_mouse_pos_.y() - event->y(), scene_.width(),
                          scene_.height());
    }

    emit coordinate_changed(
        QString("%1, %2%3")
            .arg(top_left_.x() + event->x() * pixel_width_, 0, 'f', 2)
            .arg(top_left_.y() + event->y() * pixel_height_, 0, 'f', 2)
            .arg(cs_name_));
  }
}

void view_2d::mousePressEvent(QMouseEvent *event) {
  if (layers_.empty())
    return;

  if (Qt::MidButton == event->button()) {
    setDragMode(QGraphicsView::ScrollHandDrag);
    mid_button_pressed = true;
    prev_position_ = event->pos();
    original_mouse_pos_ = prev_position_;
  }
}

void view_2d::mouseReleaseEvent(QMouseEvent *event) {
  if (layers_.empty())
    return;

  if (Qt::MidButton == event->button()) {
    if (mid_button_pressed) {
      mid_button_pressed = false;
      setDragMode(QGraphicsView::NoDrag);

      redraw();
    }
  }
}

void view_2d::wheelEvent(QWheelEvent *event) {
  if (layers_.empty())
    return;

  if (event->angleDelta().y() > 0) {
    zoom_multiples_ *= 2.0;

    int x_offset = viewport()->geometry().width() / 2 - event->x();
    int y_offset = viewport()->geometry().height() / 2 - event->y();

    top_left_.offset(-x_offset * pixel_width_, -y_offset * pixel_height_);

    // pixel_width_ *= 0.5;
    // pixel_height_ *= 0.5;

    // top_left_.offset((viewport()->geometry().width() / 2) * pixel_width_,
                     // (viewport()->geometry().height() / 2) * pixel_height_);

    redraw();
  } else if (event->angleDelta().y() < 0) {
    zoom_multiples_ *= 0.5;

    int x_offset = viewport()->geometry().width() / 2 - event->x();
    int y_offset = viewport()->geometry().height() / 2 - event->y();

    top_left_.offset(-x_offset * pixel_width_, -y_offset * pixel_height_);

    // pixel_width_ *= 2.0;
    // pixel_height_ *= 2.0;

    // top_left_.offset((viewport()->geometry().width() / 2) * pixel_width_,
                     // (viewport()->geometry().height() / 2) * pixel_height_);

    redraw();
  }
}

void view_2d::resizeEvent(QResizeEvent *event) {
  scene_.setSceneRect(0, 0, event->size().width(), event->size().height());

  redraw();
}

void view_2d::reset() { scene_.clear(); }

void view_2d::redraw() {
  scene_.clear();
  scene_.setSceneRect(0, 0, scene_.width(), scene_.height());

  for (auto &e : layers_) {
    // 暂时只考虑栅格数据
    e->draw(scene_, top_left_.x(), top_left_.y(), pixel_width_, pixel_height_);
  }
}

void view_2d::create_actions() {
  change_background_color_action_ =
      new QAction(QIcon(":/palette.svg"), tr("背景色..."), this);
  connect(change_background_color_action_, &QAction::triggered, this,
          &view_2d::change_background_color);
}

void view_2d::show_context_menu() {
  if (QGraphicsView::ScrollHandDrag == dragMode())
    return;

  QMenu *context_menu = new QMenu();
  context_menu->addAction(change_background_color_action_);
  context_menu->exec(QCursor::pos());
}

void view_2d::change_background_color() {
  QColor color = QColorDialog::getColor(
      backgroundBrush().color(), nullptr, tr("选择背景色"),
      QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    setBackgroundBrush(color);
}

void view_2d::add_raster_layers() {
  std::string pathname;
  QStringList paths =
      lemon::file_dialog::get_open_raster_names(this, tr("添加栅格图层"));
  if (!paths.isEmpty()) {
    std::size_t old_size = layers_.size();

    foreach (const QString &path, paths) {
      std::unique_ptr<lemon::raster> raster = std::make_unique<lemon::raster>();

      pathname = lemon::to_string(path);

      if (raster->load(pathname, true)) {
        if (layers_.empty()) {
          spatial_ref_ = raster->ogr_spatial_ref();
          top_left_.reset(raster->metadata()->left, raster->metadata()->top);
          pixel_width_ = raster->metadata()->pixel_width;
          pixel_height_ = std::abs(raster->metadata()->pixel_height);
          cs_name_ = gen_cs_name(raster->metadata());
        } else if (!lemon::is_same_spatial_ref(raster.get(), spatial_ref_)) {
          lemon::message_box::critical(this, tr("错误"),
                                       tr("不一致的空间参考！"));
          return;
        }

        raster_layer *layer = new raster_layer();
        layer->set_raster(raster.release());

        QTreeWidgetItem *layer_item =
            new QTreeWidgetItem(QStringList(lemon::to_qstring(
                lemon::get_filename_without_extension(pathname))));
        layer_item->setCheckState(0, Qt::Checked);
        layer_item->setIcon(0, QIcon(":/raster.svg"));
        layer_item->setToolTip(0, path);
        layer_item->setData(0, 1,
                            QVariant::fromValue((const layer_base *)(layer)));

        layer->set_layer_item(layer_item);

        view_item_->insertChild(0, layer_item);
        view_item_->setExpanded(true);

        layers_.emplace_back(
            std::unique_ptr<layer_base>(static_cast<layer_base *>(layer)));
      } else {
        lemon::message_box::critical(this, tr("错误"),
                                     tr("打开影像文件失败！"));
      }
    }

    if (layers_.size() != old_size)
      redraw();
  }
}

void view_2d::add_vector_layers() {
  std::string pathname;
  QStringList paths =
      lemon::file_dialog::get_open_vector_names(this, "添加矢量图层");
  if (!paths.isEmpty()) {
    foreach (const QString &path, paths) {
      std::unique_ptr<lemon::vector> vector = std::make_unique<lemon::vector>();

      pathname = lemon::to_string(path);

      if (vector->load(pathname)) {
        if (layers_.empty()) {
          lemon::message_box::critical(this, tr("错误"),
                                       tr("请先添加至少一个栅格图层！"));
          return;
        } else if (!lemon::is_same_spatial_ref(vector.get(), spatial_ref_)) {
          lemon::message_box::critical(this, tr("错误"),
                                       tr("不一致的空间参考！"));
          return;
        }

        vector_layer *layer = new vector_layer();
        layer->set_vector(vector.release());

        QTreeWidgetItem *layer_item = new QTreeWidgetItem(
            view_item_, QStringList(lemon::to_qstring(
                            lemon::get_filename_without_extension(pathname))));
        layer_item->setCheckState(0, Qt::Checked);
        layer_item->setIcon(0, QIcon(":/vector.svg"));
        layer_item->setToolTip(0, path);
        layer_item->setData(0, 1,
                            QVariant::fromValue((const layer_base *)(layer)));

        layer->set_layer_item(layer_item);

        view_item_->insertChild(0, layer_item);
        view_item_->setExpanded(true);

        layers_.emplace_back(
            std::unique_ptr<layer_base>(static_cast<layer_base *>(layer)));
      } else {
        lemon::message_box::critical(this, tr("错误"),
                                     tr("打开矢量文件失败！"));
      }
    }
  }
}

void view_2d::remove_layer(int index) {
  layers_.erase(layers_.begin() + index);
  layers_.shrink_to_fit();

  if (layers_.empty())
    reset();
  else
    redraw();
}

QString view_2d::gen_cs_name(const lemon::metadata *metadata) {
  if (metadata->proj_cs)
    return QString("    (%1 / %2)")
        .arg(lemon::to_qstring(metadata->proj_cs->name))
        .arg(lemon::to_qstring(metadata->proj_cs->geog_cs.name));

  if (metadata->geog_cs)
    return QString("    (%1)").arg(lemon::to_qstring(metadata->geog_cs->name));

  return QString();
}
