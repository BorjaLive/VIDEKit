#include <iostream>

#include "Framework/Window.h"

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <boost/python.hpp>
#endif



int main(int argc, char *argv[]) {

	Py_Initialize();
	
	VIDEKit::Window *window = new VIDEKit::Window("VIDEKit-beta-v1.0", 1024, 768, 120);

	window->proc();


	return 0;
}