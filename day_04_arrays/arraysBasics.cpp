#include <iostream>
#include <string>
using namespace std;

int main()
{
  string arr[4];
  arr[0] = "Ismail";
  arr[1] = "Sarah";
  arr[2] = "Sifao";
  arr[3] = "Yuba";
  
  int yearOfBirth[] = {1989, 2002, 2028, 2028};
  int currentYear;
  cout << "Enter in the current year: " << endl;
  cin >> currentYear;

  for(int i = 0; i < 4; i++)
  {
    int age = currentYear - yearOfBirth[i];
    cout << arr[i] << " born " << yearOfBirth[i] << " age: " << age << endl;
  } 
  return 0;
}
