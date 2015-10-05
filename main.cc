#include <QApplication>
// #include <QStyleFactory>

#include "main_window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

//  // dark theme
//  app.setStyle(QStyleFactory::create("Fusion"));
//  QPalette dark_palette;
//  dark_palette.setColor(QPalette::Window, QColor(53, 53, 53));
//  dark_palette.setColor(QPalette::WindowText, Qt::white);
//  dark_palette.setColor(QPalette::Base, QColor(25, 25, 25));
//  dark_palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
//  dark_palette.setColor(QPalette::ToolTipBase, Qt::white);
//  dark_palette.setColor(QPalette::ToolTipText, Qt::white);
//  dark_palette.setColor(QPalette::Text, Qt::white);
//  dark_palette.setColor(QPalette::Button, QColor(53, 53, 53));
//  dark_palette.setColor(QPalette::ButtonText, Qt::white);
//  dark_palette.setColor(QPalette::BrightText, Qt::red);
//  dark_palette.setColor(QPalette::Link, QColor(42, 130, 218));
//  dark_palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
//  dark_palette.setColor(QPalette::HighlightedText, Qt::black);

//  app.setPalette(dark_palette);
//  app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; "
//                    "border: 1px solid white; }");

  app.setApplicationName("Lemon Image");
  app.setOrganizationName("Lemon");
  app.setApplicationVersion("0.1");
  app.setWindowIcon(QIcon(":/lemon.svg"));

  main_window window;
  window.showMaximized();

  return app.exec();
}
