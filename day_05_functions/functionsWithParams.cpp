#include <iostream>
#include <string>
#include <ctime>
using namespace std;

void dateFormat(int theMonth, int theDay, int theYear);
string monthNum(int monthInput);
string getDayOrdinal(int dayNumber);
int main()
{
  int month;
  int day;
  int year;

  cout << "Enter a month: " << endl;
  cin >> month;
  if(month > 12)
  {
    cout << "Month error." << endl;
    cout << "Please enter valid month: " << endl;
    cin >> month;
    
  }

  cout << "Enter a day: " << endl;
  cin >> day;
  
  cout << "Enter a year: " << endl;
  cin >> year;

  dateFormat(month, day, year);

  return 0;
}


void dateFormat(int theMonth, int theDay, int theYear)
{
  cout << monthNum(theMonth) << " " << getDayOrdinal(theDay) << ", " << theYear <<"." << endl;


}
string monthNum(int monthInput)
{
  string month;
  if(monthInput == 1)
  {
    month = "January";
  }
  else if(monthInput == 2)
  {
    month = "February";
  }
  else if(monthInput == 3)
  {
    month = "March";

  }
  else if(monthInput == 4)
  {
    month = "April";
  }
  else if(monthInput == 5)
  {
    month = "May";
  }
  else if(monthInput == 6)
  {
    month = "June";
  }
  else if(monthInput == 7)
  {
    month = "July";
  }
  else if(monthInput == 8)
  {
    month = "August";
  }
  else if(monthInput == 9)
  {
    month = "September";
  }
  else if(monthInput == 10)
  {
    month = "October";
  }
  else if(monthInput == 11)
  {
    month = "November";
  }
  else if(monthInput == 12)
  {
    month = "December";
  }

  return month;
}
string getDayOrdinal(int dayNumber)
{
  string retVal;

  if(dayNumber == 11 || dayNumber == 12 || dayNumber == 13) 
  {
        retVal = to_string(dayNumber) + "th";
  }
  else if(dayNumber % 10 == 1) 
  {
        retVal = to_string(dayNumber) + "st";
  }
  else if(dayNumber % 10 == 2) 
  {
        retVal = to_string(dayNumber) + "nd";
  }
  else if(dayNumber % 10 == 3) 
  {
        retVal = to_string(dayNumber) + "rd";
  }
  else
  {
        retVal = to_string(dayNumber) + "th";
  }
  return retVal;
}
