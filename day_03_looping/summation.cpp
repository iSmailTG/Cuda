#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  int startingV;
  int endingV;

  cout << "Enter in a starting value: ";
  cin >> startingV;

  cout << "Enter in an ending value:  ";
  cin >> endingV;


  while( endingV <= startingV)
  {
    cout << "Please enter starting value larger then ending value. Try again." << endl;
    cout << "Enter in a starting value: ";
    cin  >> startingV;
    cout << "Enter in a ending value: " << endl;
    cin >> endingV;
  }
   
  int count = startingV + 1;
  int totalSum = startingV;
  while(count <= endingV)
  {
      cout << totalSum << " + " << count << " = ";
      totalSum = totalSum + count;
      cout << totalSum << endl;
      count ++;
  }
    
  cout << "The Sum of the numbers from " << startingV << " to " << endingV << " is ";
  cout << totalSum << endl;
     
  return 0;
}
