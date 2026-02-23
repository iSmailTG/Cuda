#include <iostream>
#include <string>
using namespace std;

struct Name
{
  string first;
  char middleInittial;
  string last;
};
struct Student
{
  Name name;
  float gpa;
};

void fillStudent(Student& student);
void printStudent(const Student& student);

int main()
{
  Student(s1);
  Student(s2);

  fillStudent(s1);
  fillStudent(s2);

  printStudent(s1);
  printStudent(s2);

  return 0;
}

void fillStudent(Student& student)
{
  cout << "Fill in Student's First name: ";
  cin >> student.name.first;

  cout << "Fill in Student's middle name: ";
  cin >> student.name.middleInittial;

  cout << "Fill in Student's last name: ";
  cin >> student.name.last;


  cout << "Fill in Student's GPA: ";
  cin >> student.gpa;
  cout << endl;
}

void printStudent(const Student& student)
{
  cout << "Full Name: " << student.name.first << ", " << student.name.middleInittial << " " << student.name.last << endl;
  cout << "GPA: " << student.gpa;
  cout << endl;
}


















