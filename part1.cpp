#include <iostream>
#include <string>
#include <cmath>
using namespace std;


int main()
{
  int number;
  cout << "Please enter a Number: " << endl;
  cin >> number;

  bool iseven = number % 2 == 0;
  if(iseven)
  {
    cout << number << " is an even number" << endl;
  }
  else
  {
    cout << number << " is an odd number" << endl;
  }

  return 0;
}
