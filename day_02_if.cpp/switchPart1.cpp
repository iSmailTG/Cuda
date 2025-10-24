#include <iostream>
#include <string>
#include <cmath>
using namespace std;

// slightly different version of switch.cpp
// using || and && and adding weekend 


int main()
{
  int dayNum;
  cout << "Enter a number day (1 Sunday, 2 Monday, 3 Tuesday .. etc" << endl;
  cin >> dayNum;
  cout << "You selected ";
  
  if(dayNum == 1)
  {
    cout << "Sunday. ";

  }
  else if(dayNum == 2)
  {
    cout << "Monday. ";
  }
  else if(dayNum == 3)
  {
    cout << "Tuesday. ";
  }
  else if(dayNum == 4)
  {
    cout << "Wednesday. ";
  }
  else if(dayNum == 5)
  {
    cout << "Thursday. ";
  }
  else if(dayNum == 6)
  {
    cout << "Friday. ";
  }
  else if(dayNum == 6)
  {
    cout << "Saturday. ";
  }
  else
    cout << "Please enter a valid day. ";


  if(dayNum == 1 || dayNum == 7)
  {
    cout << "It's a weekend.";
  }
  else if(dayNum >=2 && dayNum <= 6)
  {
    cout << "It's a weekday." << endl;
  }
  return 0;
}
