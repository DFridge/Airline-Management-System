#include "ticketdialog.h"
#include "ui_ticketdialog.h"
#include <QMessageBox>
#include <QDateTime>
#include <QRegularExpressionValidator>
#include <QInputDialog>

TicketDialog::TicketDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TicketDialog)
    , bookingId(0)
{
    ui->setupUi(this);
    setWindowTitle("View Ticket");
    setFixedSize(400, 600);

    dbManager = new DatabaseManager(this);
    if (!dbManager->initialize()) {
        QMessageBox::critical(this, "Error", "Failed to initialize database");
    }

    // Set up CNIC validator
    ui->cnicEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d{13}$"), this));

    setStyleSheet("QDialog { background-color: #f8f9fa; }"
                  "QLabel { color: #333333; }"
                  "QPushButton { background-color: #3a7bd5; color: white; border-radius: 5px; padding: 8px; }"
                  "QPushButton:hover { background-color: #00d2ff; }"
                  "QFrame { border: 2px solid #3a7bd5; border-radius: 8px; padding: 16px; }"
                  "QLineEdit { padding: 6px; border: 1px solid #ddd; border-radius: 4px; }"
                  "QLabel { padding: 4px; }");
}

TicketDialog::~TicketDialog()
{
    delete ui;
}

void TicketDialog::loadTicket()
{
    if (bookingId == 0) {
        QMessageBox::warning(this, "Error", "No booking ID provided");
        return;
    }

    // Get booking details
    QSqlQuery bookingQuery = dbManager->getBooking(bookingId);
    if (!bookingQuery.next()) {
        QMessageBox::warning(this, "Error", "Booking not found");
        return;
    }

    // Get customer details
    int customerId = bookingQuery.value("customer_id").toInt();
    QSqlQuery customerQuery = dbManager->getCustomer(customerId);
    if (!customerQuery.next()) {
        QMessageBox::warning(this, "Error", "Customer details not found");
        return;
    }

    displayBooking(bookingQuery, customerQuery);
}

void TicketDialog::loadTicketsByCNIC(const QString& cnic)
{
    QSqlQuery customerQuery = dbManager->getCustomerByCNIC(cnic);
    if (!customerQuery.next()) {
        QMessageBox::warning(this, "Error", "No customer found with this CNIC");
        return;
    }

    QSqlQuery bookingQuery = dbManager->getBookingsByCNIC(cnic);
    if (!bookingQuery.next()) {
        QMessageBox::warning(this, "Error", "No bookings found for this CNIC");
        return;
    }

    // Create a list to store all bookings
    QStringList bookings;
    do {
        QString bookingInfo = QString("Ticket #%1 - Flight %2 - %3 to %4 - %5")
            .arg(bookingQuery.value("id").toString())
            .arg(bookingQuery.value("flight_number").toString())
            .arg(bookingQuery.value("source").toString())
            .arg(bookingQuery.value("destination").toString())
            .arg(bookingQuery.value("departure_time").toDateTime().toString("dd-MM-yyyy HH:mm"));
        bookings.append(bookingInfo);
    } while (bookingQuery.next());

    // If there are multiple bookings, let user choose which one to view
    bool ok;
    QString selectedBooking;
    if (bookings.size() > 1) {
        selectedBooking = QInputDialog::getItem(this, "Select Booking",
            "Multiple bookings found. Please select one:", bookings, 0, false, &ok);
        if (!ok) {
            return;
        }
    } else {
        selectedBooking = bookings.first();
    }

    // Extract booking ID from selected booking and reload that specific booking
    QString bookingId = selectedBooking.split("#").at(1).split(" ").first();
    bookingQuery = dbManager->getBooking(bookingId.toInt());
    bookingQuery.next();
    
    // Reset customer query to first record since we moved through it
    customerQuery = dbManager->getCustomerByCNIC(cnic);
    customerQuery.next();
    
    displayBooking(bookingQuery, customerQuery);
}

void TicketDialog::displayBooking(const QSqlQuery& bookingQuery, const QSqlQuery& customerQuery)
{
    // Display ticket information
    ui->ticketNumberLabel->setText("Ticket #" + bookingQuery.value("id").toString());
    ui->passengerNameLabel->setText(customerQuery.value("name").toString());
    ui->cnicDisplayLabel->setText(customerQuery.value("cnic").toString());
    ui->flightNumberLabel->setText(bookingQuery.value("flight_number").toString());
    ui->sourceLabel->setText(bookingQuery.value("source").toString());
    ui->destinationLabel->setText(bookingQuery.value("destination").toString());
    
    QDateTime departureTime = bookingQuery.value("departure_time").toDateTime();
    ui->dateLabel->setText(departureTime.toString("dd-MM-yyyy"));
    ui->timeLabel->setText(departureTime.toString("HH:mm"));
    
    ui->priceLabel->setText("Rs. " + QString::number(bookingQuery.value("price").toDouble()));
    ui->statusLabel->setText(bookingQuery.value("status").toString());
}

void TicketDialog::on_searchButton_clicked()
{
    QString cnic = ui->cnicEdit->text().trimmed();
    
    if (cnic.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter CNIC number");
        return;
    }

    if (cnic.length() != 13) {
        QMessageBox::warning(this, "Error", "CNIC must be exactly 13 digits");
        return;
    }

    loadTicketsByCNIC(cnic);
}

void TicketDialog::on_mainMenuButton_clicked()
{
    accept();
}
