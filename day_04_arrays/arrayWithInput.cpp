#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  int nums[20];
  int num;
  int sum = 0;
  int numberOfElements;
  char response;
  for(int i = 0; i < 20; i++)
  {
    cout << "enter in a number: " << endl;
    cin >> num;

    nums[i] = num;
    numberOfElements++;

    if(i < 19)
    {
      cout << "Would you like to add another number? (Y)es Or (N)o " << endl;
      cin >> response;
      if( response != 'Y')
      {
        break;
      }  
    }      
    sum = sum + nums[i];
    cout << sum << " " << nums[i] << endl;
  }
  float avg = float(sum) / float(numberOfElements);
  float sumSDV = 0.0;
  cout << "The average is: " << avg << endl;
  float diff;
  
  for(int i=0; i<=numberOfElements; i++)
  {
    diff = nums[i] - avg;
    sumSDV = sumSDV + (diff * diff);
    
  }      
  float variance = float(diff) / float(numberOfElements);
  float standardDV = sqrt(variance);

  cout << "The variance is " << variance << endl;
  cout << "The standard deviation is " << standardDV << endl;
  return 0;
}  
