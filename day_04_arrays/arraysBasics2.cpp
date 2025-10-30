#include <iostream>
#include <string>
using namespace std;


int main()
{
  string names[4];
  names[0] = "Ismail";
  names[1] = "Sarah";
  names[2] = "Sifao";
  names[3] = "Yuba";

  int yearOfBirth[] = {1989, 2002, 2028, 2028};
  int currenYear;
  cout << "Enter in the current year: " << endl;
  cin >> currenYear;
  
  for(int i = 0; i < 4; i++)
  {

    cout << "Difference in ages: " << names[i] << " : ";
    for(int j = 0; j < 4; j++)
    {

      if(j != i)  
      {
        cout << names[j] << " " << yearOfBirth[j] - yearOfBirth[i] << " ";
      }        
    }

    cout << endl;
  }
  
  return 0;
}
