#include "file_dialog.h"

#include <QDir>
#include <QFileDialog>
#include <QStringList>
#include <QListView>

namespace lemon
{
QString file_dialog::last_directory_ = QDir::homePath();

QStringList file_dialog::get_open_raster_names(QWidget *parent, const QString &caption)
{
    QFileDialog dialog(parent, caption, last_directory_, QObject::tr("所有基于文件的栅格格式 (*.bil *.bmp *.dat *.dem *.gif *.img *.j2k *.jp2 *.jpeg *.jpg *mem *.pix *.png *.tif *.tiff *.vrt *.xpm);;所有文件(*.*);;ENVI .hdr Labelled (*.bil *.dat);;Erdas Imagine (*.img);;GDAL Virtual (*.vrt);;Graphics Interchange Format (*.gif);;Japanese DEM (*.mem);;JPEG2000 (*.j2k *.jp2);;JPEG JFIF (*.jpeg *.jpg);;Microsoft Windows Device Independent Bitmap (*.bmp);;PCI Geomatics Database File (*.pix);;Portable Network Graphics (*.png);;TIFF / BigTIFF / GeoTIFF (*.tiff *.tif);;USGS ASCII DEM / CDED (*.dem);;X11 Pixmap (*.xpm)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (dialog.exec()) {
        last_directory_ = dialog.directory().absolutePath();
        return dialog.selectedFiles();
    } else {
        return QStringList();
    }
}

QStringList file_dialog::get_open_vector_names(QWidget *parent, const QString &caption)
{
    QFileDialog dialog(parent, caption, last_directory_, QObject::tr("所有基于文件的矢量格式 (*.dxf *.shp);;所有文件(*.*);;AutoCAD DXF (*.dxf);;ESRI Shapefile (*.shp)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    
    if (dialog.exec()) {
        last_directory_ = dialog.directory().absolutePath();
        return dialog.selectedFiles();
    } else {
        return QStringList();
    }
}
}

