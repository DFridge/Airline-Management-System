#ifndef CUSTOMERDIALOG_H
#define CUSTOMERDIALOG_H

#include <QDialog>
#include "databasemanager.h"

namespace Ui {
class CustomerDialog;
}

class CustomerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomerDialog(QWidget *parent = nullptr);
    ~CustomerDialog();

    QString getName() const { return name; }
    QString getGender() const { return gender; }
    QString getPhone() const { return phone; }
    QString getAddress() const { return address; }
    QString getCnic() const { return cnic; }
    int getCustomerId() const { return customerId; }

private slots:
    void on_saveButton_clicked();

private:
    Ui::CustomerDialog *ui;
    QString name;
    QString gender;
    QString phone;
    QString address;
    QString cnic;
    int customerId;
    DatabaseManager *dbManager;
};

#endif
