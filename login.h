#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_loginButton_clicked();

private:
    Ui::Login *ui;
    QLabel *imageLabel;
};

#endif
