#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int mail()
{
  float distance;
  string make;
  string model;
  float tankCapacity;
  float mpg;

  cout << "Enter the dictance of the trip: " << distance << endl;
  cin >> distance;
  cout << "Enter the brand of the car: " << make << endl;
  cin >> make;
  cout << "Enter the model of the car: " << model << endl;
  cin >> model;
  cout << "Enter the tank capacity: " << tankCapacity << endl;
  cin >> tankCapacity;
  cout << "Enter the MPG of the car: " << mpg << endl;
  cin >> mpg;

  float mileageOnFullTank = tankCapacity * mpg;
  float numOfStops = int( distance / mileageOnFullTank);
  float gasRequired  = distance / mpg;
%%writefile gasMileage.cpp

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


  float mileageOnFullTank;numOfst
  mileageOnFullTank = mpg * carCapacity;
  float numOfStops = int(distance / mileageOnFullTank);
  float gasRequired = distance / mpg;
  float galonsLeft = ((numOfStops+1) * carCapacity) - gasRequired;
  
  
  cout << "For a trip of " << distance << " miles" << endl;
  cout << " a " << carBrand << " model " << carModel << " requires " << gasRequired << " Gallons Of Gas " << endl;
  cout << " will require " << numOfStops << " stop for gas " << endl;
  cout << " and will have " << galonsLeft << " gallons of gas left in the tank" << endl;
  return 0;
}

