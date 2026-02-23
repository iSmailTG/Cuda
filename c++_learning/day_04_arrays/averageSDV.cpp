#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  int nums[] = {22, 33, 44, 5, 6, 61, 77, 87, 76, 55, 6, 33, 4};
  int sum = 0;

  for(int i = 0; i < 13; i++)
  {
    sum = sum + nums[i];

    cout << nums[i] << " " << sum << endl;

  }
  
  float avg = float(sum) / 13;
  cout << "The average of numbers: " << avg << endl;
  
  float diff;
  float sumSDV = 0.0;
  
  for(int i=0; i < 13; i++)
  {
    diff = nums[i] - avg;
    sumSDV = sumSDV + (diff * diff);
  } 
  
  float variance = sumSDV / 13;
  cout << "The variance is : " << variance << endl;
  float standartDV = sqrt(variance);
  cout << "the standart deviation is: " << standartDV;

  return 0;
}  
