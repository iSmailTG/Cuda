#include <iostream>
#include <string>
#include <cmath>
using namespace std;


int main()
{
  int leftNums;
  int rightNums;
  int product;

  leftNums = 1;
  
  while(leftNums < 10)
  {
    
    rightNums = 1;
    while (rightNums < 10)
    {
      
      product = leftNums * rightNums;
      cout << leftNums << "X" <<  rightNums << "=" << product << " " ;
      rightNums++;
    }
    cout << endl;
    leftNums++;   
  }    





  return 0;
}
