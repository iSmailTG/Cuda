#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  string name;
  int count = 150;
  while(count >= 1)
  {
    cout << count << ". " << name << endl;
    count = count - 5;
  }
  return 0;

}  
