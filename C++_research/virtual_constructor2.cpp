#include<list>
#include<iostream>
using namespace std;

class NLComponent{
public:
    NLComponent(){ cout << "NLComponent()" << endl; }
    NLComponent(const NLComponent&){ cout << "NLComponent(const NLComponent&)" << endl; }
    ~NLComponent(){ cout << "~NLComponent()" << endl; }

    virtual NLComponent* clone() const = 0; //声明virtual copy constructor
    virtual void printName(){ cout << "NLComponent" << endl; }
};
class TextBlock : public NLComponent{
public:
    TextBlock(){ cout << "TextBlock()" << endl; }
    TextBlock(const TextBlock&){ cout << "TextBlock(const TextBlock&)" << endl; }
    ~TextBlock(){ cout << "~TextBlock()" << endl; }

    virtual TextBlock* clone() const{//virtual copy constructor
        return new TextBlock(*this);//调用copy constructor
    }
    virtual void printName(){ cout << "TextBlock" << endl; }
};
class Graphic : public NLComponent{
public:
    Graphic(){ cout << "Graphic()" << endl; }
    Graphic(const Graphic&){ cout << "Graphic(const Graphic&)" << endl; }
    ~Graphic(){ cout << "~Graphic()" << endl; }

    virtual Graphic* clone() const{//virtual copy constructor
        return new Graphic(*this);//调用copy constructor
    }
    virtual void printName(){ cout << "Graphic" << endl; }
};
class NewsLetter{
public:
    NewsLetter(const list<NLComponent*>& c) :components(c){}
    NewsLetter(const NewsLetter& rhs){
        //it指向rhs.components的目前元素，然后调用该元素的clone函数取得该元素的一个副本，
        //然后将该副本加到对象的components list尾端
        for (list<NLComponent*>::const_iterator it = rhs.components.begin();
            it != rhs.components.end(); ++it){
            components.push_back((*it)->clone());
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
};
int main(){
    list<NLComponent*> component;
    TextBlock* text = new TextBlock;
    cout << "-------------" << endl;
    Graphic* graphic = new Graphic;
    cout << "-------------" << endl;
    component.push_back(text);
    component.push_back(graphic);

    NewsLetter news1(component);
    news1.printNews();
    cout << "-------------" << endl;
    NewsLetter news2(news1);
    cout << "-------------" << endl;
    news2.printNews();

    return 0;
}
