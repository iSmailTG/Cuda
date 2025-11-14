#include <iostream>
#include <string>
#include <ctime>
using namespace std;


void printDate(int theYear, int theMonth, int theDay);
string getMonthName(int monthNumber);

int main()
{
  int year;
  int month;
  int day;

  cout << "Enter in a year: " << endl;
  cin >> year;

  cout << "Enter in a month (1-12): " << endl;
  cin >> month;

  cout << "Enter in a day (1-31): " << endl;
  cin >> day;

  printDate(year, month, day);
  

  return 0;
}

void printDate(int theYear, int theMonth, int theDay)
{
  cout << getMonthName(theMonth) << " " << theDay << ", " << theYear << "." << endl;
}
string getMonthName(int monthNumber)
{
  string retVal;
  if(monthNumber == 1)
  {
    retVal = "January";
  }
  else if(monthNumber == 2)
  {
    retVal = "February";
  }
  else if(monthNumber == 3)
  {
    retVal = "March";
  }
  else if(monthNumber == 4)
  {
    retVal = "April";
  }
  else if(monthNumber == 5)
  {
    retVal = "May";
  }
  else if(monthNumber == 6)
  {
    retVal = "June";
  }
  else if(monthNumber == 7)
  {
    retVal = "July";
  }
  else if(monthNumber == 8)
  {
    retVal = "August";
  }
  else if(monthNumber == 9)
  {
    retVal = "September";
  }
  else if(monthNumber == 10)
  {
    retVal = "October";
  }
  else if(monthNumber == 11)
  {
    retVal = "November";
  }
  else if(monthNumber == 12)
  {
    retVal = "December";
  }

  return retVal;
}
