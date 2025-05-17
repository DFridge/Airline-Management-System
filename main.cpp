#include "mainwindow.h"
#include "login.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create and show login dialog
    Login loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        // User either cancelled or failed to login
        return 0;
    }

    // If login successful, show main window
    MainWindow w;
    w.show();

    return a.exec();
}
