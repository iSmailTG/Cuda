#include <iostream>
#include <string>
#include <ctime>
using namespace std;

int main()
{
  const int NUMS_ROWS = 10;
  const int NUMS_FLIPS_IN_ROW = 12;
  char coinFlip[NUMS_ROWS][NUMS_FLIPS_IN_ROW];
  srand(time(0));
  for(int row = 0; row < NUMS_ROWS; row++)
  {
    for(int col = 0; col < NUMS_FLIPS_IN_ROW; col++)
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
  for(int row = 0; row < NUMS_ROWS; row++)
  {
    for(int col = 0; col< NUMS_FLIPS_IN_ROW; col++)
    {
      cout << coinFlip[row][col] << " ";
    }
  cout << endl;
  }
  return 0;
}
