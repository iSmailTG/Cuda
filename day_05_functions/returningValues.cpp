#include <iostream>
#include <string>
#include <ctime>
using namespace std;


void flipCoin();
int main()
{
  srand(time(0));
  flipCoin();
  




  return 0;
}  

void flipCoin()
{
  if(rand() % 2 == 0)
  {
    cout << "Head" << endl;
  }
  else
  {
    cout << "Tail" << endl;
  }
}
