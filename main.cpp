#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create the single shared manager instance for the entire application
    Controller::ClinicManager sharedClinicManager;

    // Pass it into MainWindow so everything shares the same database and queue
    MainWindow mainWindow(sharedClinicManager);
    mainWindow.show();

    return app.exec();
}