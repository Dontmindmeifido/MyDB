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

    void readTable(string tableName) {
        Table& table = this->database->getTable(tableName);
        for (int i = 0; i < table.getTableData().size(); i++) {
            for (int j = 0; j < table.getTableData()[i].getRowData().size(); j++) {
                cout << table.getTableData()[i].getRowData()[j].getValue() << " ";
            }

            cout << endl;
        }
    }

    void readEntireDatabase() {
        for (int i = 0; i < this->database->getDatabaseData().size(); i++) {
            cout << "NAME: " << this->database->getDatabaseData()[i].getTableName() << endl;
            cout << "---" << endl;

            readTable(this->database->getDatabaseData()[i].getTableName());

            cout << "---" << endl;
        }
    }

};

class Update : CRUDCONNECTION {
public:
    Update(Database& database) : CRUDCONNECTION(&database) {}

    void addRow(string tableName, vector<string> row) {
        this->database->getTable(tableName).getTableData().push_back(Row(row));
    }
};

class Delete : CRUDCONNECTION {
public:
    Delete(Database& database) : CRUDCONNECTION(&database) {}

    void deleteRow(string tableName) {
        this->database->getTable(tableName).getTableData().pop_back();
    }

    void deleteRow(string tableName, int k) {
        Table& table = this->database->getTable(tableName);
        vector<Row> dummy;

        for (int i = 0; i < table.getTableData().size(); i++) {
            if (i == k) {
                continue;
            }

            dummy.push_back(table.getTableData()[i]);
        }

        table.getTableData() = dummy;
    }
};