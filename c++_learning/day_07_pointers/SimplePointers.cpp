#include <iostream>
#include <string>
using namespace std;

int main()
{
  int num;
  num = 5;

  int* pointer_a;
  int* pointer_b;

  pointer_a = &num;

  cout << "this is variable num: " << num << endl;
  cout << "this is the address of the variable num: " << &num << endl;
  cout << "This is the address of the variable num from the pointer: " << pointer_a << endl;
  cout << "This is called dereference, which is pointing to the value stored in a given pointer: " << *pointer_a << endl;
  
  *pointer_a = 100;
  cout << "we changed pointer_a's value: " << *pointer_a << endl;
  cout << "See if num changed: " << num << endl;

  pointer_b = &num;

  cout << "address of num: " << &num;
  cout << "matches both pointers: " << pointer_a << " & " << pointer_b << endl;
  


  return 0;
}
