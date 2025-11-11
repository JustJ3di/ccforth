#include <iostream>
#include <sstream>
#include <stack>
#include <map>
#include <vector>
#include <functional>
#include <stdexcept>

// GLOBAL STACK
std::stack<int> data_stack;

// GLOBAL DICT for user new word
std::map<std::string, std::vector<std::string>> dictionary;

// primitives
std::map<std::string, std::function<void()>> primitives;

// single world exe
void execute(const std::string &word);

// Helper to execute a sequence of words (the loop body)
void execute_loop_body(const std::vector<std::string> &body) {
    for (const auto &w : body) {
        execute(w);
    }
}

//token interpreter
void interpret_token(const std::string &tok, std::istringstream &iss) {
    // try if is a num (better than isdigit ecc..)
    try {
        int num = std::stoi(tok);
        data_stack.push(num);
        return;
    } catch (...) {
        // it is not a number
    }

    // if is a primitive
    if (primitives.count(tok)) {
        primitives[tok]();
        return;
    }

    //if it is a world
    if (dictionary.count(tok)) {
        for (auto &w : dictionary[tok])
            execute(w);
        return;
    }

    // if it is ":" the user defines a new word with a new istrucion so we can push a new token in the dict
    if (tok == ":") {
        std::string name;
        iss >> name;
        std::vector<std::string> body;
        std::string w;
        while (iss >> w && w != ";")
            body.push_back(w);
        dictionary[name] = body;
        return;
    }

    // --- NEW LOGIC FOR ." (string literal) ---
    if (tok == ".\"") {
        char c;
        std::string str_to_print;
        // Read until the closing double quote or end of stream
        while (iss.get(c) && c != '"') {
            str_to_print += c;
        }
        std::cout << str_to_print;
        return;
    }


    if (tok == "IF") {
        if (data_stack.empty()) throw std::runtime_error("IF needs a flag");
        int flag = data_stack.top(); data_stack.pop();
        std::string next_word;
        if (iss >> next_word) {
            // Standard Forth: 0 is false, non-zero is true
            if (flag != 0) { // If TRUE (non-zero)
                execute(next_word);
            }
        }
        return;
    }

    // ---  DO/LOOP (Interpreted Counted Loop) ---
    if (tok == "DO") {
        if (data_stack.size() < 2) throw std::runtime_error("DO needs limit and start index");

        int start = data_stack.top(); data_stack.pop();
        int limit = data_stack.top(); data_stack.pop();

        std::vector<std::string> loop_body;
        std::string w;

        // Collect words until LOOP is found
        while (iss >> w && w != "LOOP") {
            loop_body.push_back(w);
        }

        // Execute the loop
        for (int i = start; i < limit; ++i) {
            execute_loop_body(loop_body);
        }
        return;
    }

    std::cerr << "Unknown word: " << tok << "\n";
}

// execute word
void execute(const std::string &word) {
    std::istringstream iss(word);
    std::string tok;
    while (iss >> tok)
        interpret_token(tok, iss);
}

// Setup
void init_primitives() {

    //MATH

    primitives["+"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1 + n2);
    };

    primitives["-"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n2 - n1);
    };

    primitives["/"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        if(n1==0) throw std::runtime_error("division by zero");
        data_stack.push(n2 / n1);
    };

    primitives["*"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1 * n2);
    };

    primitives["mod"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        if(n1==0) throw std::runtime_error("modulo by zero");
        data_stack.push(n2 % n1);
    };

    //LOGIC OPERATOR
    primitives["<"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n2 < n1 ? -1 : 0); // Forth standard: -1 for TRUE, 0 for FALSE
    };

    primitives[">"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n2 > n1 ? -1 : 0);
    };

    primitives["="] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1 == n2 ? -1 : 0);
    };

    primitives["<>"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1 != n2 ? -1 : 0);
    };

    primitives["and"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1 & n2); // Bitwise AND is the common Forth implementation
    };

    primitives["or"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1 | n2); // Bitwise OR is the common Forth implementation
    };

    //STACK MANIPULATION
    primitives["dup"] = []() {
        if(data_stack.empty()) throw std::runtime_error("DUP needs one element");
        int n = data_stack.top();
        data_stack.push(n);
    };

    primitives["swap"] = []() {
        if(data_stack.size() < 2) throw std::runtime_error("SWAP needs two elements");
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1);
        data_stack.push(n2);
    };

    primitives["drop"] = [](){
        if(data_stack.empty()) throw std::runtime_error("DROP needs one element");
        data_stack.pop();
    };

    primitives["over"] = [](){
        if(data_stack.size()<2) throw std::runtime_error("OVER needs 2 elements");
        int n1=data_stack.top(); data_stack.pop();
        int n2 = data_stack.top();
        data_stack.push(n1);data_stack.push(n2);
    };

    primitives["rot"] = []() {
        if(data_stack.size()<3) throw std::runtime_error("ROT needs 3 elements");
        int n3 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        int n1 = data_stack.top(); data_stack.pop();
        data_stack.push(n2);data_stack.push(n3);data_stack.push(n1);
    };

    //PRINT
    primitives["."] = []() {
        if(data_stack.empty()) throw std::runtime_error(". needs one element");
        int a = data_stack.top(); data_stack.pop();
        std::cout << a << " ";
    };

    primitives["cr"] = [](){
        std::cout<<'\n';
    };

    primitives["emit"] = [](){
        if(data_stack.empty()) throw std::runtime_error("EMIT needs one element");
        std::cout << static_cast<char>(data_stack.top());
        data_stack.pop();
    };

    primitives[".s"] = [](){

        std::vector<int> temp;


        while (!data_stack.empty()) {
            temp.push_back(data_stack.top());
            data_stack.pop();
        }


        std::cout << "<" << temp.size() << "> ";
        for (auto it = temp.rbegin(); it != temp.rend(); ++it)
            std::cout << *it << " ";
        std::cout << "\n";


        for (auto it = temp.rbegin(); it != temp.rend(); ++it)
            data_stack.push(*it);

    };
}

// La REPL (Read–Eval–Print Loop)
void repl() {
    std::string line;
    std::cout << "toy-forth (C++) REPL. Type 'q' to exit.\n";

    while (true) {
        std::cout << "ok> ";
        if (!std::getline(std::cin, line)) break;  // getline from stdin
        if (line == "q") break;                  //

        try {
            std::istringstream iss(line);              // stream string
            std::string tok;
            while (iss >> tok)                         // read token
                interpret_token(tok, iss);             // interpreting
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}

int main() {
    init_primitives();
    repl();             // go in interpreter loop
    return 0;
}