#pragma once

#include <iostream>

using namespace std;

struct __declspec(dllexport) Basic
{
	static const size_t size = 8;
	
	int ival[size];
	float fval[size];
	char cval[size];
	
	Basic()
	{
		cout << "Basic Constructor" << endl;
		for (size_t i = 0; i < size; i++)
		{
			ival[i] = i;
			fval[i] = 14.53 + i;
			cval[i] = 'A' + i;
		}
	}
	
	void dump()
	{
		cout << endl << "Basic Dump" << endl;
		for (auto i = 0; i < size; i++)
		{
			cout << ival[i] << " " << fval[i] << " " << cval[i] << endl;
		}
		cout << endl;
	}
};
