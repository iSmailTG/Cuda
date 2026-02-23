#include <iostream>
#include <string>
using namespace std;

void printName();
void printFirst();
void printLast();

int main()
{


  printName();

  return 0;
}

void printName()
{
  printFirst();
  printLast();
}
void printFirst()
{
  cout << "Ismail ";
}
void printLast()
{
  cout << "Taghouchti" << endl;
}
