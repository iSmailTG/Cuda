#include <iostream>
#include <string>
#include <ctime>
using namespace std;


int main()
{
  char coinFlip[10][10];
  srand(time(0));
  
  for(int row = 0; row < 10; row++)
  {
    for(int col = 0; col < 10; col++)
    {
      if(rand() % 2 == 0)
      {
        coinFlip[row][col] = 'H';
      }
      else
      {
        coinFlip[row][col] = 'T';
      }
    }
  }
  for(int row = 0; row < 10; row++)
  {
    for(int col = 0; col < 10; col++)
    {
      cout << coinFlip[row][col] << " ";
    }
    cout << endl;
  }
  
  return 0;
}  


































