#ifndef TICKETDIALOG_H
#define TICKETDIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include "databasemanager.h"

namespace Ui {
class TicketDialog;
}

class TicketDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TicketDialog(QWidget *parent = nullptr);
    ~TicketDialog();

    void setBookingId(int id) { bookingId = id; }
    void loadTicket();
    void loadTicketsByCNIC(const QString& cnic);

private slots:
    void on_mainMenuButton_clicked();
    void on_searchButton_clicked();

private:
    Ui::TicketDialog *ui;
    int bookingId;
    DatabaseManager *dbManager;
    void displayBooking(const QSqlQuery& bookingQuery, const QSqlQuery& customerQuery);
};

#endif
