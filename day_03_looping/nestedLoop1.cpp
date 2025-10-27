#include <iostream>
#include <string>
#include <cmath>
using namespace std;


int main()
{
  int left;
  int right;
  int product;
  

  left = 1;
  while(left < 10)
  {
    right = 1;
    while(right < 10)
    {
      product = left * right;
      cout << left << "X" << right << "=" << product << " ";
      right++;
    }
    left++;
    cout << endl;
  }

  return 0;
}
