#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  float workedHours;
  float payPerHour;
  string name;
  float weeklyPay;

  cout << "How much Hours did you work this week: " << endl;
  cin >> workedHours;
  cout << "How much you get payed per Hour?: " << endl;
  cin >> payPerHour;
  cout << "What's your Name: " << endl;
  cin >> name; 
  weeklyPay = workedHours * payPerHour;
  cout << name << " earned " << weeklyPay << "$$ this week" << endl; return 0;

  return 0;

}
