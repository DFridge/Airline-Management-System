#ifndef BOOKINGDIALOG_H
#define BOOKINGDIALOG_H

#include <QDialog>
#include <QDateTime>
#include "databasemanager.h"

namespace Ui {
class BookingDialog;
}

class BookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookingDialog(QWidget *parent = nullptr);
    ~BookingDialog();

    void setCustomerId(int id) { customerId = id; }
    QString getFlightNumber() const { return flightNumber; }
    QDateTime getDepartureTime() const { return departureTime; }
    QString getSource() const { return source; }
    QString getDestination() const { return destination; }
    double getPrice() const { return charges; }
    int getBookingId() const { return bookingId; }

signals:
    void bookingComplete();

private slots:
    void on_searchButton_clicked();
    void on_bookButton_clicked();

private:
    Ui::BookingDialog *ui;
    int customerId;
    int bookingId;
    QString flightNumber;
    QDateTime departureTime;
    QString source;
    QString destination;
    double charges;
    DatabaseManager *dbManager;
};

#endif
