#include<list>
#include<sstream>
#include<iostream>
using namespace std;

class NLComponent{
public:
    NLComponent(){ cout << "NLComponent()" << endl; }
    NLComponent(const NLComponent&){ cout << "NLComponent(const NLComponent&)" << endl; }
    ~NLComponent(){ cout << "~NLComponent()" << endl; }

    virtual void printName(){ cout << "NLComponent" << endl; }
};
class TextBlock : public NLComponent{
public:
    TextBlock(){ cout << "TextBlock()" << endl; }
    TextBlock(const TextBlock&){ cout << "TextBlock(const TextBlock&)" << endl; }
    ~TextBlock(){ cout << "~TextBlock()" << endl; }

    virtual void printName(){ cout << "TextBlock" << endl; }
};
class Graphic : public NLComponent{
public:
    Graphic(){ cout << "Graphic()" << endl; }
    Graphic(const Graphic&){ cout << "Graphic(const Graphic&)" << endl; }
    ~Graphic(){ cout << "~Graphic()" << endl; }

    virtual void printName(){ cout << "Graphic" << endl; }
};
class NewsLetter{
public:
    NewsLetter(stringstream& ss){ //copy constructor
        string str;
        while (ss >> str){
            components.push_back(readComponent(str));
        }
    }
    void printNews(){
        for (list<NLComponent*>::const_iterator it = components.begin();
            it != components.end(); ++it){
            (*it)->printName();
        }
    }
private:
    list<NLComponent*> components;
    static NLComponent* readComponent(const string& str){//virtual copy constructor
        if (str == "TextBlock")
            return new TextBlock;
        if (str == "Graphic")
            return new Graphic;
        return NULL;
    }
};
int main(){
    stringstream ss("TextBlock Graphic Graphic");
    NewsLetter news(ss);
    news.printNews();

//    system("pause");
    return 0;
}
