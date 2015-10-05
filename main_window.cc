#include <QtWidgets>

#include "main_window.h"
#include "utility.h"
#include "view_2d.h"
#include "raster_layer.h"
#include "vector_layer.h"
#include "catalog_view.h"
#include "file_dialog.h"
#include "metadata_dialog.h"
#include "change_rgb_bands_dialog.h"

Q_DECLARE_METATYPE(const layer_base *)
Q_DECLARE_METATYPE(view_2d *)

main_window::main_window() {
  mdi_area_ = new QMdiArea();

  setCentralWidget(mdi_area_);

  read_settings();

  create_actions();
  create_menus();
  create_toolbars();
  create_statusbar();
  create_dock_windows();
  create_view();

  setUnifiedTitleAndToolBarOnMac(true);

  view_mode_ = view_mode::PAN;

  previous_vector_ = nullptr;

  lemon::raster::init();
  lemon::vector::init();
}

main_window::~main_window() {
  lemon::raster::uninit();
  lemon::vector::uninit();
}

void main_window::closeEvent(QCloseEvent *event) {
  mdi_area_->closeAllSubWindows();
  if (mdi_area_->currentSubWindow()) {
    event->ignore();
  } else {
    write_settings();
    event->accept();
  }
}

void main_window::read_settings() {
  QSettings settings;
  lemon::file_dialog::set_last_directory(
      settings.value("last_path", QDir::homePath()).toString());
}

void main_window::write_settings() {
  QSettings settings;
  settings.setValue("last_path", lemon::file_dialog::last_directory());
}

void main_window::create_actions() {
  new_action_ = new QAction(QIcon(":/view.svg"), tr("新建"), this);
  new_action_->setShortcut(QKeySequence::New);
  new_action_->setStatusTip(tr("新建视图"));
  connect(new_action_, &QAction::triggered, this, &main_window::create_view);

  open_action_ = new QAction(QIcon(":/open.svg"), tr("打开..."), this);
  open_action_->setShortcut(QKeySequence::Open);
  open_action_->setStatusTip(tr("打开一个Lemon Image工程文件"));
  connect(open_action_, &QAction::triggered, this, &main_window::open);

  save_action_ = new QAction(QIcon(":/save.svg"), tr("保存"), this);
  save_action_->setShortcut(QKeySequence::Save);
  save_action_->setStatusTip(tr("保存Lemon Image工程"));
  connect(save_action_, &QAction::triggered, this, &main_window::save);

  quit_action_ = new QAction(QIcon(":/quit.svg"), tr("退出"), this);
  quit_action_->setShortcuts(QKeySequence::Quit);
  quit_action_->setStatusTip(tr("关闭Lemon Image"));
  connect(quit_action_, &QAction::triggered, this, &main_window::close);

  select_action_ = new QAction(QIcon(":/select.svg"), tr("选择"), this);
  select_action_->setCheckable(true);
  select_action_->setStatusTip(tr("选择元素"));
  connect(select_action_, &QAction::triggered, this, &main_window::select);

  pan_action_ = new QAction(QIcon(":/hand.svg"), tr("漫游"), this);
  pan_action_->setCheckable(true);
  pan_action_->setChecked(true);
  pan_action_->setStatusTip(tr("平移视图"));
  connect(pan_action_, &QAction::triggered, this, &main_window::pan);

  zoom_in_action_ = new QAction(QIcon(":/zoom_in.svg"), "放大", this);
  zoom_in_action_->setCheckable(true);
  zoom_in_action_->setShortcut(QKeySequence::ZoomIn);
  zoom_in_action_->setStatusTip(tr("以鼠标点击处为中心放大视图区域"));
  connect(zoom_in_action_, &QAction::triggered, this, &main_window::zoom_in);

  zoom_out_action_ = new QAction(QIcon(":/zoom_out.svg"), tr("缩小"), this);
  zoom_out_action_->setCheckable(true);
  zoom_out_action_->setShortcut(QKeySequence::ZoomOut);
  zoom_out_action_->setStatusTip(tr("以鼠标点击处为中心缩小视图区域"));
  connect(zoom_out_action_, &QAction::triggered, this, &main_window::zoom_out);

  add_raster_layers_action_ =
      new QAction(QIcon(":/raster.svg"), tr("添加栅格图层"), this);
  add_raster_layers_action_->setStatusTip(tr("添加栅格图层至当前活动视图"));
  connect(add_raster_layers_action_, &QAction::triggered, this,
          &main_window::add_raster_layers);

  add_vector_layers_action_ =
      new QAction(QIcon(":/vector.svg"), tr("添加矢量图层"), this);
  add_vector_layers_action_->setStatusTip(tr("添加矢量图层至当前活动视图"));
  connect(add_vector_layers_action_, &QAction::triggered, this,
          &main_window::add_vector_layers);

  cascade_action_ = new QAction(QIcon(":/cascade.svg"), tr("层叠"), this);
  cascade_action_->setStatusTip(tr("层叠显示所有视图"));
  connect(cascade_action_, &QAction::triggered, mdi_area_,
          &QMdiArea::cascadeSubWindows);

  tile_action_ = new QAction(QIcon(":/tile.svg"), "平铺", this);
  tile_action_->setStatusTip(tr("平铺显示所有视图"));
  connect(tile_action_, &QAction::triggered, mdi_area_,
          &QMdiArea::tileSubWindows);

  about_action_ = new QAction(QIcon(":/lemon.svg"), "关于 Lemon Image", this);
  about_action_->setStatusTip(tr("查看Lemon Image的版权信息"));
  connect(about_action_, &QAction::triggered, this, &main_window::about);

  remove_layer_action_ =
      new QAction(QIcon(":/remove.svg"), tr("移除图层"), this);
  remove_layer_action_->setStatusTip(tr("从视图中移除该图层"));
  connect(remove_layer_action_, &QAction::triggered, this,
          &main_window::remove_layer);

  show_metadata_dialog_action_ =
      new QAction(QIcon(":/metadata.svg"), tr("元数据..."), this);
  show_metadata_dialog_action_->setStatusTip(tr("显示栅格数据的元数据"));
  connect(show_metadata_dialog_action_, &QAction::triggered, this,
          &main_window::show_metadata_dialog);

  change_rgb_bands_action_ =
      new QAction(QIcon(":/rgb.svg"), tr("改变RGB波段..."), this);
  change_rgb_bands_action_->setStatusTip(tr("改变多光谱数据的RGB波段"));
  connect(change_rgb_bands_action_, &QAction::triggered, this,
          &main_window::change_rgb_bands);

  change_style_action_ =
      new QAction(QIcon(":/change_style.svg"), tr("样式..."), this);
  change_style_action_->setStatusTip(tr("改变矢量数据的样式"));
  connect(change_style_action_, &QAction::triggered, this,
          &main_window::change_style);

  show_attribute_table_action_ =
      new QAction(QIcon(":/attribute_table.svg"), tr("显示属性表"), this);
  show_attribute_table_action_->setStatusTip(tr("显示矢量数据的属性表"));
  connect(show_attribute_table_action_, &QAction::triggered, this,
          &main_window::show_attribute_table);
}

void main_window::create_menus() {
  file_menu_ = menuBar()->addMenu(tr("文件(&F)"));
  file_menu_->addAction(new_action_);
  file_menu_->addAction(open_action_);
  file_menu_->addAction(save_action_);
  file_menu_->addSeparator();
  file_menu_->addAction(quit_action_);

  view_menu_ = menuBar()->addMenu(tr("视图(&V)"));
  view_menu_->addAction(select_action_);
  view_menu_->addAction(pan_action_);
  view_menu_->addAction(zoom_in_action_);
  view_menu_->addAction(zoom_out_action_);

  layer_menu_ = menuBar()->addMenu(tr("图层(&L)"));
  QMenu *add_layers_menu = layer_menu_->addMenu(tr("添加图层"));
  add_layers_menu->addAction(add_raster_layers_action_);
  add_layers_menu->addAction(add_vector_layers_action_);

  window_menu_ = menuBar()->addMenu(tr("窗口(&W)"));
  window_menu_->addAction(cascade_action_);
  window_menu_->addAction(tile_action_);

  help_menu_ = menuBar()->addMenu(tr("帮助(&H)"));
  help_menu_->addAction(about_action_);
}

void main_window::create_toolbars() {
  file_toolbar_ = addToolBar(tr("文件"));
  file_toolbar_->addAction(new_action_);
  file_toolbar_->addAction(open_action_);
  file_toolbar_->addAction(save_action_);

  layer_toolbar_ = addToolBar(tr("图层"));
  layer_toolbar_->addAction(add_raster_layers_action_);
  layer_toolbar_->addAction(add_vector_layers_action_);

  view_toolbar_ = addToolBar(tr("视图"));
  view_toolbar_->addAction(select_action_);
  view_toolbar_->addAction(pan_action_);
  view_toolbar_->addAction(zoom_in_action_);
  view_toolbar_->addAction(zoom_out_action_);
}

void main_window::create_statusbar() {
  coordinate_label_ = new QLabel();

  QStatusBar *status_bar = statusBar();
  status_bar->showMessage(tr("就绪"));
  status_bar->addPermanentWidget(coordinate_label_);
}

void main_window::create_view() {
  QString title = QString(tr("2D 视图 #")) +
                  QString::number(mdi_area_->subWindowList().size() + 1);

  view_2d *view = new view_2d();
  view->setWindowTitle(title);
  connect(view, &view_2d::coordinate_changed, coordinate_label_,
          &QLabel::setText);

  QTreeWidgetItem *item = new QTreeWidgetItem(layer_tree_, QStringList(title));
  item->setCheckState(0, Qt::Checked);
  item->setIcon(0, QIcon(":/view.svg"));
  item->setData(0, 1, QVariant::fromValue(view));

  mdi_area_->addSubWindow(view);

  view->show();
  view->set_view_item(item);

  mdi_area_->tileSubWindows();
}

void main_window::create_dock_windows() {
  layer_tree_ = new QTreeWidget();
  layer_tree_->setColumnCount(1);
  layer_tree_->setHeaderHidden(true);
  layer_tree_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(layer_tree_, &QTreeWidget::itemClicked, this,
          &main_window::item_clicked);
  connect(layer_tree_, &QTreeWidget::customContextMenuRequested, this,
          &main_window::show_item_context_menu);

  layer_dock_ = new QDockWidget(tr("图层"), this);
  layer_dock_->setAllowedAreas(Qt::LeftDockWidgetArea |
                               Qt::RightDockWidgetArea);
  layer_dock_->setWidget(layer_tree_);

  catalog_tree_ = new catalog_view();

  catalog_dock_ = new QDockWidget(tr("目录"), this);
  catalog_dock_->setAllowedAreas(Qt::LeftDockWidgetArea |
                                 Qt::RightDockWidgetArea);
  catalog_dock_->setWidget(catalog_tree_);

  QTreeWidgetItem *root_item = new QTreeWidgetItem(QStringList(tr("算法")));
  root_item->setIcon(0, QIcon(":/tool.svg"));

  QTreeWidgetItem *raster_item =
      new QTreeWidgetItem(root_item, QStringList(tr("栅格")));
  raster_item->setIcon(0, QIcon(":/tool.svg"));

  QTreeWidgetItem *vetor_item =
      new QTreeWidgetItem(root_item, QStringList(tr("矢量")));
  vetor_item->setIcon(0, QIcon(":/tool.svg"));

  toolbox_tree_ = new QTreeWidget();
  toolbox_tree_->setColumnCount(1);
  toolbox_tree_->setHeaderHidden(true);
  toolbox_tree_->addTopLevelItem(root_item);
  toolbox_tree_->expandItem(root_item);

  toolbox_dock_ = new QDockWidget(tr("工具箱"), this);
  toolbox_dock_->setAllowedAreas(Qt::LeftDockWidgetArea |
                                 Qt::RightDockWidgetArea);
  toolbox_dock_->setWidget(toolbox_tree_);

  attribute_table_ = new QTableWidget();
  attribute_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  attribute_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  attribute_table_->setSelectionMode(QAbstractItemView::ExtendedSelection);

  attribute_table_dock_ = new QDockWidget(tr("属性表"), this);
  attribute_table_dock_->setAllowedAreas(Qt::LeftDockWidgetArea |
                                         Qt::RightDockWidgetArea);
  attribute_table_dock_->setWidget(attribute_table_);
  attribute_table_dock_->setHidden(true);

  addDockWidget(Qt::LeftDockWidgetArea, layer_dock_);
  addDockWidget(Qt::LeftDockWidgetArea, catalog_dock_);

  tabifyDockWidget(layer_dock_, catalog_dock_);

  layer_dock_->raise();

  addDockWidget(Qt::RightDockWidgetArea, toolbox_dock_);
  addDockWidget(Qt::RightDockWidgetArea, attribute_table_dock_);
}

void main_window::open() {}

void main_window::save() {}

void main_window::select() {
  if (view_mode_ != view_mode::SELECT) {
    view_mode_ = view_mode::SELECT;

    select_action_->setChecked(true);
    pan_action_->setChecked(false);
    zoom_in_action_->setChecked(false);
    zoom_out_action_->setChecked(false);

    active_view()->setCursor(QCursor(Qt::ArrowCursor));
  }
}

void main_window::pan() {
  if (view_mode_ != view_mode::PAN) {
    view_mode_ = view_mode::PAN;

    pan_action_->setChecked(true);
    select_action_->setChecked(false);
    zoom_in_action_->setChecked(false);
    zoom_out_action_->setChecked(false);

    active_view()->setCursor(QCursor(Qt::OpenHandCursor));
  }
}

void main_window::zoom_in() {
  if (view_mode_ != view_mode::ZOOM_IN) {
    view_mode_ = view_mode::ZOOM_IN;

    zoom_in_action_->setChecked(true);
    pan_action_->setChecked(false);
    select_action_->setChecked(false);
    zoom_out_action_->setChecked(false);

    active_view()->setCursor(QCursor(QPixmap(":/zoom_in.png")));
  }
}

void main_window::zoom_out() {
  if (view_mode_ != view_mode::ZOOM_OUT) {
    view_mode_ = view_mode::ZOOM_OUT;

    zoom_out_action_->setChecked(true);
    zoom_in_action_->setChecked(false);
    pan_action_->setChecked(false);
    select_action_->setChecked(false);

    active_view()->setCursor(QCursor(QPixmap(":/zoom_out.png")));
  }
}

void main_window::add_raster_layers() {
  view_2d *view = active_view();
  view->add_raster_layers();
}

void main_window::add_vector_layers() {
  view_2d *view = active_view();
  view->add_vector_layers();
}

void main_window::about() {
  QMessageBox::about(
      this, tr("关于 Lemon Image"),
      tr("Lemon Image 跨平台遥感数据处理系统\n\nCopyright 2015, Michael "
         "Yang\n\nymh1232001@163.com\n\n许可协议：GNU 通用公共许可协议版本 3"));
}

view_2d *main_window::active_view() {
  QMdiSubWindow *active_sub_window = mdi_area_->activeSubWindow();
  if (nullptr == active_sub_window)
    return nullptr;

  return qobject_cast<view_2d *>(active_sub_window->widget());
}

void main_window::item_clicked(QTreeWidgetItem *item, int column) {
  if (Qt::Checked == item->checkState(0))
    item->data(0, 1).value<view_2d *>()->parentWidget()->show();
  else
    item->data(0, 1).value<view_2d *>()->parentWidget()->hide();
}

void main_window::show_item_context_menu(const QPoint &pos) {
  QTreeWidgetItem *item = layer_tree_->itemAt(pos);
  if (nullptr == item)
    return;

  current_item_ = item;

  if (nullptr == item->parent()) { // view_item

  } else { // layer_item
    QMenu *context_menu = new QMenu(this);
    context_menu->addAction(remove_layer_action_);
    context_menu->addSeparator();
    context_menu->addAction(show_metadata_dialog_action_);

    const layer_base *layer = item->data(0, 1).value<const layer_base *>();

    if (layer->is_raster_layer()) {
      if (static_cast<const raster_layer *>(layer)
              ->raster()
              ->metadata()
              ->num_bands >= 3)
        context_menu->addAction(change_rgb_bands_action_);
    } else {
      context_menu->addAction(change_style_action_);
      context_menu->addAction(show_attribute_table_action_);
    }

    context_menu->exec(QCursor::pos());
  }
}

void main_window::remove_layer() {
  QTreeWidgetItem *view_item = current_item_->parent();

  view_2d *view = view_item->data(0, 1).value<view_2d *>();
  view->remove_layer(view_item->indexOfChild(current_item_));
}

void main_window::show_metadata_dialog() {
  const layer_base *layer =
      current_item_->data(0, 1).value<const layer_base *>();
  metadata_dialog *dialog = new metadata_dialog(layer);
  dialog->show();
}

void main_window::change_rgb_bands() {
  raster_layer *layer =
      const_cast<raster_layer *>(static_cast<const raster_layer *>(
          current_item_->data(0, 1).value<const layer_base *>()));

  if (layer->change_rgb_bands_dlg() != nullptr) {
    layer->show_change_rgb_bands_dialog();
    return;
  }

  change_rgb_bands_dialog *dialog = new change_rgb_bands_dialog(
      layer->raster()->metadata(), layer->red_band_index(),
      layer->green_band_index(), layer->blue_band_index());

  connect(dialog, &change_rgb_bands_dialog::closed, layer,
          &raster_layer::change_rgb_bands_dialog_closed);
  connect(dialog, &change_rgb_bands_dialog::red_band_changed, layer,
          &raster_layer::set_red_band_index);
  connect(dialog, &change_rgb_bands_dialog::green_band_changed, layer,
          &raster_layer::set_green_band_index);
  connect(dialog, &change_rgb_bands_dialog::blue_band_changed, layer,
          &raster_layer::set_blue_band_index);

  layer->set_change_rgb_bands_dialog(dialog);

  dialog->show();
}

void main_window::change_style() {}

void main_window::show_attribute_table() {
  const lemon::vector *vector =
      static_cast<const vector_layer *>(
          current_item_->data(0, 1).value<const layer_base *>())->vector();

  if (previous_vector_ == vector) {
    attribute_table_dock_->raise();

    return;
  }

  if (attribute_table_dock_->isHidden()) {
    if (dockWidgetArea(layer_dock_) != Qt::RightDockWidgetArea &&
        dockWidgetArea(catalog_dock_) != Qt::RightDockWidgetArea &&
        dockWidgetArea(toolbox_dock_) != Qt::RightDockWidgetArea) {
      if (Qt::RightDockWidgetArea == dockWidgetArea(attribute_table_dock_))
        attribute_table_dock_->setHidden(false);
      else
        addDockWidget(Qt::RightDockWidgetArea, attribute_table_dock_);
    } else {
      QDockWidget *right_dock;

      if (Qt::RightDockWidgetArea == dockWidgetArea(layer_dock_))
        right_dock = layer_dock_;
      else if (Qt::RightDockWidgetArea == dockWidgetArea(catalog_dock_))
        right_dock = catalog_dock_;
      else
        right_dock = toolbox_dock_;

      attribute_table_dock_->setHidden(false);
      tabifyDockWidget(right_dock, attribute_table_dock_);
    }
  } else {
    attribute_table_dock_->raise();
  }

  attribute_table_->clearContents();

  QStringList header;

  const std::vector<std::string> *field_name = vector->field_name(0);

  for (auto &e : *field_name)
    header << lemon::to_qstring(e);

  const std::vector<std::vector<std::string>> *attribute_table =
      vector->attribute_table(0);

  int num_columns = field_name->size();

  int num_rows = attribute_table->size();

  attribute_table_->setRowCount(num_rows);
  attribute_table_->setColumnCount(num_columns);

  for (int row_index = 0; row_index < num_rows; ++row_index) {
    for (int column_index = 0; column_index < num_columns; ++column_index) {
      attribute_table_->setItem(
          row_index, column_index,
          new QTableWidgetItem(
              lemon::to_qstring((*attribute_table)[row_index][column_index])));
    }
  }

  attribute_table_->setHorizontalHeaderLabels(header);
  attribute_table_->resizeColumnsToContents();

  previous_vector_ = const_cast<lemon::vector *>(vector);
}
