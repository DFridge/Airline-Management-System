#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , customerDialog(nullptr)
    , bookingDialog(nullptr)
    , ticketDialog(nullptr)
    , cancelDialog(nullptr)
    , dbManager(new DatabaseManager(this))
    , currentCustomerId(0)
    , currentBookingId(0)
{
    ui->setupUi(this);
    setWindowTitle("FLYNOW - Airline Reservation System");
    setFixedSize(800, 600);

    // Initialize database
    if (!dbManager->initialize()) {
        QMessageBox::critical(this, "Error", "Failed to initialize database");
    }

    setStyleSheet("QMainWindow { background-color: #f8f9fa; }"
                  "QPushButton { background-color: #3a7bd5; color: white; border-radius: 5px; padding: 8px; min-width: 120px; }"
                  "QPushButton:hover { background-color: #00d2ff; }"
                  "QLabel { color: #333333; font-size: 14px; }");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_customerButton_clicked()
{
    customerDialog = new CustomerDialog(this);
    if (customerDialog->exec() == QDialog::Accepted) {
        currentCustomerId = customerDialog->getCustomerId();
        QMessageBox::information(this, "Success", "Customer details saved successfully!");
    }
    delete customerDialog;
}

void MainWindow::on_bookingButton_clicked()
{
    if (currentCustomerId == 0) {
        QMessageBox::warning(this, "Error", "Please enter customer details first");
        return;
    }

    bookingDialog = new BookingDialog(this);
    bookingDialog->setCustomerId(currentCustomerId);
    
    if (bookingDialog->exec() == QDialog::Accepted) {
        currentBookingId = bookingDialog->getBookingId();
        QMessageBox::information(this, "Success", "Flight booked successfully!");
    }
    delete bookingDialog;
}

void MainWindow::on_viewTicketButton_clicked()
{
    ticketDialog = new TicketDialog(this);
    ticketDialog->exec();
    delete ticketDialog;
}

void MainWindow::on_cancelButton_clicked()
{
    cancelDialog = new CancelDialog(this);
    if (cancelDialog->exec() == QDialog::Accepted) {
        currentBookingId = 0; // Reset current booking ID after successful cancellation
    }
    delete cancelDialog;
}

void MainWindow::on_exitButton_clicked()
{
    close();
}
