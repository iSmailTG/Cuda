#include <iostream>
#include <string>
#include <cmath>
using namespace std;


int main()
{
  float hoursPerWeek;
  float payRatePerHour;
  string name;
  float weeklyPay;

  cout << "How many Hours you worked: " << endl;
  cin >> hoursPerWeek;
  cout << "How you get payed per Hour?: " << endl;
  cin >> payRatePerHour;
  cout << "Please enter your name: " << endl;
  cin >> name;


  if(hoursPerWeek <= 40.0)
  {
    weeklyPay = hoursPerWeek * payRatePerHour;
  }
  else 
  {
    weeklyPay = ((payRatePerHour * 40.0) + ((hoursPerWeek - 40.0) * ( payRatePerHour * 1.5)));
    
  }

  
  cout << name << " earned " << weeklyPay << " $$ this week." << endl;


  return 0;
}
