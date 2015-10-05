#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <cstdint>

namespace lemon {
// TODO : 拉伸时忽略NoData值
template <typename T>
inline std::uint8_t linear_stretch(T data, double min, double max) {
  if (data < min)
    return 0;
  if (data > max)
    return 255;

  return static_cast<uint8_t>(255 * (data - min) /
                              (max == min ? 1 : (max - min)));
}

// TODO : 增加错误检查
template <typename T>
std::uint8_t *build_image_data(T *data_r, T *data_g, T *data_b, int width,
                               int height, double stretch_min_r,
                               double stretch_max_r, double stretch_min_g,
                               double stretch_max_g, double stretch_min_b,
                               double stretch_max_b) {
  int num_line_bytes = (width * 24 + 31) / 8;

  std::uint8_t *image_data = new std::uint8_t[num_line_bytes * height * 3];

#pragma omp parallel for
  for (int i = 0; i < height; ++i)
    for (int j = 0; j < width; ++j) {
      image_data[i * num_line_bytes + j * 3] =
          linear_stretch(data_r[i * width + j], stretch_min_r, stretch_max_r);
      image_data[i * num_line_bytes + j * 3 + 1] =
          linear_stretch(data_g[i * width + j], stretch_min_g, stretch_max_g);
      image_data[i * num_line_bytes + j * 3 + 2] =
          linear_stretch(data_b[i * width + j], stretch_min_b, stretch_max_b);
    }

  return image_data;
}

template <typename T>
std::uint8_t *build_image_data(bool color_table, T *data, int width, int height,
                               double stretch_min, double stretch_max) {
  int num_line_bytes = (width * 8 + 31) / 8;

  std::uint8_t *image_data = new std::uint8_t[num_line_bytes * height];

  if (color_table) {
#pragma omp parallel for
    for (int i = 0; i < height; ++i)
      for (int j = 0; j < width; ++j)
        image_data[i * num_line_bytes + j] = data[i * width + j];
  } else {
#pragma omp parallel for
    for (int i = 0; i < height; ++i)
      for (int j = 0; j < width; ++j)
        image_data[i * num_line_bytes + j] =
            linear_stretch(data[i * width + j], stretch_min, stretch_max);
  }

  return image_data;
}
}

#endif // ALGORITHM_H
