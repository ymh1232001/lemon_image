#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>

#ifdef LEMON_HAVE_QT
#include <QVector>
#include <QRgb>

#include "raster.h"

class QString;
#else
namespace lemon {
class raster_metadata;
}
#endif

namespace lemon {
#ifdef LEMON_HAVE_QT
QString to_qstring(const std::string &str);
std::string to_string(const QString &q_str);

QVector<QRgb> to_qt_color_table(std::vector<lemon::color_entry> color_table);

std::vector<QString> gen_band_list(const lemon::raster_metadata *metadata);
#endif

std::string get_filename(const std::string &pathname);
std::string get_filename_without_extension(const std::string &pathname);
std::uintmax_t get_filesize(const std::string &pathname);

std::string gen_file_size_string(const std::uintmax_t &num_bytes);

std::vector<std::string> get_vector_pathname_list(const std::string &pathname);

template <typename T> class point_2d {
public:
  point_2d() {}
  point_2d(T x, T y) : x_(x), y_(y) {}
  point_2d(const point_2d &point) {
    x_ = point.x();
    y_ = point.y();
  }

  inline void set_x(T x) { x_ = x; }
  inline void set_y(T y) { y_ = y; }
  inline T x() const { return x_; }
  inline T y() const { return y_; }

  inline void reset(T x, T y) {
    x_ = x;
    y_ = y;
  }

  inline void reset(const point_2d &point) {
    x_ = point.x();
    y_ = point.y();
  }

  inline void offset(T x_offset, T y_offset) {
    x_ += x_offset;
    y_ += y_offset;
  }

protected:
  T x_;
  T y_;
};
}

#endif // UTILITY_H
