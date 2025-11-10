#include <iostream>
#include <string>
#include <ctime>
using namespace std;


int main()
{
  const int NUM_OF_ROWS = 10;
  const int NUM_OF_FLIP_PER_ROW = 15;

  
  char coinFlip[NUM_OF_ROWS][NUM_OF_FLIP_PER_ROW];
  
  srand(time(0));
  float  sumOfRowPercentages = 0.0;
  for(int row = 0; row < NUM_OF_ROWS; row++)
  {

    float percentageOfHeadsInARow[NUM_OF_ROWS];
    int numOfHeadsPerRow = 0;
    for(int col = 0; col < NUM_OF_FLIP_PER_ROW; col++)
    {
      if(rand() % 2 == 0)
      {
        coinFlip[row][col] = 'H';
        numOfHeadsPerRow++;
      }
      else 
      {
        coinFlip[row][col] = 'T';
      }  
    
      
    }
    
    percentageOfHeadsInARow[row] = (float)numOfHeadsPerRow / (float)NUM_OF_FLIP_PER_ROW;

    cout << " | Heads: " << percentageOfHeadsInARow[row] * 100 << "%.";            
    cout << " | Tails: " << ((1 - percentageOfHeadsInARow[row]) * 100) << "%.";    
    cout << endl;                                                                               }

  return 0;
}  
