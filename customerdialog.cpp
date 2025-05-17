#include "customerdialog.h"
#include "ui_customerdialog.h"
#include <QMessageBox>

CustomerDialog::CustomerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CustomerDialog)
    , customerId(0)
{
    ui->setupUi(this);
    setWindowTitle("Customer Details");
    setFixedSize(400, 450);

    dbManager = new DatabaseManager(this);
    if (!dbManager->initialize()) {
        QMessageBox::critical(this, "Error", "Failed to initialize database");
    }

    setStyleSheet("QDialog { background-color: #f8f9fa; }"
                  "QLabel { color: #333333; }"
                  "QPushButton { background-color: #3a7bd5; color: white; border-radius: 5px; padding: 8px; }"
                  "QPushButton:hover { background-color: #00d2ff; }"
                  "QLineEdit, QComboBox, QTextEdit { padding: 6px; border: 1px solid #ddd; border-radius: 4px; }");
}

CustomerDialog::~CustomerDialog()
{
    delete ui;
}

void CustomerDialog::on_saveButton_clicked()
{
    name = ui->nameEdit->text();
    gender = ui->genderComboBox->currentText();
    phone = ui->phoneEdit->text();
    address = ui->addressEdit->toPlainText();
    cnic = ui->cnicEdit->text();

    if(name.isEmpty() || cnic.isEmpty() || phone.isEmpty() || gender.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all required fields");
        return;
    }

    // Save customer to database
    if (dbManager->addCustomer(name, cnic, gender, phone, address)) {
        // Get the last inserted customer ID
        QSqlQuery query = dbManager->getAllCustomers();
        while (query.next()) {
            customerId = query.value("id").toInt();
        }
        QMessageBox::information(this, "Success", "Customer details saved successfully!");
        accept();
    } else {
        QMessageBox::critical(this, "Error", "Failed to save customer details. Please try again.");
    }
}
