#ifndef CANCELDIALOG_H
#define CANCELDIALOG_H

#include <QDialog>
#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CancelDialog; }
QT_END_NAMESPACE

class QRegularExpressionValidator;

class CancelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CancelDialog(QWidget *parent = nullptr);
    ~CancelDialog();

signals:
    void bookingCancelled(int bookingId);

private slots:
    void handleCancellation();

private:
    Ui::CancelDialog *ui;
    QRegularExpressionValidator *m_cnicValidator;
    DatabaseManager *dbManager;
};
#endif // CANCELDIALOG_H
