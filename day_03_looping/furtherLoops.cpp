
#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
    int size;
    cout << "Enter multiplication table size: ";
    cin >> size;

    cout << "\nMultiplication Table up to " << size << endl;
    cout << "-----------------------------------\n";

    int leftNums = 1;
    while(leftNums <= size)
    {
        int rightNums = 1;
        while(rightNums <= leftNums)
        {
            cout << setw(4) << leftNums * rightNums;
            rightNums++;
        }
        cout << endl;
        leftNums++;
    }

    return 0;
}













