#include "message_box.h"

#ifdef LEMON_HAVE_KF5
#include <kmessagebox.h>
#else
#include <QMessageBox>
#endif

namespace lemon
{
namespace message_box
{
void critical(QWidget *parent, const QString &title, const QString &text)
{
#ifdef LEMON_HAVE_KF5
    KMessageBox::error(parent, text, title);
#else
    QMessageBox::critical(parent, title, text);
#endif
}
}
}
