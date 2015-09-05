#include "common.h"

bool isnum(string s) {
        stringstream sin(s);
        double t;
        char p;
        if(!(sin >> t))
                return false;
        if(sin >> p)
                return false;
        else
                return true;
}

int stringToInt(string s) {
        stringstream ss;
        int value;

        ss << s;
        ss >> value;

        return value;
}

