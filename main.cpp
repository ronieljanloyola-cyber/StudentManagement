#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion"); // Clean cross-platform UI theme

    MainWindow window;
    window.show();

    return app.exec();
}