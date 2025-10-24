#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  float temp;
  char tempScale;
  cout << "Enter in the temperature: " << endl;
  cin >> temp;
  cout << "Enter temperature scale ( C or F ): " << endl;
  cin >> tempScale;

  if( tempScale == 'C' || tempScale == 'F')
  {
    if(tempScale == 'F')
    {
      if(temp <= 32.0)
      {
        cout << "Water will turn to ice at "<<temp<< " F degrees." << endl;
        
      }
      else if(temp >= 212.0)
      {
        cout << "Water will turn to steam at " <<temp<< " F degrees." << endl;

      }
      else
      {
        cout << "Water will stay liquid at " << temp<< " F degrees." << endl;
      }
    
    }
    else
    {
      if(temp <= 0.0)
      {
        cout << "Water will turn to ice at " << temp << " C degrees." << endl;
      }
      else if(temp >= 100.0)
      {
        cout << "Water will turn to steam at " << temp << "C degrees." << endl;
      }
      else
      {
        cout << "water will stay liquid at " << temp << "C degrees." << endl;
      }
    }
  }
  else 
  {
    cout << "You entered an invalid temperature scale. Please run the program again." <<endl;
  }
  return 0;
}

