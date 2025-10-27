#include <iostream>
#include <string>
using namespace std;

int main()
{
  int startingV;
  int endingV;
  int sum;
  
  cout << "Enter in starting value: " << endl;
  cin >> startingV;

  cout << "Enter in endingV: " << endl;
  cin >> endingV;

  while(endingV < startingV)
  {
    cout << "The ending value must be greater than or equal to starting value " << startingV << "Please enter again: "<< endl;
    cin >> startingV;

    cout << "Enter in ending value: " << endl;
    cin  >> endingV;

  }    
  sum = startingV;
  for(int count = startingV + 1; count <= endingV; count++)
  {
    cout << sum << " + " << count << " = ";
    sum = sum + count;
    cout << sum << endl;
  
  }
  cout << "the summation of numbers from " << startingV << " to " << endingV << " is: " << sum << endl;
  return 0;
}
