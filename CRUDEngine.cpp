#include <iostream>
#include <vector>
#include "Database.cpp"

using namespace std;

class CRUDCONNECTION {
protected:
    Database* database;

public:
    CRUDCONNECTION(Database* database) {
        this->database = database;
    }
};

class Create : CRUDCONNECTION {
public:
    Create(Database& database) : CRUDCONNECTION(&database)  {}

    void createTable(string tableName, vector<string> header) {
        if (this->database->getTable(tableName).getTableName() == "NULL_TABLE") {
            this->database->getDatabaseData().push_back(Table(header, tableName));
        } else {
            cout << "TABLE ALREADY EXISTS";
        }
    }
};

class Read : CRUDCONNECTION {
public:
    Read(Database& database) : CRUDCONNECTION(&database) {}

    Table readTable(string tableName, vector<string> headers) { // TODO add where stuff, order by, etc..
        Table& table = this->database->getTable(tableName);
        Table retTable(headers, "RESPONSE");
        vector<Row> dummyrows = {Row(headers)};

        for (int i = 1; i < table.getTableData().size(); i++) {
            int x = 0;
            vector<string> rowdata;
            for (int j = 0; j < table.getTableData()[i].getRowData().size(); j++) { // Direction concern
                if (table.getTableData()[0].getRowData()[j].getValue() == headers[x]) {
                    rowdata.push_back(table.getTableData()[i].getRowData()[j].getValue());
                    if (x < headers.size() - 1) {
                        x++;
                    }
                }
            }
            dummyrows.push_back(Row(rowdata));
        }

        retTable.setTableData(dummyrows);
        return retTable;
    }
};

class Update : CRUDCONNECTION {
public:
    Update(Database& database) : CRUDCONNECTION(&database) {}

    void addRow(string tableName, vector<string> row) {
        if (row.size() > this->database->getTable(tableName).getTableData()[0].getRowData().size()) {
            while(row.size() > this->database->getTable(tableName).getTableData()[0].getRowData().size()) {
                row.pop_back();
            }
        } else if (row.size() < this->database->getTable(tableName).getTableData()[0].getRowData().size()) {
            while(row.size() < this->database->getTable(tableName).getTableData()[0].getRowData().size()) {
                row.push_back("NULL");
            }
        }

        this->database->getTable(tableName).getTableData().push_back(Row(row));
    }
};

class Delete : CRUDCONNECTION {
public:
    Delete(Database& database) : CRUDCONNECTION(&database) {}

    void deleteRow(string tableName) {
        if (this->database->getTable(tableName).getTableData().size() >= 2) {
            this->database->getTable(tableName).getTableData().pop_back();
        }
    }

    void deleteRow(string tableName, int k) {
        Table& table = this->database->getTable(tableName);
        vector<Row> dummy;

        if (table.getTableData().size() >= 2) {
            for (int i = 0; i < table.getTableData().size(); i++) {
                if (i == k) {
                    continue;
                }

                dummy.push_back(table.getTableData()[i]);
            }

            table.getTableData() = dummy;
        }
    }
};