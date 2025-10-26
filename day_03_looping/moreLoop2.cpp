#include <iostream>
#include <string>
#include <cmath>
using namespace std;

int main()
{
  bool doneYet = false;
  char answer;

  while(!doneYet)
  {
    cout << "Are you Ismail Taghouchti?:(Y or N)  " << endl;
    cin >> answer;
    
    if(answer == 'Y')
    {
      doneYet = true;
      cout << "access granted .." << endl;
    }
    else 
    {
      cout << "Only Ismail Taghouchti could access." << endl;
    }
    
  }    
  return 0;
}  
