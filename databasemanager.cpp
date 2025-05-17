#include "databasemanager.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::initialize()
{
    if(QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

    QString appDir = QCoreApplication::applicationDirPath();
    QString dbPath = QDir(appDir).filePath("airline.db");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Error: Failed to connect to database at" << dbPath;
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    qDebug() << "Successfully connected to database at" << dbPath;
    return createTables();
}

bool DatabaseManager::createTables()
{
    bool customersCreated = createCustomerTable();
    if (!customersCreated) {
        qDebug() << "Failed to create customers table:" << db.lastError().text();
        return false;
    }

    bool bookingsCreated = createBookingTable();
    if (!bookingsCreated) {
        qDebug() << "Failed to create bookings table:" << db.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::createCustomerTable()
{
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS customers ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "cnic TEXT NOT NULL,"
        "gender TEXT NOT NULL,"
        "phone TEXT NOT NULL,"
        "address TEXT"
        ")"
    );
}

bool DatabaseManager::createBookingTable()
{
    QSqlQuery query;
    return query.exec(
        "CREATE TABLE IF NOT EXISTS bookings ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "customer_id INTEGER NOT NULL,"
        "flight_number TEXT NOT NULL,"
        "departure_time DATETIME NOT NULL,"
        "source TEXT NOT NULL,"
        "destination TEXT NOT NULL,"
        "price REAL NOT NULL,"
        "status TEXT DEFAULT 'active',"
        "FOREIGN KEY(customer_id) REFERENCES customers(id)"
        ")"
    );
}

bool DatabaseManager::addCustomer(const QString& name, const QString& cnic, const QString& gender,
                                const QString& phone, const QString& address)
{
    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO customers (name, cnic, gender, phone, address) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(name.trimmed());
    query.addBindValue(cnic.trimmed());
    query.addBindValue(gender.trimmed());
    query.addBindValue(phone.trimmed());
    query.addBindValue(address.trimmed());
    
    if (!query.exec()) {
        qDebug() << "Error adding customer:";
        qDebug() << "Query error:" << query.lastError().text();
        qDebug() << "Database error:" << db.lastError().text();
        qDebug() << "Values:" << name << cnic << gender << phone;
        return false;
    }
    
    if (query.numRowsAffected() <= 0) {
        qDebug() << "Error: No rows were inserted";
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateCustomer(int id, const QString& name, const QString& cnic, const QString& phone)
{
    QSqlQuery query;
    query.prepare("UPDATE customers SET name = ?, cnic = ?, phone = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(cnic);
    query.addBindValue(phone);
    query.addBindValue(id);
    return query.exec();
}

bool DatabaseManager::deleteCustomer(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM customers WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

QSqlQuery DatabaseManager::getCustomer(int id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM customers WHERE id = ?");
    query.addBindValue(id);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getAllCustomers()
{
    QSqlQuery query("SELECT * FROM customers");
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getCustomerByCNIC(const QString& cnic)
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM customers WHERE cnic = ?");
    query.addBindValue(cnic);
    query.exec();
    return query;
}

bool DatabaseManager::hasActiveBooking(const QString& cnic)
{
    QSqlQuery query(db);
    query.prepare("SELECT b.* FROM bookings b "
                 "JOIN customers c ON b.customer_id = c.id "
                 "WHERE c.cnic = ? AND b.status = 'active'");
    query.addBindValue(cnic);
    
    if (!query.exec()) {
        qDebug() << "Error checking active bookings:";
        qDebug() << "Query error:" << query.lastError().text();
        return false;
    }
    
    return query.next(); // Returns true if there's at least one active booking
}

bool DatabaseManager::addBooking(int customerId, const QString& flightNumber, const QDateTime& departureTime,
                               const QString& source, const QString& destination, double price)
{
    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open";
        return false;
    }

    // Get customer CNIC
    QSqlQuery customerQuery(db);
    customerQuery.prepare("SELECT cnic FROM customers WHERE id = ?");
    customerQuery.addBindValue(customerId);
    if (!customerQuery.exec() || !customerQuery.next()) {
        qDebug() << "Error: Could not find customer";
        return false;
    }
    QString cnic = customerQuery.value("cnic").toString();

    // Check if customer already has an active booking
    if (hasActiveBooking(cnic)) {
        qDebug() << "Error: Customer with CNIC" << cnic << "already has an active booking";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO bookings (customer_id, flight_number, departure_time, source, destination, price, status) "
                 "VALUES (?, ?, ?, ?, ?, ?, 'active')");
    query.addBindValue(customerId);
    query.addBindValue(flightNumber);
    query.addBindValue(departureTime);
    query.addBindValue(source);
    query.addBindValue(destination);
    query.addBindValue(price);

    if (!query.exec()) {
        qDebug() << "Error adding booking:";
        qDebug() << "Query error:" << query.lastError().text();
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseManager::cancelBooking(int bookingId)
{
    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open";
        return false;
    }

    // First check if the booking exists and its current status
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT status FROM bookings WHERE id = ?");
    checkQuery.addBindValue(bookingId);
    
    if (!checkQuery.exec()) {
        qDebug() << "Error checking booking status:" << checkQuery.lastError().text();
        return false;
    }

    if (!checkQuery.next()) {
        qDebug() << "Error: Booking ID" << bookingId << "not found";
        return false;
    }

    QString currentStatus = checkQuery.value("status").toString();
    if (currentStatus == "cancelled") {
        qDebug() << "Error: Booking is already cancelled";
        return false;
    }

    // Now proceed with cancellation
    QSqlQuery cancelQuery(db);
    cancelQuery.prepare("UPDATE bookings SET status = 'cancelled' WHERE id = ?");
    cancelQuery.addBindValue(bookingId);
    
    if (!cancelQuery.exec()) {
        qDebug() << "Error cancelling booking:" << cancelQuery.lastError().text();
        return false;
    }

    if (cancelQuery.numRowsAffected() <= 0) {
        qDebug() << "Error: No booking was updated";
        return false;
    }

    return true;
}

QSqlQuery DatabaseManager::getBooking(int bookingId)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM bookings WHERE id = ?");
    query.addBindValue(bookingId);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getBookingsByCNIC(const QString& cnic)
{
    QSqlQuery query(db);
    query.prepare("SELECT b.* FROM bookings b "
                 "JOIN customers c ON b.customer_id = c.id "
                 "WHERE c.cnic = ? "
                 "ORDER BY b.departure_time DESC");
    query.addBindValue(cnic);
    query.exec();
    return query;
}

QSqlQuery DatabaseManager::getAllBookings()
{
    QSqlQuery query("SELECT * FROM bookings ORDER BY departure_time DESC");
    query.exec();
    return query;
}

bool DatabaseManager::doesCNICExist(const QString& cnic)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM customers WHERE cnic = ?");
    query.addBindValue(cnic);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::verifyBookingOwnership(int bookingId, const QString& cnic)
{
    if (!db.isOpen()) {
        qDebug() << "Error: Database is not open";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT b.id FROM bookings b "
                 "JOIN customers c ON b.customer_id = c.id "
                 "WHERE b.id = ? AND c.cnic = ?");
    query.addBindValue(bookingId);
    query.addBindValue(cnic);

    if (!query.exec()) {
        qDebug() << "Error verifying booking ownership:" << query.lastError().text();
        return false;
    }

    return query.next(); // Returns true if a matching record was found
} 