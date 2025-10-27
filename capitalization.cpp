#include <iostream>
#include <string>
using namespace std;

int main()
{
  string sentence = "Life is like a marathon, not a sprint!.";
  bool makeCap = false;
  for(int i=0;i < sentence.length(); i++)
  {
    
    if(makeCap)
    {
      char upperC = toupper(sentence[i]);
      cout << upperC;
      makeCap = false;
    }
    
    else
    {
      cout << sentence[i];
    }
    
    if(sentence[i] == ' ')
    {
      makeCap = true;
    }
  }
  cout << endl;
  return 0;
}



























