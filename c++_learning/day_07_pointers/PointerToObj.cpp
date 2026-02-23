#include <iostream>
#include <string>
using namespace std;

struct Student
{
  int id;
  float mid;
  float final;
};

int main()
{
  Student s1;

  s1.id = 53;
  s1.mid = 76.9;
  s1.final = 80.1;

  Student* p_student;

  p_student = &s1;
  cout << s1.id << endl;
  (*p_student).id = 22;
  cout << s1.id << endl;

  p_student->id = 27;
  cout << s1.id;

  return 0;

}
