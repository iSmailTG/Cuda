#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  int num;
  cout << "print the number before assigning any value to it : " << num << endl;
  num = 5;
  cout << "print the number: " << num << endl;
  cout <<"print the number address: " << &num << endl;
  int *ptr;
  ptr = &num;
  cout << "print the address of the number through a pointer: " << ptr << endl;

  int age = 36;
  int *ptrAge = &age;
  cout << "print age: " << age << endl;
  cout << "print age from pointer: " << *ptrAge << endl;
  cout << "change age from pointer: " << endl;
  *ptrAge = 37;
  cout << *ptrAge;
  return 0;
}
