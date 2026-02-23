#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  int dayNumber;

  cout << "Entre in a day number (1 Sunday, 2 Monday, 3 Tuesday ..etc)" << endl;
  cin >> dayNumber;

  if(dayNumber == 1)
  {
    cout << "Sunday" << endl;
  }
  else if(dayNumber == 2)
  {
    cout << "Monday" << endl;
  }
  else if(dayNumber == 3)
  {
    cout << "Tuesday" << endl;
  }  
  else if(dayNumber == 4)
  {
    cout << "Wednesday" << endl;
  }  
  else if( dayNumber == 5)
  {
    cout << "Thursday" << endl;
  }
  else if(dayNumber == 6)
  {
    cout << "Friday" << endl;
  }
  else if( dayNumber == 7)
  {
    cout << "Saturday." << endl;
  }
  else
  {
    cout << "an invalid day. ";
  }
  return 0;
}

