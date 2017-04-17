#include<iostream>  
using namespace std;

class NLComponent{
public:
    virtual ostream& operator<<(ostream& s) const = 0;//output operator的非传统声明   
};
class TextBlock : public NLComponent{
public:
    virtual ostream& operator<<(ostream& s) const{
        s << "TextBlock";
        return s;
    }
};

class Graphic : public NLComponent{
public:
    virtual ostream& operator<<(ostream& s) const{
        s << "Graphic";
        return s;
    }
};

int main(){
    TextBlock tx;
    Graphic gc;
    tx << cout << endl;//此语法与传统不符 
    gc << cout << endl;
    
    return 0;
}
