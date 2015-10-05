cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(Boost_USE_STATIC_RUNTIME OFF)

project(lemon_image)

find_package(Qt5Widgets REQUIRED)
find_package(GDAL REQUIRED)
find_package(Boost 1.58 COMPONENTS system filesystem REQUIRED)
find_package(KF5WidgetsAddons)

find_package(OpenMP)

qt5_add_resources(RC_SRCS lemon_image.qrc)

add_executable(lemon_image
               main.cc
               main_window.cc
               scene_2d.cc
               view_2d.cc
               catalog_view.cc
               layer_base.cc
               raster_layer.cc
               vector_layer.cc
               raster.cc
               vector.cc
               utility.cc
               spatial_ref.cc
               algorithm.cc
               file_dialog.cc
               message_box.cc
               metadata_dialog.cc
               general_tab.cc
               spatial_reference_tab.cc
               metadata_tab.cc
               change_rgb_bands_dialog.cc
               ${RC_SRCS})

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if(Qt5Widgets_FOUND)
    add_definitions(-DLEMON_HAVE_QT)
endif()

if(KF5WidgetsAddons_FOUND)
    add_definitions(-DLEMON_HAVE_KF5)
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GDAL_INCLUDE_DIR})

qt5_use_modules(lemon_image Widgets)

target_link_libraries(lemon_image ${Boost_LIBRARIES})
target_link_libraries(lemon_image ${GDAL_LIBRARY})
target_link_libraries(lemon_image KF5::WidgetsAddons)
              
install(TARGETS lemon_image RUNTIME DESTINATION bin)
