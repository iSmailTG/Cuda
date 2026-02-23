#include <iostream>
#include <string>
using namespace std;


int main()
{
  for(int i = 0; i < 50; i++)
  {
    if(i == 21)
    {
      break;
    }  

    if(i % 2 == 0)
    {
      continue;
    }  
    cout << i << endl;
  }  
  return 0;  
}
