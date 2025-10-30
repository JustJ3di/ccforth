# Inline forth intepreter
This is a simple intepreter of a subset of forth language.
I write it to improve my c++ skill.
In this case no class, no generic programming .
But, I make extensive use of lambda funcion for the primitive.

'''

    primitives["*"] = []() {
        int b = data_stack.top(); data_stack.pop();
        int a = data_stack.top(); data_stack.pop();
        data_stack.push(a * b);
    };

'''

For the primitive it is used 
''' 
std::map<std::string,void()> 
'''
Also for the word that are defined by user it is used '''
std::map<std::string,std::vector<std::string>>> 
'''
