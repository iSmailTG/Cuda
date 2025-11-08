#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
using namespace std;

int main()
{
  const int NUMS_ROWS = 10;
  const int NUMS_FLIPS_IN_ROW = 12;
  char coinFlip[NUMS_ROWS][NUMS_FLIPS_IN_ROW];
  float percentageOfHeadsInARow[NUMS_ROWS];
  srand(time(0));
  for(int row = 0; row < NUMS_ROWS; row++)
  {
    int numOfHeadsInARow = 0;
    for(int col = 0; col < NUMS_FLIPS_IN_ROW; col++)
    {
      if(rand() % 2 == 0)
      {
        coinFlip[row][col] = 'H';
        numOfHeadsInARow++;

      }
      else
      {
        coinFlip[row][col] = 'T';
      }
      percentageOfHeadsInARow[row]= numOfHeadsInARow / (float)NUMS_FLIPS_IN_ROW;
    }
  }
  for(int row = 0; row < NUMS_ROWS; row++)
  {
    cout << fixed << setprecision(2);
    for(int col = 0; col< NUMS_FLIPS_IN_ROW; col++)
    {
      cout << coinFlip[row][col] << " ";
    }
    
    cout << "| Heads: " << (percentageOfHeadsInARow[row] * 100) << "% ";
    cout << "| Tails: " <<  ((1 - percentageOfHeadsInARow[row]) * 100) << "% ";
    cout << endl;
  }
  return 0;
}
