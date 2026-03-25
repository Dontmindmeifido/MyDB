#include <iostream>
#include <vector>
#include <algorithm>
#include "00Database.cpp"

using namespace std;

class Connection {
protected:
    Database* database;

    Connection() {
        this->database = Database::getInstance();
    }

public:
    const Database* getConnection() {
        return this->database;
    }

    virtual ~Connection() {
        delete database;
    }
};

class Create : virtual public Connection {
public:
    void createTable(string tableName, vector<string> headers, vector<string> dataTypes) {
        Table* table = this->database->getTable(tableName);

        if (table != nullptr || headers.empty() || headers.empty()) return;

        vector<Table> databaseData = this->database->getDatabaseData();
        databaseData.push_back(Table(headers, dataTypes, tableName));
    
        this->database->setDatabaseData(databaseData);
    }
};

class Read : virtual public Connection {
public:
    Table* readTable(string tableName, vector<string> headers, vector<string> where, vector<string> orderby) {
        Table* table = this->database->getTable(tableName);

        if (table == nullptr || headers.empty()) return nullptr;

        // Get columns
        vector<Row> dummyrows = {Row(headers)};
        for (int i = 1; i < table->getTableData().size(); i++) {
            int column = 0;
            vector<string> rowdata;

            for (int j = 0; j < table->getTableData()[i].getRowData().size(); j++) { // Direction sensitive
                if (table->getTableData()[0].getRowData()[j].getValue() == headers[column]) {
                    rowdata.push_back(table->getTableData()[i].getRowData()[j].getValue());

                    if (column < headers.size() - 1) {
                        column++;
                    }
                }
            }

            if (!rowdata.empty()) {
                dummyrows.push_back(Row(rowdata));
            }
        }

        // Where Filter
        if (!where.empty()) {
            vector<Row> rowdata = {Row(headers)};

            string column = where[0];
            string operation = where[1];
            string comparator = where[2];

            int columnIndex = 0;
            for (int i = 0; i < table->getTableData()[0].getRowData().size(); i++) {
                if (table->getTableData()[0].getRowData()[i].getValue() == column) {
                    columnIndex = i;
                }
            }

            for (int i = 1; i < dummyrows.size(); i++) {
                if (operation == ">") {
                    if (table->getTableData()[i].getRowData()[columnIndex].getValue() > comparator) {
                        rowdata.push_back(dummyrows[i]);
                    }
                } else if (operation == "<") {
                    if (table->getTableData()[i].getRowData()[columnIndex].getValue() < comparator) {
                        rowdata.push_back(dummyrows[i]);
                    }
                } else if (operation == ">=") {
                    if (table->getTableData()[i].getRowData()[columnIndex].getValue() >= comparator) {
                        rowdata.push_back(dummyrows[i]);
                    }
                } else if (operation == "<=") {
                    if (table->getTableData()[i].getRowData()[columnIndex].getValue() <= comparator) {
                        rowdata.push_back(dummyrows[i]);
                    }
                } else if (operation == "=") {
                    if (table->getTableData()[i].getRowData()[columnIndex].getValue() == comparator) {
                        rowdata.push_back(dummyrows[i]);
                    }
                }

            }

            dummyrows = rowdata;
        }

        // Orderby Filter
        if (!orderby.empty()) {
            string column = orderby[0];
            string order = orderby[1];

            int columnIndex = 0;
            for (int i = 0; i < headers.size(); i++) {
                if (headers[i] == column) {
                    columnIndex = i;
                }
            }

            if (order == "descending") {
                sort(dummyrows.begin() + 1, dummyrows.end(), [&columnIndex](Row a, Row b) {return a.getRowData()[columnIndex].getValue() > b.getRowData()[columnIndex].getValue();});
            } else if (order == "ascending"){
                sort(dummyrows.begin() + 1, dummyrows.end(), [&columnIndex](Row a, Row b) {return a.getRowData()[columnIndex].getValue() < b.getRowData()[columnIndex].getValue();});
            }
        }

        Table* retTable = new Table(headers, {"ANY"}, "RESPONSE");
        retTable->setTableData(dummyrows);

        return retTable;
    }
};

class Update : virtual public Connection {
public:
    void addRow(string tableName, vector<string> row) {
        Table* table = this->database->getTable(tableName);

        if (table == nullptr || row.empty()) return;

        vector<Row> tableData = table->getTableData();
        tableData.push_back(Row(row));

        table->setTableData(tableData);
    }
};

class Delete : virtual public Connection {
public:
    void deleteRow(string tableName, string row) {
        Table* table = this->database->getTable(tableName);

        if (table == nullptr) return;

        if (row.empty()) row = "0";

        if (table->getTableData().size() >= 2) {
            vector<Row> dummy;
            for (int i = 0; i < table->getTableData().size(); i++) {
                if (i == stoi(row)) {
                    continue;
                }

                dummy.push_back(table->getTableData()[i]);
            }

            table->setTableData(dummy);
        }
    }
};

class Crud: public Create, public Read, public Update, public Delete {
    static Crud* instance;

    Crud() {}

public:
    static Crud* getInstance() {
        if (!instance) instance = new Crud();
        return instance;
    }
};

Crud* Crud::instance = nullptr;

