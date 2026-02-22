#include <iostream>
#include <string>
#include <ctime>
using namespace std;

string returnVal();

int main()
{
  string flips[10];
  srand(time(0));
  
  for(int i = 0; i < 10; i++)
  {
    flips[i] = returnVal();
  }
  for(int i = 0; i < 10; i++)
  {
    cout << i +1 << ". " << flips[i] << endl;

  }

  return 0;
}

string returnVal()
{
  string val;
  if( rand() % 2 == 0)
  {
    val = "Head";
  }
  else
  {
    val = "Tail";

  }
  return val;
}
