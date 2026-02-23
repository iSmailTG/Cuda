#include <iostream>
#include <string>
using namespace std;

int main()
{
  const int NUM_YEARS_IN_SALES = 10;
  const int NUM_WEEKS_IN_YEAR = 52;
  const int NUM_DAYS_IN_WEEKS = 7;

  int numCarsSold[NUM_YEARS_IN_SALES][NUM_WEEKS_IN_YEAR][NUM_DAYS_IN_WEEKS];

  for(int year = 0; year < NUM_YEARS_IN_SALES; year++)
  {
    for(int week = 0; week < NUM_WEEKS_IN_YEAR; week++)
    {
      for(int day = 0; day < NUM_DAYS_IN_WEEKS; day++)
      {
        numCarsSold[year][week][day] = 0;
      }
    }
  }
  cout << "Enter in the car sales for the 10 years. ";
  int enteredYear;
  int enteredWeek;
  int enteredDay;
  int numCarsSoldInADay;

  char answer = 'y';
  while(answer == 'y')
  {
    cout << "Enter in a year number: " << endl;
    cin >> enteredYear;

    cout << "Enter in a week number" << endl;
    cin >> enteredWeek;

    cout << "Enter in a day number: " << endl;
    cin >> enteredDay;

    cout << "Enter in number of cars were sold in that particular day: " << endl;
    cin >> numCarsSoldInADay;

    if((enteredYear >= 0 && enteredYear < NUM_YEARS_IN_SALES) &&
       (enteredWeek >= 0 && enteredWeek < NUM_WEEKS_IN_YEAR) &&
       (enteredDay >= 0 && enteredDay < NUM_DAYS_IN_WEEKS))
    {
      numCarsSold[NUM_YEARS_IN_SALES][NUM_WEEKS_IN_YEAR][NUM_DAYS_IN_WEEKS] = numCarsSoldInADay;
    }
    else
    {
      cout << "You entered invalid year/week/day combination" << endl;
    }

    cout << "Would you like to enter another sales day (y or n):  " << endl;
    cin >> answer;
  }
  int weeklySaleYearNumber;
  int weeklySaleWeekNumber;
  
  
  cout << "Enter in the year number: " << endl;
  cin >> weeklySaleYearNumber;

  cout << "Enter in the week number: " << endl;
  cin >> weeklySaleWeekNumber;
  if((weeklySaleYearNumber >= 0 && weeklySaleYearNumber < NUM_YEARS_IN_SALES) && 
     (weeklySaleWeekNumber >= 0 && weeklySaleWeekNumber < NUM_WEEKS_IN_YEAR))
  {
    int sum = 0;
    for(int day = 0; day < weeklySaleYearNumber; day++)
    {
      sum += numCarsSold[weeklySaleYearNumber][weeklySaleWeekNumber][day];
    }
    cout << "The sum of cars sold in the week: " << sum << endl;
  }
  else
  {
    cout << "Couldn't find sales for the same day with those inputs." << endl;
  }


  int allSameDaySalesYear;
  int allSameDaySalesDay;
  
  cout << "Enter in the year number: " << endl;
  cin >> allSameDaySalesYear;
  cout << "Enter in a day number (0 for sunday, 1 for monday ..etc): " << endl;
  cin >> allSameDaySalesDay;
  
  
  if((allSameDaySalesYear >= 0 && allSameDaySalesYear < NUM_YEARS_IN_SALES) &&
     (allSameDaySalesDay >= 0 && allSameDaySalesDay < NUM_DAYS_IN_WEEKS))
  {
  
    int sum = 0;
    for(int week= 0; week < NUM_WEEKS_IN_YEAR; week++)
    {
      sum += numCarsSold[allSameDaySalesYear][week][allSameDaySalesDay];
    }
    cout << "The sum of cars sold in day of the week " << allSameDaySalesDay << " during year number: " << allSameDaySalesYear << " is " << sum << endl;
  }
  else
  {
    cout << "Cannot find the sales for the same day of the year with those inputs."<<endl;
  }
  int weekentered;

  cout << "Enter in the week number: " << endl;
  cin >> weekentered;
  if(weekentered >= 0 && weekentered < NUM_WEEKS_IN_YEAR)
  {
    int sum = 0;
    for(int year= 0; year < NUM_YEARS_IN_SALES; year++)
    {
      for(int day= 0; day < NUM_DAYS_IN_WEEKS; day++)
      {
        sum += numCarsSold[year][weekentered][day];
      }
    }
  
    cout << "The sum of cars sold in that speific week during all years is: " << sum << endl;
  }
  else
  {
    cout << "Cannot find the sales for the same day of the year with those inputs."<<endl;
  }


  return 0;
}
