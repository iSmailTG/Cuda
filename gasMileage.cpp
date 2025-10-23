#include <iostream>
#include <string>
#include <cmath>
using namespace std;
int main()
{
  float distance;
  string carBrand;
  string carModel;
  float carCapacity;
  float mpg;

  cout << "Enter the Distance of the Trip: " << endl;
  cin >> distance;

  cout << "Enter the rand of the car: " << endl;
  cin >> carBrand;

  cout  << "Enter the car Model: " << endl;
  cin >> carModel;

  cout << "Enter the fuel Tank Capacity: " << endl;
  cin >> carCapacity;
  
  cout << "Enter the MPG of the car: " << endl;
  cin >> mpg;


  float mileageOnFullTank = mpg * carCapacity;
  float numOfStops = int(distance / mileageOnFullTank);
  float gasRequired = distance / mpg;
  float galonsLeft = ((numOfStops+1) * carCapacity) - gasRequired;
  
  
  cout << "For a trip of " << distance << " miles" << endl;
  cout << " a " << carBrand << " model " << carModel << " requires " << gasRequired << " Gallons Of Gas " << endl;
  cout << " will require " << numOfStops << " stop for gas " << endl;
  cout << " and will have " << galonsLeft << " gallons of gas left in the tank" << endl;
  return 0;
}

