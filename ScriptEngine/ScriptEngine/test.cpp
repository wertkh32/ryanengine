#include <stdio.h>
#include <conio.h>
#include "Python.h"

int main()
{
	char filename[] = "test.py";
	FILE* fp;

	Py_Initialize();

	fp = _Py_fopen(filename, "r");
	PyRun_SimpleFile(fp, filename);

	Py_Finalize();
	_getch();
	return 0;
}