#include <iostream>
#include "Complex.h"


using namespace std;
using namespace caveofprogramming;

int main() {
   Complex c1(3,4);
   Complex c2(2,3);
   cout <<"c1 :"<< c1 << endl;
   cout <<"c2 :"<< c2 << endl;
   cout <<"*c2 :"<< *c2 << endl;
   cout <<"c1 + c2 :" <<c1 + c2 << endl;

   return 0;

}