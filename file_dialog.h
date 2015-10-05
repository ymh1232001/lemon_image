#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <QString>

QT_BEGIN_NAMESPACE
class QWidget;
class QStringList;
QT_END_NAMESPACE

namespace lemon
{
    class file_dialog
    {
    public:
        static QString get_open_raster_name(QWidget *parent = nullptr, const QString &caption = QString());
        static QString get_open_vector_name(QWidget *parent = nullptr, const QString &caption = QString());
        static QStringList get_open_raster_names(QWidget *parent = nullptr, const QString &caption = QString());
        static QStringList get_open_vector_names(QWidget *parent = nullptr, const QString &caption = QString());
        
        inline static QString last_directory() { return last_directory_; }
        inline static void set_last_directory(const QString &directory) { last_directory_ = directory; }
        
    private:
        static QString last_directory_;
    };
}

#endif // FILE_DIALOG_H
