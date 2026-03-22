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
            vector<string> headers;
            vector<string> datatypes;

            for (int i = 0; i < header.size(); i++) {
                bool side = true;
                headers.push_back("");
                datatypes.push_back("");
                for (auto x: header[i]) {
                    if (x == ':') {
                        side = false;
                    } else {
                        if (side) {
                            headers[headers.size() - 1] += x;
                        } else {
                            datatypes[datatypes.size() - 1] += x;
                        }
                    }
                }
            }

            this->database->getDatabaseData().push_back(Table(headers, datatypes, tableName));
        } else {
            cout << "TABLE ALREADY EXISTS";
        }
    }
};

class Read : CRUDCONNECTION {
public:
    Read(Database& database) : CRUDCONNECTION(&database) {}

    Table readTable(string tableName, vector<string> headers, string where, string orderby) { // TODO add where stuff, order by, etc..
        Table& table = this->database->getTable(tableName);
        Table retTable(headers, {""}, "RESPONSE");
        vector<Row> dummyrows = {Row(headers)};

        string column = "";
        string operation = "";
        string comparator = "";

        int k = 0;
        for (auto x: where) {
            if (x == ',') {
                k += 1;
                continue;
            }

            if (k == 0 && x != '(' && x != ')') {
                column += x;
            } else if (k == 1 && x != '(' && x != ')') {
                operation += x;
            } else if (k == 2 && x != '(' && x != ')') {
                comparator += x;
            }
        }

        int columnIndex = 0;
        for (int i = 0; i < table.getTableData()[0].getRowData().size(); i++) {
            if (table.getTableData()[0].getRowData()[i].getValue() == column) {
                columnIndex = i;
            }
        }

        string columnOrder = "";
        string comparatorOrder = "";

        k = 0;
        for (auto x: orderby) {
            if (x == ',') {
                k += 1;
                continue;
            }

            if (k == 0 && x != '(' && x != ')') {
                columnOrder += x;
            } else if (k == 1 && x != '(' && x != ')') {
                comparatorOrder += x;
            }
        }
        cout << columnOrder;

        int columnIndexOrder = 0;
        for (int i = 0; i < headers.size(); i++) {
            if (headers[i] == columnOrder) {
                columnIndexOrder = i;
            }
        }

        for (int i = 1; i < table.getTableData().size(); i++) {
            int x = 0;
            vector<string> rowdata;
            for (int j = 0; j < table.getTableData()[i].getRowData().size(); j++) { // Direction concern
                if (table.getTableData()[0].getRowData()[j].getValue() == headers[x]) {
                    if (!where.empty()) {
                        if (operation == ">") {
                            if (table.getTableData()[i].getRowData()[columnIndex].getValue() > comparator) {
                                rowdata.push_back(table.getTableData()[i].getRowData()[j].getValue());
                            }
                        } else if (operation == "<") {
                            if (table.getTableData()[i].getRowData()[columnIndex].getValue() < comparator) {
                                rowdata.push_back(table.getTableData()[i].getRowData()[j].getValue());
                            }
                        } else if (operation == ">=") {
                            if (table.getTableData()[i].getRowData()[columnIndex].getValue() >= comparator) {
                                rowdata.push_back(table.getTableData()[i].getRowData()[j].getValue());
                            }
                        } else if (operation == "<=") {
                            if (table.getTableData()[i].getRowData()[columnIndex].getValue() <= comparator) {
                                rowdata.push_back(table.getTableData()[i].getRowData()[j].getValue());
                            }
                        } else if (operation == "=") {
                            if (table.getTableData()[i].getRowData()[columnIndex].getValue() == comparator) {
                                rowdata.push_back(table.getTableData()[i].getRowData()[j].getValue());
                            }
                        }
                    } else {
                        rowdata.push_back(table.getTableData()[i].getRowData()[j].getValue());
                    }

                    if (x < headers.size() - 1) {
                        x++;
                    }
                }
            }

            if (!rowdata.empty()) {
                dummyrows.push_back(Row(rowdata));
            }
        }

        if (!orderby.empty()) {
            if (comparatorOrder == "descending") {
                sort(dummyrows.begin() + 1, dummyrows.end(), [&columnIndexOrder](Row a, Row b) {return a.getRowData()[columnIndexOrder].getValue() > b.getRowData()[columnIndexOrder].getValue();});
            } else if (comparatorOrder == "ascending"){
                sort(dummyrows.begin() + 1, dummyrows.end(), [&columnIndexOrder](Row a, Row b) {return a.getRowData()[columnIndexOrder].getValue() < b.getRowData()[columnIndexOrder].getValue();});
            }
        }


        retTable.setTableData(dummyrows);
        return retTable;
    }
};

class Update : CRUDCONNECTION {
public:
    Update(Database& database) : CRUDCONNECTION(&database) {}

    void addRow(string tableName, vector<string> row) {
        if (row.size() == this->database->getTable(tableName).getTableData()[0].getRowData().size()) {
            if (this->database->getTable(tableName).verifyDataTypes(Row(row))) {
                this->database->getTable(tableName).getTableData().push_back(Row(row));
            }
        } else {
            cout << "WRONG DIMENSION OR WRONG TYPES";
        }
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