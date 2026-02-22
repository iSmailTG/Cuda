#include <iostream>
#include <string>
using namespace std;

void printBox(int hight, int width);
void expandBox(int& hight, int& width, int expandScalar);

int main()
{
  int hight1box;
  int width1box;

  hight1box = 3;
  width1box = 8;


  for(int i = 0; i <= 2; i++)
  {
    expandBox(hight1box, width1box, 2);
    printBox(hight1box, width1box);
  }

  return 0;
}

void printBox(int hight, int width)
{
  int primeter = (2 * hight) + (2 * width);
  int area = hight * width;

  cout << hight << "X" << width << endl;
   
  for(int row = 1; row <= hight; row++)
  {
    for(int col = 1; col <= width; col++)
    {
      if(row == 1 || row == hight || col == 1 || col == width)
      {
        cout << "*";
      }
      else 
      {
        cout << " ";
      }
    }
    cout << endl;
  }
  cout << primeter <<endl;
  cout << area << endl;

}

void expandBox(int& hight, int& width, int expandScalar)
{
  cout << "Expand the box by Factor of: " << expandScalar << endl;
  hight = hight * expandScalar;
  width = width * expandScalar;


}










