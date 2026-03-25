#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include "02Query.cpp"
#include "04Parser.cpp"

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

class DFA {
    vector<int> alphabet;
    vector<int> states;
    vector<int> transitions;
    vector<char> alphabetMap; // includes % as last elements - everything else
    int currentState;

    void UpdateState(int transition) {
        currentState = transitions[currentState * alphabet.size() + transition];
    }

public:
    DFA(vector<int> alphabet, vector<int> states, vector<int> transitions, vector<char> alphabetMap) {
        this->alphabet = alphabet;
        this->states = states;
        this->transitions = transitions;
        this->alphabetMap = alphabetMap;
        this->currentState = 0;
    }

    vector<int> runDFA(string value) {
        vector<int> states;

        for (auto chr: value) {
            // Find in alphabetMap
            int transitionIndex = alphabetMap.size() - 1;
            for (int i = 0; i < alphabetMap.size() - 1; i++) {
                if (chr == alphabetMap[i]) {
                    transitionIndex = i;
                    break;
                }
            }

            this->UpdateState(transitionIndex);
            states.push_back(currentState); // Starts from 1
        }

        return states;
    }
};

class Lexer: public Parser {
    static Lexer* instance;
    DFA dfaLexer;

    Lexer(): 
    dfaLexer(vector<int>{0, 1, 2}, vector<int>{0, 1}, vector<int>{0, 0, 1,  0, 0, 1}, vector<char> {' ', ';', '%'}) {}

public:
    static Lexer* getInstance() {
        if (!instance) instance = new Lexer();
        return instance;
    }

    vector<vector<string>> runLexer(string queries) {
        vector<vector<string>> tokenList;

        removeSpaces(queries);   
        vector<string> instructions = splitInstructions(queries);
        for (int k = 0; k < instructions.size(); k++) {
            vector<int> state = dfaLexer.runDFA(instructions[k]);

            tokenList.push_back({""});
            for (int i = 0; i < instructions[k].length(); i++) {
                int lastInstruction = tokenList.size() - 1;
                int lastToken = tokenList[lastInstruction].size() - 1;

                // Collect char based on state
                if (state[i] == 0 && tokenList[lastInstruction][lastToken] != "") {
                    tokenList[lastInstruction].push_back("");
                } else if (state[i] == 1) {
                    tokenList[lastInstruction][lastToken] += instructions[k][i];
                }

                // EDGE CASE: last word
                if (i == instructions[k].length() - 1 && tokenList[lastInstruction][lastToken] == "") {
                    tokenList[lastInstruction].pop_back();
                }
            }
        }

        // Debug
        cout << "\nDEBUG TOKENLIST\n";
        for (auto x: tokenList) {
            for (auto y: x) {
                cout << y << " ";
            }
            cout << endl;
        }
        cout << "\nDEBUG TOKENLIST\n";

        removeIn(tokenList);

        return tokenList;
    }
};

class Interpreter {
    static Interpreter* instance;

    Interpreter() {}

public:
    static Interpreter* getInstance() {
        if (!instance) instance = new Interpreter();
        return instance;
    }

    void runInterpreter(string queries, vector<Table*>* READRESPONSE) {
        Interpreter* Interpreter = Interpreter::getInstance();
        Lexer* lexer = Lexer::getInstance();
        
        // Build queries
        vector<vector<string>> instructions = lexer->runLexer(queries);
        for (vector<string> token: instructions) {
            Query query;
            
            query.action = lexer->retToLower(token[0]);
            query.destination = token[2];
            query.source = lexer->tokenizeGeneralArgument(token[1]);

            if (lexer->retToLower(query.action) == "read") {
                for (int j = 3; j < token.size(); j += 2) {
                    string keyword = lexer->retToLower(token[j]);

                    if (keyword == "where") {
                        query.where = lexer->tokenizeFilterArgument(token[j + 1]);
                    } else if (keyword == "orderby") {
                        query.orderby = lexer->tokenizeFilterArgument(token[j + 1]);
                    }
                }
            }

            if (lexer->retToLower(query.action) == "create") {
                pair<vector<string>, vector<string>> pair = lexer->tokenizeCreateArgument(query.source);
                query.source = pair.first;
                query.types = pair.second;
            }

            // Run query
            Table* queryResponse = query.runQuery();
            if (queryResponse != nullptr) {
                READRESPONSE->push_back(queryResponse);
            }
        }
    }
};

Lexer* Lexer::instance = nullptr;
Interpreter* Interpreter::instance = nullptr;