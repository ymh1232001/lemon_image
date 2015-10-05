#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <QString>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace lemon
{
    namespace message_box 
    {
        void critical(QWidget *parent, const QString &title, const QString &text);
    }
}

#endif // MESSAGE_BOX_H
