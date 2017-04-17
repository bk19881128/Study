#include<iostream>
using namespace std;

class NLComponent{
public:
    virtual ostream& print(ostream& s) const = 0;
};
class TextBlock : public NLComponent{
public:
    virtual ostream& print(ostream& s) const{
        s << "TextBlock";
        return s;
    }
};
class Graphic : public NLComponent{
public:
    virtual std::ostream& print(ostream& s) const{
        s << "Graphic";
        return s;
    }
};
inline std::ostream& operator<<(std::ostream& s, const NLComponent& c){
    return c.print(s);
}

int main(){
    TextBlock tx;
    Graphic gc;
    cout << tx << endl;
    cout << gc << endl;
    
    return 0;
}
