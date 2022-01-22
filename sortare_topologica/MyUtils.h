#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include "raylib/raylib.h"

int random_number(const int& a, const int& b)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(a, b);

	return dist(mt);
}

Color random_color()
{
	Color returnColor = { random_number(0,255), random_number(0,255), random_number(0,255), 255 };
	return returnColor;
}

template <typename T>
void matrixAlloc(T**& mat, const unsigned int& m, const unsigned int& n)
{
	mat = new T* [m];
	for (unsigned int i = 0; i < n; i++)
		mat[i] = new T[n];
}

template <typename T>
void matrixDealloc(T** &mat, const unsigned int &m, const unsigned int &n)
{
	for (unsigned int i = 0; i < m; i++)
	{
		delete[] mat[i];
	}
	delete[]mat;
}

template <typename T>
void printMatrixToFile(T** &mat, const unsigned int& m, const unsigned int& n, std::string fileName)
{
	std::ofstream f(fileName);
	f << m << std::endl;
	for (unsigned int i = 0; i < m; i++)
	{
		for (unsigned int j = 0; j < n; j++)
		{
			f << mat[i][j] << " ";
		}
		f << std::endl;
	}
	f.close();
}
void matrixZero(unsigned int** &mat, const unsigned int& m, const unsigned int &n)
{
	for (unsigned int i = 0; i < m; i++)
		for (unsigned int j = 0; j < n; j++)
			mat[i][j] = 0;
}

namespace util
{
	template <typename T>
	void printVector(std::vector<T>& v)
	{
		for (auto it : v)
			std::cout << it << " ";
	}
}