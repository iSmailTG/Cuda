#include <iostream>
#include <string>
using namespace std;

int main()
{
  string sentence = "I will make it, I'm pretty sure about that.";
  bool capL = false;
  bool capW = false;
  char answer;

  cout << "Pick what do you want to capitalize:\n first letter of each word (L),\n or all words (W)" << endl;
  cin >> answer;
  for(int i = 0; i < sentence.length(); i++)
  {
    if(answer == 'L')
    {
      if(capW)
      {
        char upperW = toupper(sentence[i]);
        cout << upperW;
        capW = false;
      }
      else if(capL)
      {
        char upperL = toupper(sentence[i]);
        cout << upperL;
        capL = false;
      }
      else
      {
        cout << sentence[i];
      }
      if(sentence[i] == ' ')
      {
        if( answer == 'L')
        {
          capL = true;
        }
        else if( answer = 'W')
        {
          capW = true;
        }  
      }  
       
      
    }
  }




  return 0;
}
