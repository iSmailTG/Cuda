#include <iostream>
#include <string>
#include <ctime>
using namespace std;


string flipCoin();
int main()
{
  srand(time(0));
  string flips[10];
  for(int i = 0; i < 10; i++)
  {
    flips[i] = flipCoin();
  }    
  for(int i=0; i < 10; i++)
  {
    cout << i + 1 << ". " << flips[i] << endl;
  }
  return 0;
}  

string flipCoin()
{
  string retVal;
  if(rand() % 2 == 0)
  {
    retVal = "Head";
  }
  else
  {
    retVal = "Tail";
  }

  return retVal;
}
