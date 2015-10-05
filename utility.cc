#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#ifdef LEMON_HAVE_QT
#include <QObject>
#include <QString>
#endif

#include "utility.h"

namespace lemon {
#ifdef LEMON_HAVE_QT
QString to_qstring(const std::string &str) { return str.c_str(); }

std::string to_string(const QString &str) {
  return str.toLocal8Bit().constData();
}

QVector<QRgb> to_qt_color_table(std::vector<color_entry> color_table) {
  QVector<QRgb> qt_color_table;

  for (auto &e : color_table)
    qt_color_table.push_back(qRgba(static_cast<int>(e.r), static_cast<int>(e.g),
                                   static_cast<int>(e.b),
                                   static_cast<int>(e.a)));

  return qt_color_table;
}

std::vector<QString> gen_band_list(const raster_metadata *metadata) {
  std::vector<QString> band_list;

  QString text = QObject::tr("波段_");
  int num_bands = metadata->num_bands;
  for (int i = 0; i < num_bands; ++i)
    if ("None" == metadata->band_info[i].description)
      band_list.emplace_back(text + QString::number(i + 1));
    else
      band_list.emplace_back(
          lemon::to_qstring(metadata->band_info[i].description));

  return band_list;
}
#endif

std::string get_filename(const std::string &pathname) {
  boost::filesystem::path path(pathname);
  return path.filename().string();
}

std::string get_filename_without_extension(const std::string &pathname) {
  boost::filesystem::path path(pathname);
  return path.stem().string();
}

std::uintmax_t get_filesize(const std::string &pathname) {
  boost::filesystem::path path(pathname);
  return boost::filesystem::file_size(path);
}

std::string gen_file_size_string(const std::uintmax_t &num_bytes) {
  double kb = num_bytes / 1024.0;
  double mb = kb / 1024.0;

  if (kb < 1024.0)
    return (boost::format("%f %s") % kb % "KB").str();
  else if (mb < 1024.0)
    return (boost::format("%f %s") % mb % "MB").str();
  else
    return (boost::format("%f %s") % (mb / 1024.0) % "GB").str();
}

std::vector<std::string> get_vector_pathname_list(const std::string &pathname) {
  std::vector<std::string> file_list{pathname};

  boost::filesystem::path path(pathname);

  std::string file;
  auto find_file = [&file_list, &file, &path](const std::string &extension) {
    file = path.replace_extension(extension).string();
    if (boost::filesystem::exists(file))
      file_list.push_back(file);
  };

  // 暂时只支持shp文件
  find_file("shx");
  find_file("dbf");
  find_file("prj");
  find_file("qpj");
  find_file("qix");
  find_file("sbn");
  find_file("sbx");
  find_file("shp.xml");

  return file_list;
}
}
