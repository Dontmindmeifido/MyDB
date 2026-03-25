#include <iostream>
#include <vector>

using namespace std;

bool isNUMBER(string value) {
    bool isNumber = true;
    for (auto x: value) {
        if (!(char(x) >= 48 && char(x) <= 57)) {
            isNumber = false;
        }
    }

    return isNumber;
}

bool isNUMBER(char value) {
    bool isNumber = true;
    if (!(char(value) >= 48 && char(value) <= 57)) {
        isNumber = false;
    }

    return isNumber;
}

bool isDATETIME(string value) {
    bool isDatetime = true;
    vector<char> FORMAT = {'D', 'D', '-', 'M', 'M', '-', 'Y', 'Y', 'Y', 'Y'};

    for (int i = 0; i < FORMAT.size(); i++) {
        switch(FORMAT[i]) {
            case 'D':
                if (!isNUMBER(value[i])) isDatetime = false;
                break;
            case 'M':
                    if (!isNUMBER(value[i])) isDatetime = false;
                break;
            case 'Y':
                if (!isNUMBER(value[i])) isDatetime = false;
                break;
            case '-':
                if (value[i] != '-') {
                    isDatetime = false;
                }
                break;
        }
    }

    return isDatetime;
}

class DataType {
protected:
    string value;
    string type;

public:
    string getValue() const {
        return this->value;
    }

    string getType() const {
        return this->type;
    }

    virtual ~DataType() {};
};

class VARCHAR: public DataType {
public:
    VARCHAR() {
        this->value = "";
        this->type = "VARCHAR";
    }

    VARCHAR(string value) {
        this->value = value;
        this->type = "VARCHAR";
    }

    VARCHAR(VARCHAR* obj) {
        this->value = obj->getValue();
        this->type = "VARCHAR";
    }
};

class NUMBER: public DataType {
public: 
    NUMBER() {
        this->value = "0";
        this->type = "NUMBER";
    }

    NUMBER(string value) {
        if (isNUMBER(value)) {
            this->value = value;
            this->type = "NUMBER";
        } else {
            cout << "ERROR WRONG NUMBER TYPE";
        }
    }

    NUMBER(NUMBER* obj) {
        this->value = obj->getValue();
        this->type = "NUMBER";
    }
};

class DATETIME: public DataType {
public:
    DATETIME() {
        this->value = "01-01-1970";
        this->type = "DATETIME";
    }

    DATETIME(string value) {
        if (isDATETIME(value)) {
            this->value = value;
            this->type = "DATETIME";
        } else {
            cout << "ERROR WRONG DATETIME TYPE";
        }
    }

    DATETIME(DATETIME* obj) {
        this->value = obj->getValue();
        this->type = "DATETIME";
    }
};

class Cell {
    DataType* data;

public:
    Cell() {
        this->data = new VARCHAR();
    }

    Cell(const Cell& other) {
        if (other.getDataType() == "VARCHAR") {
            this->data = new VARCHAR(other.getValue());
        } else if (other.getDataType() == "NUMBER") {
            this->data = new NUMBER(other.getValue());
        } else if (other.getDataType() == "DATETIME") {
            this->data = new DATETIME(other.getValue());
        }
    }

    Cell(string value) {
        if (isDATETIME(value)) {
            this->data = new DATETIME(value);
        } else if (isNUMBER(value)) {
            this->data = new NUMBER(value);
        } else {
            this->data = new VARCHAR(value);
        }
    }

    Cell& operator=(const Cell& other) {
        if (&other == this) {
            return *this;
        }

        delete this->data;

        if (other.getDataType() == "VARCHAR") {
            this->data = new VARCHAR(other.getValue());
        } else if (other.getDataType() == "NUMBER") {
            this->data = new NUMBER(other.getValue());
        } else if (other.getDataType() == "DATETIME") {
            this->data = new DATETIME(other.getValue());
        }

        return *this;
    }

    string getValue() const {
        return this->data->getValue();
    }

    string getDataType() const {
        return this->data->getType();
    }

    ~Cell() {
        delete this->data;
    }
};

class Row {
    vector<Cell> rowData;

public:
    Row(vector<string> rowData) {
        vector<Cell> auxRowData;

        for (auto x: rowData) {
            auxRowData.push_back(Cell(x));
        }

        this->rowData = auxRowData;
    }

    vector<Cell> getRowData() const {
        return this->rowData;
    }

    void setRowData(vector<Cell> rowData) {
        this->rowData = rowData;
    }
};

class Table {
    vector<Row> tableData;
    vector<string> tableDataTypes;
    string tableName;

    bool verifyDataTypes(const vector<Row>& tableData) {
        if (tableDataTypes[0] == "ANY") { // Escape ANY type
            return true;
        }

        for (int i = 1; i < tableData.size(); i++) {
            for (int j = 0; j < tableData[i].getRowData().size(); j++) {
                if (tableDataTypes[j] != tableData[i].getRowData()[j].getDataType())
                return false;
            }
        }

        return true;
    }

    bool verifyColumnSize(const vector<Row>& tableData) {
        for (int i = 1; i < tableData.size(); i++) {
            if (tableData[0].getRowData().size() != tableData[i].getRowData().size()) 
            return false;

        }

        return true;
    }

public:
    Table(vector<string> header, vector<string> datatypes, string tableName) {
        this->tableData.push_back(Row(header));
        this->tableDataTypes = datatypes;
        this->tableName = tableName;
    }

    vector<Row> getTableData() {
        return this->tableData;
    }

    void setTableData(vector<Row> tableData) {
        bool validType = verifyDataTypes(tableData);
        bool validColumns = verifyColumnSize(tableData);

        if (validType && validColumns) {
            this->tableData = tableData;
        } else {
            cout << "INVALID WTF";
        }
    }

    string getTableName() {
        return this->tableName;
    }
};

class Database {
    static Database* instance;
    vector<Table> databaseData; 

    Database() {}

public:
    static Database* getInstance() {
        if (!instance) instance = new Database();
        return instance;
    }

    Table* getTable(string name) {
        for (int i = 0; i < databaseData.size(); i++) {
            if (databaseData[i].getTableName() == name) {
                return &databaseData[i];
            }
        }

        cout << "TABLE NOT FOUND";
        return nullptr;
    }

    vector<Table> getDatabaseData() {
        return this->databaseData;
    }

    void setDatabaseData(vector<Table> databaseData) {
        this->databaseData = databaseData;
    }
};

Database* Database::instance = nullptr;