#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <mysql.h>
#include "Config.h"

class Database {
private:
    MYSQL* conn;
public:
    Database() : conn(nullptr) {}

    bool connect(const Config& config) {
        conn = mysql_init(nullptr);
        if (!conn) {
            std::cerr << "MySQL init failed\n";
            return false;
        }
        if (!mysql_real_connect(conn, config.host.c_str(), config.user.c_str(),
                                config.password.c_str(), config.dbname.c_str(),
                                config.port, nullptr, 0)) {
            std::cerr << "MySQL connect error: " << mysql_error(conn) << "\n";
            mysql_close(conn);
            conn = nullptr;
            return false;
        }
        return true;
    }

    ~Database() {
        if (conn) mysql_close(conn);
    }

    MYSQL* getConn() { return conn; }
    bool isConnected() { return conn != nullptr; }

    bool query(const std::string& q) {
        if (!conn) return false;
        return mysql_query(conn, q.c_str()) == 0;
    }

    MYSQL_RES* execQuery(const std::string& q) {
        if (!conn) return nullptr;
        if (mysql_query(conn, q.c_str()) != 0) {
            std::cerr << "Query error: " << mysql_error(conn) << "\n  SQL: " << q << "\n";
            return nullptr;
        }
        return mysql_store_result(conn);
    }

    int getLastInsertId() {
        if (!conn) return 0;
        return mysql_insert_id(conn);
    }

    std::string escape(const std::string& s) {
        if (!conn) return s;
        std::vector<char> buf(s.length() * 2 + 1);
        mysql_real_escape_string(conn, buf.data(), s.c_str(), s.length());
        return std::string(buf.data());
    }

    bool beginTransaction() {
        return query("START TRANSACTION");
    }

    bool commit() {
        return query("COMMIT");
    }

    bool rollback() {
        return query("ROLLBACK");
    }

    // Prepared statement helpers
    MYSQL_STMT* prepareStmt(const std::string& query) {
        if (!conn) return nullptr;
        MYSQL_STMT* stmt = mysql_stmt_init(conn);
        if (!stmt) return nullptr;
        if (mysql_stmt_prepare(stmt, query.c_str(), query.length()) != 0) {
            std::cerr << "Stmt prepare error: " << mysql_stmt_error(stmt) << "\n  SQL: " << query << "\n";
            mysql_stmt_close(stmt);
            return nullptr;
        }
        return stmt;
    }

    bool bindParamInt(MYSQL_BIND& bind, int& value) {
        memset(&bind, 0, sizeof(MYSQL_BIND));
        bind.buffer_type = MYSQL_TYPE_LONG;
        bind.buffer = &value;
        bind.is_unsigned = false;
        return true;
    }

    bool bindParamString(MYSQL_BIND& bind, std::string& value) {
        memset(&bind, 0, sizeof(MYSQL_BIND));
        bind.buffer_type = MYSQL_TYPE_STRING;
        bind.buffer = (void*)value.c_str();
        bind.buffer_length = value.length();
        return true;
    }
};
