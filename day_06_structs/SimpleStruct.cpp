#include <iostream>
#include <string>

using namespace std;


struct Student
{
  string name;
  float gpa;
};

void fillTheStudent(Student& student);
void printStudent(const Student& student);



int main()
{
  Student(s1);
  Student(s2);

  fillTheStudent(s1);
  fillTheStudent(s2);
  
  printStudent(s1);
  printStudent(s2);

  return 0;
}

void fillTheStudent(Student& student)
{
  cout << "fill in the name of the student: ";
  cin >> student.name;
  cout << "fill in the gpa of the student: ";
  cin >> student.gpa;
  cout << endl;
}

void printStudent(const Student& student)
{
  cout << "Name: " << student.name << endl;
  cout << "GPA: " << student.gpa << endl;
}





