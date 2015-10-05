#include <QtWidgets>

#include "spatial_reference_tab.h"
#include "metadata.h"
#include "utility.h"

QGroupBox *add_geog_cs_group(lemon::geographic_coordinate_system *geog_cs) {
  QGroupBox *geog_cs_group = new QGroupBox(QObject::tr("地理坐标系统"));

  QVBoxLayout *main_layout = new QVBoxLayout();

  QLabel *name_label = new QLabel(QObject::tr("名称"));
  QLabel *name = new QLabel(lemon::to_qstring(geog_cs->name));

  QHBoxLayout *name_layout = new QHBoxLayout();
  name_layout->addWidget(name_label);
  name_layout->addWidget(name);

  main_layout->addLayout(name_layout);

  if (!geog_cs->authority_name.empty() || !geog_cs->authority_code.empty()) {
    QLabel *authority_name =
        new QLabel(lemon::to_qstring(geog_cs->authority_name));
    QLabel *authority_code =
        new QLabel(lemon::to_qstring(geog_cs->authority_code));

    QHBoxLayout *authority_layout = new QHBoxLayout();
    authority_layout->addWidget(authority_name);
    authority_layout->addWidget(authority_code);

    main_layout->addLayout(authority_layout);
  }

  QLabel *datum_label = new QLabel(QObject::tr("基准面"));
  QLabel *datum = new QLabel(lemon::to_qstring(geog_cs->datum));

  QHBoxLayout *datum_layout = new QHBoxLayout();
  datum_layout->addWidget(datum_label);
  datum_layout->addWidget(datum);

  QLabel *spheroid_label = new QLabel(QObject::tr("椭球体"));
  QLabel *spheroid = new QLabel(lemon::to_qstring(geog_cs->spheroid));

  QHBoxLayout *spheroid_layout = new QHBoxLayout();
  spheroid_layout->addWidget(spheroid_label);
  spheroid_layout->addWidget(spheroid);

  QLabel *semimajor_label = new QLabel(QObject::tr("长半轴"));
  QLabel *semimajor = new QLabel(QString::number(geog_cs->semimajor));

  QHBoxLayout *semimajor_layout = new QHBoxLayout();
  semimajor_layout->addWidget(semimajor_label);
  semimajor_layout->addWidget(semimajor);

  QLabel *semiminor_label = new QLabel(QObject::tr("短半轴"));
  QLabel *semiminor = new QLabel(QString::number(geog_cs->semiminor));

  QHBoxLayout *semiminor_layout = new QHBoxLayout();
  semiminor_layout->addWidget(semiminor_label);
  semiminor_layout->addWidget(semiminor);

  QLabel *inverse_flattening_label = new QLabel(QObject::tr("扁率"));
  QLabel *inverse_flattening =
      new QLabel(QString::number(geog_cs->inverse_flattening));

  QHBoxLayout *inverse_flattening_layout = new QHBoxLayout();
  inverse_flattening_layout->addWidget(inverse_flattening_label);
  inverse_flattening_layout->addWidget(inverse_flattening);

  QLabel *angular_unit_label = new QLabel(QObject::tr("角度单位"));
  QLabel *angular_unit = new QLabel(lemon::to_qstring(geog_cs->angular_unit));

  QHBoxLayout *angular_unit_layout = new QHBoxLayout();
  angular_unit_layout->addWidget(angular_unit_label);
  angular_unit_layout->addWidget(angular_unit);

  QLabel *radians_per_unit_label = new QLabel(QObject::tr("弧度/单位"));
  QLabel *radians_per_unit =
      new QLabel(QString::number(geog_cs->radians_per_unit));

  QHBoxLayout *radians_per_unit_layout = new QHBoxLayout();
  radians_per_unit_layout->addWidget(radians_per_unit_label);
  radians_per_unit_layout->addWidget(radians_per_unit);

  QLabel *prime_meridian_label = new QLabel(QObject::tr("本初子午线"));
  QLabel *prime_meridian =
      new QLabel(lemon::to_qstring(geog_cs->prime_meridian));

  QHBoxLayout *prime_meridian_layout = new QHBoxLayout();
  prime_meridian_layout->addWidget(prime_meridian_label);
  prime_meridian_layout->addWidget(prime_meridian);

  QLabel *meridian_longitude_label = new QLabel(QObject::tr("子午线经度"));
  QLabel *meridian_longitude =
      new QLabel(QString::number(geog_cs->meridian_longitude));

  QHBoxLayout *meridian_longitude_layout = new QHBoxLayout();
  meridian_longitude_layout->addWidget(meridian_longitude_label);
  meridian_longitude_layout->addWidget(meridian_longitude);

  main_layout->addLayout(datum_layout);
  main_layout->addLayout(spheroid_layout);
  main_layout->addLayout(semimajor_layout);
  main_layout->addLayout(semiminor_layout);
  main_layout->addLayout(inverse_flattening_layout);
  main_layout->addLayout(angular_unit_layout);
  main_layout->addLayout(radians_per_unit_layout);
  main_layout->addLayout(prime_meridian_layout);
  main_layout->addLayout(meridian_longitude_layout);

  if (geog_cs->have_towgs84_node) {
    QLabel *towgs84_params_label = new QLabel(QObject::tr("TOWGS84参数"));

    QString params;

    for (int i = 0; i < 6; ++i)
      params.append(QString::number(geog_cs->towgs84_params[i])).append(", ");

    params.append(QString::number(geog_cs->towgs84_params[6]));

    QLabel *towgs84_params = new QLabel(params);

    QHBoxLayout *towgs84_params_layout = new QHBoxLayout();
    towgs84_params_layout->addWidget(towgs84_params_label);
    towgs84_params_layout->addWidget(towgs84_params);

    main_layout->addLayout(towgs84_params_layout);
  }

  geog_cs_group->setLayout(main_layout);

  return geog_cs_group;
}

QGroupBox *add_proj_cs_group(lemon::projected_coordinate_system *proj_cs) {
  QGroupBox *proj_cs_group = new QGroupBox(QObject::tr("投影坐标系统"));

  QVBoxLayout *main_layout = new QVBoxLayout();

  QLabel *name_label = new QLabel(QObject::tr("名称"));
  QLabel *name = new QLabel(lemon::to_qstring(proj_cs->name));

  QHBoxLayout *name_layout = new QHBoxLayout();
  name_layout->addWidget(name_label);
  name_layout->addWidget(name);

  QLabel *projection_label = new QLabel(QObject::tr("投影"));
  QLabel *projection = new QLabel(lemon::to_qstring(proj_cs->projection_name));

  QHBoxLayout *projection_layout = new QHBoxLayout();
  projection_layout->addWidget(projection_label);
  projection_layout->addWidget(projection);

  main_layout->addLayout(name_layout);
  main_layout->addLayout(projection_layout);

  if (!proj_cs->authority_name.empty() || !proj_cs->authority_code.empty()) {
    QLabel *authority_name =
        new QLabel(lemon::to_qstring(proj_cs->authority_name));
    QLabel *authority_code =
        new QLabel(lemon::to_qstring(proj_cs->authority_code));

    QHBoxLayout *authority_layout = new QHBoxLayout();
    authority_layout->addWidget(authority_name);
    authority_layout->addWidget(authority_code);

    main_layout->addLayout(authority_layout);
  }

  QLabel *linear_unit_label = new QLabel(QObject::tr("线性单位"));
  QLabel *linear_unit = new QLabel(lemon::to_qstring(proj_cs->linear_unit));

  QHBoxLayout *linear_unit_layout = new QHBoxLayout();
  linear_unit_layout->addWidget(linear_unit_label);
  linear_unit_layout->addWidget(linear_unit);

  QLabel *meters_per_unit_label = new QLabel(QObject::tr("米/单位"));
  QLabel *meters_per_unit =
      new QLabel(QString::number(proj_cs->meters_per_unit));

  QHBoxLayout *meters_per_unit_layout = new QHBoxLayout();
  meters_per_unit_layout->addWidget(meters_per_unit_label);
  meters_per_unit_layout->addWidget(meters_per_unit);

  main_layout->addLayout(linear_unit_layout);
  main_layout->addLayout(meters_per_unit_layout);

  if (!proj_cs->projection_params.empty()) {
    main_layout->addWidget(new QLabel(QObject::tr("参数")));

    QTableWidget *params = new QTableWidget(
        static_cast<int>(proj_cs->projection_params.size()), 2);
    params->horizontalHeader()->setVisible(false);
    params->verticalHeader()->setVisible(false);
    params->setEditTriggers(QAbstractItemView::NoEditTriggers);
    params->setSelectionMode(QAbstractItemView::NoSelection);

    int row_index = 0;
    for (auto &e : proj_cs->projection_params) {
      params->setItem(row_index, 0,
                      new QTableWidgetItem(lemon::to_qstring(e.first)));
      params->setItem(row_index, 1,
                      new QTableWidgetItem(QString::number(e.second)));

      ++row_index;
    }

    params->resizeColumnsToContents();

    main_layout->addWidget(params);
  }

  main_layout->addWidget(add_geog_cs_group(&proj_cs->geog_cs));

  proj_cs_group->setLayout(main_layout);

  return proj_cs_group;
}

QGroupBox *add_local_cs_group(lemon::local_coordinate_system *local_cs) {
  QGroupBox *local_cs_group = new QGroupBox(QObject::tr("地方坐标系统"));

  return local_cs_group;
}

QGroupBox *add_vert_cs_group(lemon::vertical_coordinate_system *vert_cs) {
  QGroupBox *vert_cs_group = new QGroupBox(QObject::tr("垂直坐标系统"));

  QVBoxLayout *main_layout = new QVBoxLayout();

  QLabel *name_label = new QLabel(QObject::tr("名称"));
  QLabel *name = new QLabel(lemon::to_qstring(vert_cs->name));

  QHBoxLayout *name_layout = new QHBoxLayout();
  name_layout->addWidget(name_label);
  name_layout->addWidget(name);

  main_layout->addLayout(name_layout);

  if (!vert_cs->authority_name.empty() || !vert_cs->authority_code.empty()) {
    QLabel *authority_name =
        new QLabel(lemon::to_qstring(vert_cs->authority_name));
    QLabel *authority_code =
        new QLabel(lemon::to_qstring(vert_cs->authority_code));

    QHBoxLayout *authority_layout = new QHBoxLayout();
    authority_layout->addWidget(authority_name);
    authority_layout->addWidget(authority_code);

    main_layout->addLayout(authority_layout);
  }

  QLabel *vert_datum_label = new QLabel(QObject::tr("垂直基准面"));
  QLabel *vert_datum = new QLabel(lemon::to_qstring(vert_cs->name));

  QHBoxLayout *vert_datum_layout = new QHBoxLayout();
  vert_datum_layout->addWidget(vert_datum_label);
  vert_datum_layout->addWidget(vert_datum);

  QLabel *linear_unit_label = new QLabel(QObject::tr("线性单位"));
  QLabel *linear_unit = new QLabel(lemon::to_qstring(vert_cs->linear_unit));

  QHBoxLayout *linear_unit_layout = new QHBoxLayout();
  linear_unit_layout->addWidget(linear_unit_label);
  linear_unit_layout->addWidget(linear_unit);

  QLabel *meters_per_unit_label = new QLabel(QObject::tr("米/单位"));
  QLabel *meters_per_unit =
      new QLabel(QString::number(vert_cs->meters_per_unit));

  QHBoxLayout *meters_per_unit_layout = new QHBoxLayout();
  meters_per_unit_layout->addWidget(meters_per_unit_label);
  meters_per_unit_layout->addWidget(meters_per_unit);

  main_layout->addLayout(vert_datum_layout);
  main_layout->addLayout(linear_unit_layout);
  main_layout->addLayout(meters_per_unit_layout);

  if (!vert_cs->axis_name.empty()) {
    QLabel *axis_name_label = new QLabel(QObject::tr("坐标轴"));
    QLabel *axis_name = new QLabel(lemon::to_qstring(vert_cs->axis_name));

    QHBoxLayout *axis_name_layout = new QHBoxLayout();
    axis_name_layout->addWidget(axis_name_label);
    axis_name_layout->addWidget(axis_name);

    QLabel *axis_orientation_label = new QLabel(QObject::tr("坐标轴方向"));
    QLabel *axis_orientation =
        new QLabel(lemon::to_qstring(vert_cs->axis_orientation));

    QHBoxLayout *axis_orientation_layout = new QHBoxLayout();
    axis_orientation_layout->addWidget(axis_orientation_label);
    axis_orientation_layout->addWidget(axis_orientation);

    main_layout->addLayout(axis_name_layout);
    main_layout->addLayout(axis_orientation_layout);
  }

  vert_cs_group->setLayout(main_layout);

  return vert_cs_group;
}

spatial_reference_tab::spatial_reference_tab(const lemon::metadata *metadata,
                                             QWidget *parent)
    : QWidget(parent) {
  QVBoxLayout *main_layout = new QVBoxLayout();

  if (metadata->proj_cs)
    main_layout->addWidget(add_proj_cs_group(metadata->proj_cs.get()));
  if (metadata->geog_cs)
    main_layout->addWidget(add_geog_cs_group(metadata->geog_cs.get()));
  if (metadata->local_cs)
    main_layout->addWidget(add_local_cs_group(metadata->local_cs.get()));
  if (metadata->vert_cs)
    main_layout->addWidget(add_vert_cs_group(metadata->vert_cs.get()));

  setLayout(main_layout);
}
