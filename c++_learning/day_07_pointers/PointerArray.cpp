#include <iostream>
#include <string>
using namespace std;

void scaling(float* data, int size, float scalar);

int main()
{
  float weights[4] = {0.5f, 1.2f, 3.2f, 1.8f};
  float* ptr = weights;

  cout << "before scaling: " << endl;
  for(int i = 0; i < 4; i++)
  
    cout << "ptr: " << ptr[i] << " || " << "weights: " << weights[i] << endl;
  cout << endl;
  scaling(ptr, 4, 2.0f);
  
  cout << "after scaling: ";

  for(int i = 0; i < 4; i++)
    cout << "ptr: " << ptr[i] << " || " << "weights: " << weights[i] << endl;
  cout << endl;

  return 0;
}


void scaling(float* data, int size, float scalar)
{
  for(int i = 0; i < size; i++)
  { 
    data[i] *= scalar;
  }
}
