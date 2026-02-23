#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  int numberOfIterations;
  string name;

  cout << "Enter number of times your name will be printed: " << endl;
  cin >> numberOfIterations;
  cout << "Enter your name: " << endl;
  cin >> name;

  int count = 1;
  while( count <= numberOfIterations)
  {
    cout << name << endl;
    count++;
  }    
  return 0;
}  
