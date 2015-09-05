//============================================================================
// Name        : algorithmSunday.cpp
// Author      : kubao
// Version     :
// Copyright   : Your copyright notice
// Description : Sunday Algorithm
//============================================================================

#include <iostream>
#include <string.h>
#include <time.h>

#define PERFORMANCE_MONITOR

using namespace std;

enum Compare_Flag {
	COMPARE,
	NOT_COMPARE,
	NOT_INCLUDE,
};

int compareSunday(char *start, char *pattern) {
	char *s = start, *p = pattern;

	while(*p != '\0') {
		if(*s == '\0') {
			return NOT_INCLUDE;
		}
		if(*s != *p) {
			return NOT_COMPARE;
		}
		else {
			s++;
			p++;
		}
	}
	return COMPARE;
}

int shiftingSunday(char *start, char *pattern, int p_length) {
    char *position, *p_position;
    int shift_num = 1;
    position = start + p_length;
    p_position = pattern + p_length - 1;
    while(*p_position != *position && p_position >= pattern) {
        shift_num++;
        p_position--;
    }
    return shift_num;
}

bool algorithmSunday(char *inputtext,char *pattern) {
	char *i = inputtext;
	char *p = pattern;
	int compare_flag = NOT_COMPARE, num_flag = 1, num_shift = 0;
	int i_length = strlen(i);
	int p_length = strlen(p);
	while((compare_flag = compareSunday(i, p)) != COMPARE) {
		if(compare_flag == NOT_INCLUDE) {
			return false;
		}
		else {
            num_shift = shiftingSunday(i, p, p_length);
            num_flag += num_shift;
            if(num_flag >= i_length) {
    			return false;
            }
			i += num_shift;
		}
	}
	return true;
}

bool algorithmBrute(char *inputtext, char *pattern) {
	char *i = inputtext;
	char *p = pattern;
	int compare_flag = NOT_COMPARE;
	while((compare_flag = compareSunday(i, p)) != COMPARE) {
		if(compare_flag == NOT_INCLUDE) {
			return false;
		}
		else {
			i ++;
		}
	}
	return true;
}

int main() {
	bool result;
	char *text = new char[1000];
	char *patt = new char[100];

        cout << "please input the text" << endl;
        cin >> text;
        cout << "please input the patt" << endl;
        cin >> patt;

	cout << endl;

#ifdef PERFORMANCE_MONITOR
        time_t start = clock();
        for(int i = 0; i <= 1000000; i++)
	    algorithmSunday(text, patt);
        time_t end = clock();
        cout << "use Sunday algorithm: ";
        cout << double(end - start)/CLOCKS_PER_SEC << "s" << endl;

	char *test;
        start = clock();
        for(int i = 0; i <= 1000000; i++)
	    test = strstr(text, patt);
        end = clock();
        cout << "use strstr: ";
        cout << double(end - start)/CLOCKS_PER_SEC << "s" << endl;

        start = clock();
        for(int i = 0; i <= 1000000; i++)
	    algorithmBrute(text, patt);
        end = clock();
        cout << "use Brute algorithm: ";
        cout << double(end - start)/CLOCKS_PER_SEC << "s" << endl;

	cout << endl;
#endif

	result = algorithmBrute(text, patt);
	if(result == true) {
		cout << "Result:include" << endl;
	}
	else {
		cout << "Result:not include" << endl;
	}
	return 0;
}
