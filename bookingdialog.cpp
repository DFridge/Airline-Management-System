#include "bookingdialog.h"
#include "ui_bookingdialog.h"
#include <QMessageBox>
#include <QDateTime>
#include <QLocale>

BookingDialog::BookingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BookingDialog)
    , customerId(0)
    , bookingId(0)
{
    ui->setupUi(this);
    setWindowTitle("Flight Booking");

    dbManager = new DatabaseManager(this);
    if (!dbManager->initialize()) {
        QMessageBox::critical(this, "Error", "Failed to initialize database");
    }

    setStyleSheet("QDialog { background-color: #f8f9fa; }"
                  "QLabel { color: #333333; }"
                  "QPushButton { background-color: #3a7bd5; color: white; border-radius: 5px; padding: 8px; }"
                  "QPushButton:hover { background-color: #00d2ff; }"
                  "QLineEdit, QComboBox, QDateEdit, QTimeEdit { padding: 6px; border: 1px solid #ddd; border-radius: 4px; }"
                  "QTableWidget { background-color: white; gridline-color: #ddd; }"
                  "QHeaderView::section { background-color: #3a7bd5; color: white; }");

    // Populate departure cities
    QStringList departures = {"Islamabad", "Karachi", "Lahore", "Peshawar", "Quetta"};
    ui->departureComboBox->addItems(departures);

    // Populate destinations
    QStringList destinations = {"Dubai", "UK", "USA", "Australia", "Europe", "Saudi Arabia", "China"};
    ui->destinationComboBox->addItems(destinations);

    // Set current date and time
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());
}

BookingDialog::~BookingDialog()
{
    delete ui;
}

void BookingDialog::on_searchButton_clicked()
{
    source = ui->departureComboBox->currentText();
    destination = ui->destinationComboBox->currentText();
    QDate date = ui->dateEdit->date();
    QTime time = ui->timeEdit->time();
    departureTime = QDateTime(date, time);

    if(customerId == 0) {
        QMessageBox::warning(this, "Error", "Please enter customer details first");
        return;
    }

    // Check if the customer's CNIC already has an active booking
    QSqlQuery customerQuery = dbManager->getCustomer(customerId);
    if (customerQuery.next()) {
        QString cnic = customerQuery.value("cnic").toString();
        if (dbManager->hasActiveBooking(cnic)) {
            QMessageBox::warning(this, "Booking Error", 
                "This CNIC already has an active booking.\nPlease cancel the existing booking before making a new one.");
            return;
        }
    }

    // Simulate flight search results
    ui->flightsTable->setRowCount(3);

    // Generate flight numbers
    QStringList flights = {
        QString("%1-%2").arg(source.left(3).toUpper()).arg("101"),
        QString("%1-%2").arg(source.left(3).toUpper()).arg("202"),
        QString("%1-%2").arg(source.left(3).toUpper()).arg("303")
    };

    QStringList times = {"08:00", "12:00", "18:00"};
    QStringList durations = {"2h 30m", "3h 15m", "2h 45m"};
    QStringList prices = {"102000", "62000", "180000"};

    for(int i = 0; i < 3; i++) {
        ui->flightsTable->setItem(i, 0, new QTableWidgetItem(flights[i]));
        ui->flightsTable->setItem(i, 1, new QTableWidgetItem(date.toString("dd-MM-yyyy")));
        ui->flightsTable->setItem(i, 2, new QTableWidgetItem(times[i]));
        ui->flightsTable->setItem(i, 3, new QTableWidgetItem(durations[i]));
        ui->flightsTable->setItem(i, 4, new QTableWidgetItem("Rs. " + prices[i]));
    }
}

void BookingDialog::on_bookButton_clicked()
{
    if(ui->flightsTable->currentRow() >= 0) {
        // Get the selected flight details
        flightNumber = ui->flightsTable->item(ui->flightsTable->currentRow(), 0)->text();
        
        // Get price and convert to float properly
        QString priceText = ui->flightsTable->item(ui->flightsTable->currentRow(), 4)->text();
        priceText.remove("Rs. "); // Remove the currency prefix
        bool ok;
        charges = priceText.toFloat(&ok);
        
        if (!ok) {
            QMessageBox::warning(this, "Error", "Invalid price format");
            return;
        }

        // Save booking to database
        if (dbManager->addBooking(customerId, flightNumber, departureTime, source, destination, charges)) {
            // Get the last inserted booking ID
            QSqlQuery query = dbManager->getAllBookings();
            while (query.next()) {
                bookingId = query.value("id").toInt();
            }
            QMessageBox::information(this, "Success", "Flight booked successfully!");
            emit bookingComplete();
            accept();
        } else {
            // Check if the failure is due to an existing booking
            QSqlQuery customerQuery = dbManager->getCustomer(customerId);
            if (customerQuery.next()) {
                QString cnic = customerQuery.value("cnic").toString();
                if (dbManager->hasActiveBooking(cnic)) {
                    QMessageBox::warning(this, "Booking Error", 
                        "This CNIC already has an active booking.\nPlease cancel the existing booking before making a new one.");
                    return;
                }
            }
            QMessageBox::critical(this, "Error", "Failed to save booking");
        }
    } else {
        QMessageBox::warning(this, "Error", "Please select a flight first");
    }
}
