#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include "CRUDEngine.cpp"

using namespace std;

/*
CREATE (h1:VARCHAR, h2:NUMBER, ...) in TABLE_NAME

ADDROW (c1, c2, ...) in TABLE_NAME

DELROW (k) in TABLE_NAME

DELROW () in TABLE_NAME

READ (h1, h2, ...) in TABLE_NAME where (x,y,z) orderby (ascending)


SYNTAXER:
    ACTION, SOURCE, DESTINATION

*/

void toUpper(string& value) {
    transform(value.begin(), value.end(), value.begin(), [](char x) {return toupper(x);});
} 

string retToLower(string value) {
    transform(value.begin(), value.end(), value.begin(), [](char x) {return tolower(x);});
    return value;
} 

void parseString(string& value) {
    string tmp = "";
    bool inside = false;

    for (auto x: value) {
        // Replace newline with space
        if (x == '\n') {
            tmp += " ";
            continue;
        }

        // Check if inside parantheses
        if (x == '(') {
            inside = true;
        } else if (x == ')') {
            inside = false;
        }

        // Remove space inside parantheses
        if (inside) {
            if (x != ' ') {
                tmp += x;
            }
        } else {
            tmp += x;
        }
    }

    value = tmp;
}

void removeIn(vector<string>& values) {
    vector<string> dummy;

    for (auto x: values) {
        if (retToLower(x) != "in") { // Do lowercase
            dummy.push_back(x);
        }
    }

    values = dummy;
}

struct Query {
    string action;
    vector<string> source;
    string destination;
    string where;
    string orderby;

    Table* nullTbl;
    Create* create;
    Read* read;
    Update* update;
    Delete* del;

    Query(Database& db) {
        nullTbl = &(db.getNullTable());
        create = new Create(db);
        read = new Read(db);
        update = new Update(db);
        del = new Delete(db);
    }

    Table runQuery() {
        if (action == "create") { // Syntax HNAME:DATATYPE.
            create->createTable(destination, source);
            return *(this->nullTbl);
        } else if (action == "addrow") {
            update->addRow(destination, source);
            return *(this->nullTbl);
        } else if (action == "delrow") {
            if (source[0] == "") {
                del->deleteRow(destination);
            } else {
                del->deleteRow(destination, stoi(source[0]));
            }

            return *(this->nullTbl);
        } else if (action == "read") {
            return read->readTable(destination, source, where, orderby);
        }
    }
};

struct DFA {
    vector<int> alphabet;
    vector<int> states;
    vector<int> transitions;
    int currentState;

    DFA() {}

    void UpdateState(int transition) {
        currentState = transitions[currentState * alphabet.size() + transition];
    }

};

class Lexer {
    DFA dfa;

public:
    Lexer() {
        dfa.states = vector<int>{0, 1};
        dfa.alphabet = vector<int>{0, 1, 2};
        dfa.transitions = vector<int>{0, 0, 1,  0, 0, 1};
    }

    void runLexer(Database& database, string queries, vector<Table>& READRESPONSE) {
        vector<vector<string>> wordList;

        parseString(queries);   
        vector<string> temp = {""};
        for (auto x: queries) {
            if (x == ';') {
                temp.push_back("");
            } else {
                temp[temp.size() - 1] += x;
            }
        }
        temp.pop_back();

        for (int k = 0; k < temp.size(); k++) {
            wordList.push_back({""});
            for (int i = 0; i < temp[k].length(); i++) {
                // State update
                if (temp[k][i] == ' ') {
                    dfa.UpdateState(0);
                } else if (temp[k][i] == ';') {
                    dfa.UpdateState(1);
                } else {
                    dfa.UpdateState(2);
                }

                // Collect/Append char based on state
                if (dfa.currentState == 0 && wordList[wordList.size() - 1][wordList[wordList.size() - 1].size() - 1] != "") {
                    wordList[wordList.size() - 1].push_back("");
                } else if (dfa.currentState == 1) {
                    wordList[wordList.size() - 1][wordList[wordList.size() - 1].size() - 1] += temp[k][i];
                }

                // EDGE CASE: last word
                if (i == queries.length() - 1 && wordList[wordList.size() - 1][wordList[wordList.size() - 1].size() - 1] == "") {
                    wordList[wordList.size() - 1].pop_back();
                }
            }
        }

        vector<Query> queriesVec;
        Query dummy(database);
        for (auto& x: wordList) {
            removeIn(x);
        }

        // Build queries and verify legality, run
        for (int k = 0; k < wordList.size(); k++) {
            dummy.action = retToLower(wordList[k][0]);
            dummy.destination = wordList[k][2];

            vector<string> dummytemp = {""};
            for (int j = 0; j < wordList[k][1].length(); j++) {
                if (wordList[k][1][j] == ',' && dummytemp[dummytemp.size() - 1] != "") {
                    dummytemp.push_back("");
                } else if (wordList[k][1][j] != '(' && wordList[k][1][j] != ')') {
                    dummytemp[dummytemp.size() - 1] += wordList[k][1][j];
                }
            }
            dummy.source = dummytemp;

            if (wordList[k].size() >= 4 && retToLower(dummy.action) == "read") {
                int m = wordList[k].size() - 3;
                while (m) {
                    if (retToLower(wordList[k][2 + m - 1]) == "where") {
                        dummy.where = wordList[k][2 + m];
                        m -= 2;
                    } else if (retToLower(wordList[k][2 + m - 1]) == "orderby") {
                        dummy.orderby = wordList[k][2 + m];
                        m -= 2;
                    }
                }
            }

            queriesVec.push_back(dummy);
        }

        for (auto x: queriesVec) {
            cout << x.action;
            if (x.runQuery().getTableName() != "NULL_TABLE") {
                READRESPONSE.push_back(x.runQuery());
            }
        }
    }

    vector<string> getWords(string queries) {
        vector<string> wordList = {""};

        for (auto x: queries) {
            if ((x == ' ' || x == '\n') && wordList[wordList.size() - 1] != "") {
                wordList.push_back("");
            }

            wordList[wordList.size() - 1] += x;
        }

        string sum = "";
        for (int i = 0; i < wordList.size(); i++) {
            const string orgsum = sum;
            for (int j = 0; j < wordList[i].size(); j++) {
                if (wordList[i][j] == '\n') {
                    sum += "\n";
                } else {
                    sum += " ";
                }
            }

            wordList[i] = orgsum + wordList[i];
        }

        // Ok so basically this is decent, but do calculate the colors here based on what the word is and stuff;
        return wordList;
    }
};