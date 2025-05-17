#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QPixmap>

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("Login System");




    QPixmap logo(":/Images/360_F_20136083_gk0ppzak6UdK9PcDRgPdLjcuAdo7o1LK.jpg");
    if(!logo.isNull()) {
        QPixmap scaledLogo = logo.scaled(500, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->imageLabel->setPixmap(scaledLogo);
    } else {
        ui->imageLabel->setText("Logo Not Found");
        ui->imageLabel->setStyleSheet("color: red; font-size: 16px;");
    }


    // Set styles
    setStyleSheet(
        "QDialog { background-color: #f5f9ff; }"
        "QLabel#titleLabel {"
        "   color: #003366;"
        "   font-family: Segoe UI;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "}"
        "QLabel {"
        "   color: #003366;"
        "   font-family: Segoe UI;"
        "   font-size: 14px;"
        "}"
        "QLineEdit {"
        "   border: 1px solid #a3c1ff;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   background: white;"
        "   min-width: 200px;"
        "}"
        "QPushButton {"
        "   background-color: #4a90e2;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "   min-width: 100px;"
        "}"
        "QPushButton:hover { background-color: #3a7bc8; }"
        "QWidget#loginContainer {"
        "   background-color: #f5f9ff;"
        "   padding: 20px;"
        "}"
        );
}

Login::~Login()
{
    delete ui;
}

void Login::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter both username and password");
        return;
    }

    if (username == "admin" && password == "admin") {
        accept();
    } else {
        QMessageBox::critical(this, "Error", "Invalid credentials");
        ui->passwordEdit->clear();
        ui->passwordEdit->setFocus();
    }
}
