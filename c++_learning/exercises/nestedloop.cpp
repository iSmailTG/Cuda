#include <iostream>
#include <string>
using namespace std;


int main()
{
  int rowX;
  int columnX;
  rowX = 1;
  while(rowX <= 5)
  {
    cout << "X";
    columnX = 1;
    while(columnX <= 5)
    {
      cout << "X";
      columnX++;
      
    }
    rowX++;
    cout << endl;
    }
  return 0;
}
