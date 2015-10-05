#include <QtWidgets>

#include "metadata_tab.h"
#include "raster.h"
#include "utility.h"

metadata_tab::metadata_tab(const lemon::metadata_domain *metadata,
                           QWidget *parent)
    : QWidget(parent) {
  int num_titles = 4;
  if (metadata->default_domain.empty())
    --num_titles;
  if (metadata->image_structure_domain.empty())
    --num_titles;
  if (metadata->geolocation_domain.empty())
    --num_titles;
  if (metadata->rpc_domain.empty())
    --num_titles;

  QTableWidget *table = new QTableWidget(
      static_cast<int>(metadata->default_domain.size() +
                       metadata->image_structure_domain.size()) +
          num_titles,
      2);
  table->horizontalHeader()->setVisible(false);
  table->verticalHeader()->setVisible(false);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table->setSelectionMode(QAbstractItemView::NoSelection);

  QFont bold_font;
  bold_font.setBold(true);

  int row_index = 0;

  if (!metadata->default_domain.empty()) {
    QTableWidgetItem *title = new QTableWidgetItem(tr("Metadata"));
    title->setFont(bold_font);
    title->setBackgroundColor(Qt::gray);

    table->setSpan(0, 0, 1, 2);
    table->setItem(0, 0, title);

    ++row_index;

    for (auto &e : metadata->default_domain) {
      table->setItem(row_index, 0,
                     new QTableWidgetItem(lemon::to_qstring(e.first)));
      table->setItem(row_index, 1,
                     new QTableWidgetItem(lemon::to_qstring(e.second)));

      ++row_index;
    }
  }

  if (!metadata->image_structure_domain.empty()) {
    QTableWidgetItem *title =
        new QTableWidgetItem(tr("Image Structure Metadata"));
    title->setFont(bold_font);
    title->setBackgroundColor(Qt::gray);

    table->setSpan(row_index, 0, 1, 2);
    table->setItem(row_index, 0, title);

    ++row_index;

    for (auto &e : metadata->image_structure_domain) {
      table->setItem(row_index, 0,
                     new QTableWidgetItem(lemon::to_qstring(e.first)));
      table->setItem(row_index, 1,
                     new QTableWidgetItem(lemon::to_qstring(e.second)));

      ++row_index;
    }
  }

  if (!metadata->geolocation_domain.empty()) {
    QTableWidgetItem *title = new QTableWidgetItem(tr("Geolocation Metadata"));
    title->setFont(bold_font);
    title->setBackgroundColor(Qt::gray);

    table->setSpan(row_index, 0, 1, 2);
    table->setItem(row_index, 0, title);

    ++row_index;

    for (auto &e : metadata->geolocation_domain) {
      table->setItem(row_index, 0,
                     new QTableWidgetItem(lemon::to_qstring(e.first)));
      table->setItem(row_index, 1,
                     new QTableWidgetItem(lemon::to_qstring(e.second)));

      ++row_index;
    }
  }

  if (!metadata->rpc_domain.empty()) {
    QTableWidgetItem *title = new QTableWidgetItem(tr("RPC Metadata"));
    title->setFont(bold_font);
    title->setBackgroundColor(Qt::gray);

    table->setSpan(row_index, 0, 1, 2);
    table->setItem(row_index, 0, title);

    ++row_index;

    for (auto &e : metadata->rpc_domain) {
      table->setItem(row_index, 0,
                     new QTableWidgetItem(lemon::to_qstring(e.first)));
      table->setItem(row_index, 1,
                     new QTableWidgetItem(lemon::to_qstring(e.second)));

      ++row_index;
    }
  }

  table->resizeColumnsToContents();

  QHBoxLayout *main_layout = new QHBoxLayout();
  main_layout->addWidget(table);

  setLayout(main_layout);
}
