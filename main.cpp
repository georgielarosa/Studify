#include <QApplication>
#include <QCoreApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
