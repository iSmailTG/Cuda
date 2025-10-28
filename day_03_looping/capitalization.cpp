#include <iostream>
#include <string>
using namespace std;

int main()
{
  string sentence = "Today was a bit sunny.";
  cout << sentence << endl;
  bool isCap = false;
  for(int i=0; i < sentence.length(); i++)
  {
    if(isCap)
    {
      char capL = toupper(sentence[i]);
      cout << capL;
      isCap = false;
    }
    else 
    {
      cout << sentence[i];
    }
    if(sentence[i] == ' ')
    {
      isCap = true;
    }
      
  }
  cout << endl;
  return 0;
}
