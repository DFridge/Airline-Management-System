#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initialize();
    bool createTables();

    // Customer operations
    bool addCustomer(const QString& name, const QString& cnic, const QString& gender,
                    const QString& phone, const QString& address);
    bool updateCustomer(int id, const QString& name, const QString& cnic, const QString& phone);
    bool deleteCustomer(int id);
    bool doesCNICExist(const QString& cnic);
    QSqlQuery getCustomer(int id);
    QSqlQuery getCustomerByCNIC(const QString& cnic);
    QSqlQuery getAllCustomers();

    // Booking operations
    bool addBooking(int customerId, const QString& flightNumber, const QDateTime& departureTime,
                   const QString& source, const QString& destination, double price);
    bool hasActiveBooking(const QString& cnic);
    bool cancelBooking(int bookingId);
    QSqlQuery getBooking(int bookingId);
    QSqlQuery getBookingsByCNIC(const QString& cnic);
    QSqlQuery getAllBookings();
    bool verifyBookingOwnership(int bookingId, const QString& cnic);

private:
    QSqlDatabase db;
    bool createCustomerTable();
    bool createBookingTable();
};

#endif
