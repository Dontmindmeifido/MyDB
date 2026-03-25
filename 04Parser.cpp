#include <string>
#include <cctype>
#include <algorithm>
#include <vector>

using namespace std;

class Parser {
public:
    void toUpper(string& value) {
        transform(value.begin(), value.end(), value.begin(), [](char x) {return toupper(x);});
    } 

    string retToLower(string value) {
        transform(value.begin(), value.end(), value.begin(), [](char x) {return tolower(x);});
        return value;
    } 

    // Strips all newlines and spaces;
    string retStrip(string value) {
        string dummy = "";
        for (auto x: value) {
            if (x != ' ' && x != '\n') {
                dummy += x;
            }
        }

        return dummy;
    }
    
    // Removes spaces inside paranthesis of queries
    void removeSpaces(string& value) {
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

    // Removes in keywords in list of token lists
    void removeIn(vector<vector<string>>& values) {
        vector<vector<string>> tempValues;

        for (auto instruction: values) {
            vector<string> dummy;

            for (auto x: instruction) {
                if (retToLower(x) != "in") {
                    dummy.push_back(x);
                }
            }

            tempValues.push_back(dummy);
        }

        values = tempValues;
    }

    // Splits instructions by the separator ';'
    vector<string> splitInstructions(string value) {
        vector<string> instructions = {""};
        for (auto x: value) {
            if (x == ';') {
                instructions.push_back("");
            } else {
                instructions[instructions.size() - 1] += x;
            }
        }
        instructions.pop_back();

        return instructions;
    }

    // Tokenizes general arguments (separated only by comma, undefined number of inputs)
    vector<string> tokenizeGeneralArgument(string value) {
        vector<string> dummy = {""};

        for (int j = 0; j < value.length(); j++) {
            if (value[j] == ',' && dummy[dummy.size() - 1] != "") {
                dummy.push_back("");
            } else if (value[j] != '(' && value[j] != ')') {
                dummy[dummy.size() - 1] += value[j];
            }
        }

        return dummy;
    }

    // Tokenizes filter arguments (separated only by comma, 2 or 3 inputs)
    vector<string> tokenizeFilterArgument(string value) {
        vector<string> tokens;

        int k = 0;
        for (auto x: value) {
            if (x == ',') {
                k += 1;
                continue;
            }

            if (k == 0 && x != '(' && x != ')') {
                if (tokens.size() == 0) {
                    tokens.push_back("");
                }

                tokens[0] += x;
            } else if (k == 1 && x != '(' && x != ')') {
                if (tokens.size() == 1) {
                    tokens.push_back("");
                }

                tokens[1] += x;
            } else if (k == 2 && x != '(' && x != ')') {
                if (tokens.size() == 2) {
                    tokens.push_back("");
                }
                
                tokens[2] += x;
            }
        }

        return tokens;
    }

    // Tokenizes Create crud arguments (separated by comma and double point)
    pair<vector<string>, vector<string>> tokenizeCreateArgument(vector<string> header) {
        vector<string> headers;
        vector<string> dataTypes;

        for (int i = 0; i < header.size(); i++) {
            bool side = true;

            headers.push_back("");
            dataTypes.push_back("");
            
            for (auto x: header[i]) {
                if (x == ':') {
                    side = false;
                } else {
                    if (side) {
                        headers[headers.size() - 1] += x;
                    } else {
                        dataTypes[dataTypes.size() - 1] += x;
                    }
                }
            }
        }

        return {headers, dataTypes};
    }

    // For syntax coloring
    vector<string> getSpacedWords(string queries) {
        vector<string> wordList = partitionQuery(queries);

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

        return wordList;
    }

    // For getSpacedWords
    vector<string> partitionQuery(string value) {
        vector<string> dummy = {""};

        for (auto x: value) {
            if ((x == ' ' || x == '\n') && dummy[dummy.size() - 1] != "") {
                dummy.push_back("");
            }

            dummy[dummy.size() - 1] += x;
        }

        return dummy;
    }

    // For name suggestions
    vector<string> getSnippet(string lastWord, Database& db) {
        vector<string> checks = {"CREATE", "ADDROW", "DELROW", "READ", "WHERE", "ORDERBY"};
        for (auto x: db.getDatabaseData()) {
            checks.push_back(x.getTableName());
            for (auto y: x.getTableData()[0].getRowData()) {
                checks.push_back(y.getValue() + ":" + x.getTableName());
            }
        }

        vector<string> ret;
        if (lastWord == "") {
            return ret;
        }

        if (lastWord[0] == '(') {
            lastWord = lastWord.substr(1, -1);
        }

        for (auto word: checks) {
            if ( retToLower(word.substr(0, min(int(word.length()), int(lastWord.length())))) == retToLower(lastWord.substr(0, min(int(word.length()), int(lastWord.length())))) ) {
                ret.push_back(word);
            }
        }

        return ret;
    }

    // For name suggestions rect positioning
    pair<int, int> getCursorPosition(string queryBuffer) {
        int x = 0;
        int y = 0;

        for (auto chr: queryBuffer) {
            if (chr == '\n') {
                y += 20;
                x = 0;
            } else {
                x += 8;
            }
        }

        return {x, y};
    }
};