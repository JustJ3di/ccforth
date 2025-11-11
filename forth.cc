#include <iostream>
#include <sstream>
#include <stack>
#include <map>
#include <vector>
#include <functional>

// GLOBAL STACK
std::stack<int> data_stack;

// GLOBAL DICT for user new word
std::map<std::string, std::vector<std::string>> dictionary;

// primitives
std::map<std::string, std::function<void()>> primitives;

// single world exe
void execute(const std::string &word);

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
    primitives["+"] = []() {
        int b = data_stack.top(); data_stack.pop();
        int a = data_stack.top(); data_stack.pop();
        data_stack.push(a + b);
    };

    primitives["-"] = []() {
        int b = data_stack.top(); data_stack.pop();
        int a = data_stack.top(); data_stack.pop();
        data_stack.push(a - b);
    };

    primitives["/"] = []() {
        int b = data_stack.top(); data_stack.pop();
        int a = data_stack.top(); data_stack.pop();
        if(b==0) throw std::runtime_error("division by zero");
        data_stack.push(a / b);
    };

    primitives["*"] = []() {
        int b = data_stack.top(); data_stack.pop();
        int a = data_stack.top(); data_stack.pop();
        data_stack.push(a * b);
    };

    primitives["."] = []() {
        int a = data_stack.top(); data_stack.pop();
        std::cout << a << std::endl;
    };

    primitives["dup"] = []() {
        int a = data_stack.top();
        data_stack.push(a);
    };

    primitives["swap"] = []() {
        int a = data_stack.top(); data_stack.pop();
        int b = data_stack.top(); data_stack.pop();
        data_stack.push(a);
        data_stack.push(b);
    };

    primitives["drop"] = [](){
        data_stack.pop(); 
    };

    primitives["over"] = [](){
        if(data_stack.size()<2) throw std::runtime_error("over needs 2 elements");
        int first=data_stack.top(); data_stack.pop();
        int second = data_stack.top();
        data_stack.push(first);data_stack.push(second);
    };

    primitives["cr"] = [](){
        std::cout<<'\n';
    };

    primitives["emit"] = [](){
        std::cout << static_cast<char>(data_stack.top());
        data_stack.pop();
    };

    primitives["rot"] = []() {
        int n3 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        int n1 = data_stack.top(); data_stack.pop();
        data_stack.push(n2);data_stack.push(n3);data_stack.push(n1);
    };

    primitives["mod"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        if(n2%n1==0) throw std::runtime_error("division by zero");
        data_stack.push(n2%n1);
    };

    primitives["<"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push({n1 < n2 ? 0 : -1});
    };

    primitives[">"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push({n1 > n2 ? 0 : -1});
    };

    primitives["="] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1 == n2 ? 0 : -1);
    };

    primitives["<>"] = []() {
        int n1 = data_stack.top(); data_stack.pop();
        int n2 = data_stack.top(); data_stack.pop();
        data_stack.push(n1 != n2 ? 0 : -1);
    };

    primitives[".s"] = [](){
        
        std::vector<int> temp;
        
       
        while (!data_stack.empty()) {
            temp.push_back(data_stack.top());
            data_stack.pop();
        }

        
        std::cout << "Stack size = " << temp.size() << "\n";
        for(auto &&it:temp)
            std::cout << it << " ";
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
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;  // getline from stdin
        if (line == "q") break;                  // 

        std::istringstream iss(line);              // stream string
        std::string tok;
        while (iss >> tok)                         // read token 
            interpret_token(tok, iss);             // interpreting
    }
}

int main() {
    init_primitives();  
    repl();             // go in interpreter loop
    return 0;
}