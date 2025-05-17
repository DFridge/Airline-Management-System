#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "customerdialog.h"
#include "bookingdialog.h"
#include "ticketdialog.h"
#include "canceldialog.h"
#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_customerButton_clicked();
    void on_bookingButton_clicked();
    void on_viewTicketButton_clicked();
    void on_cancelButton_clicked();
    void on_exitButton_clicked();

private:
    Ui::MainWindow *ui;
    CustomerDialog *customerDialog;
    BookingDialog *bookingDialog;
    TicketDialog *ticketDialog;
    CancelDialog *cancelDialog;
    DatabaseManager *dbManager;
    int currentCustomerId;
    int currentBookingId;
};

#endif
