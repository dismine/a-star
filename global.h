#include <vector>
#include <utility>

const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 500;

using std::vector;
using std::pair;

template <class T>
class DynamicArray;

// The program is very small so i don't see big dangerous in using global variables.
extern vector<pair<int, int>> coordinates; // point coordinates
extern vector<pair<int, int>> edges; // conncetions
extern DynamicArray<double> map; // contain weights

// I really hate the idea to use raw pointers for two dimensional dynamic array
// This nice template will make my life a lot easier
// 
// Usage:
// DynamicArray<int> my2dArr(2, 2);
// my2dArr[0][0] = -1;
// my2dArr[0][1] = 5;
// cout << my2dArr[0][0] << endl;
// cout << my2dArr[0][1] << endl;
//
template <typename T>
class DynamicArray
{
public:
	DynamicArray();
    DynamicArray(int rows, int cols);

    vector<T>       & operator[](int i);
    const vector<T> & operator[](int i) const;

    void Resize(int rows, int cols);//resize the two dimentional array.
	void Init(); // Fill with zero
private:
    vector<vector<T> > dArray;  
};

template <typename T>
DynamicArray<T>::DynamicArray()
	: dArray()
{}

template <typename T>
DynamicArray<T>::DynamicArray(int rows, int cols)
	: dArray(rows, vector<T>(cols))
{
	Init();
}

template <typename T>
vector<T> & DynamicArray<T>::operator[](int i) 
{ 
	return dArray[i];
}

template <typename T>
const vector<T> & DynamicArray<T>::operator[] (int i) const 
{ 
	return dArray[i];
}

template <typename T>
void DynamicArray<T>::Resize(int rows, int cols)
{
    dArray.resize(rows);
    for(int i = 0; i < rows; ++i)
	{
		dArray[i].resize(cols);
	}
}

template <typename T>
void DynamicArray<T>::Init()
{
    for(unsigned int i = 0; i < dArray.size(); ++i)
	{
		for(unsigned int j = 0; j < dArray[i].size(); ++j)
		{
			dArray[i][j] = 0;
		}
	}
}