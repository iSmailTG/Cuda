#include <iostream>
#include <string>
#include <cmath>
using namespace std;
// same as what I did in switch.cpp but now I will use swith instead of if statements.

int main()
{
  int dayNumber;
  cout << "Enter a number day ( 1 for Sunday, 2 Monday, 3 Tuesday ...etc)" << endl;
  cin >> dayNumber;
  
  switch(dayNumber)
  {
    case 1:
      cout << "Sunday. " << endl;
      break;
    case 2:
      cout << "Monday. " << endl;
      break;
    case 3:
      cout << "Tuesday. " << endl;
      break;
    case 4:
      cout << "Wednesday. " << endl;
      break;
    case 5:
      cout << "Thursday. " << endl;
      break;
    case 6:
      cout << "Friday. " << endl;
      break;
    case 7:
      cout << "Saturday. " << endl;
      break;
    default:
      cout << "an invalid day. " << endl;
  }
  return 0;

}
