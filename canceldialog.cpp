#include "canceldialog.h"
#include "ui_canceldialog.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>

CancelDialog::CancelDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CancelDialog)
    , m_cnicValidator(new QRegularExpressionValidator(QRegularExpression("^\\d{13}$"), this))
    , dbManager(new DatabaseManager(this))
{
    ui->setupUi(this);
    setWindowTitle("Cancel Booking");
    setFixedSize(350, 200);

    // Initialize database
    if (!dbManager->initialize()) {
        QMessageBox::critical(this, "Error", "Failed to initialize database connection");
    }

    // Set up validator for CNIC input
    ui->cnicEdit->setValidator(m_cnicValidator);

    // Set up booking ID validator (assuming booking IDs are 1-6 digits)
    ui->bookingIdEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d{1,6}$"), this));

    // Connect button
    connect(ui->cancelButton, &QPushButton::clicked, this, &CancelDialog::handleCancellation);
}

CancelDialog::~CancelDialog()
{
    delete ui;
}

void CancelDialog::handleCancellation()
{
    QString cnic = ui->cnicEdit->text().trimmed();
    QString bookingIdStr = ui->bookingIdEdit->text().trimmed();

    if(cnic.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter your CNIC number");
        return;
    }

    if(cnic.length() != 13) {
        QMessageBox::warning(this, "Error", "CNIC must be exactly 13 digits");
        return;
    }

    if(bookingIdStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter your booking ID");
        return;
    }

    int bookingId = bookingIdStr.toInt();
    if(bookingId <= 0) {
        QMessageBox::warning(this, "Error", "Invalid booking ID");
        return;
    }

    // Verify booking ownership
    if (!dbManager->verifyBookingOwnership(bookingId, cnic)) {
        QMessageBox::warning(this, "Error", "No booking found with this ID and CNIC combination");
        return;
    }

    int reply = QMessageBox::question(this, "Confirm Cancellation",
                                    "Are you sure you want to cancel this booking?",
                                    QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes) {
        if (dbManager->cancelBooking(bookingId)) {
            QMessageBox::information(this, "Success", "Booking has been cancelled successfully");
            accept();
        } else {
            QMessageBox::critical(this, "Error", "Failed to cancel booking. The booking might already be cancelled.");
        }
    }
}
