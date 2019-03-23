#ifndef _UTILS
#define _UTILS

#include <string>
#include <stdint.h>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "Vector2D.h"

#if defined (_WIN32)

#include <Windows.h>
#include <math.h>


//WINAPI FILE BROWSER UTILITY FUNCTION

static std::string OpenFileBrowser() {

	char filename[1024];

	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = filename;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = "Visual Development Graph\0*.videg\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn)) {
		return std::string{ filename };
	}

	return "invalid";
}

static std::string SaveFileDialog() {

	char filename[1024];

	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = filename;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = "Visual Development Graph\0*.videg\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetSaveFileName(&ofn)) {
		return std::string{ filename };
	}
	
	return "invalid";
}

static bool OpenMessageDialog(const std::string &title, const std::string &message) {
	int response = MessageBox(NULL, message.c_str(), title.c_str(), MB_YESNO);
	return response == IDYES;
}

#endif


static uint32_t factorial(uint32_t n) {
	return (n <= 0) ? 1 : n * factorial(n - 1);
}

static float nOverK(uint32_t N, uint32_t K) {
	return (float)factorial(N) / ((float)factorial(K)*(float)factorial(N - K));
}

static uint32_t repeated(std::string str, std::string word) {
	uint32_t count = 0;

	size_t ind = ind = str.find(word, 0);
	while (ind != std::string::npos) {
		count++;
		ind = str.find(word, ind + 1);
	}

	return count;
}

static std::string readFile(const std::string &file) {
	std::ifstream reader(file, std::ios::in | std::ios::ate);

	size_t sz = reader.tellg();
	std::string data;
	data.resize(sz + 1);

	reader.seekg(0);
	reader.read(&data[0], sz);

	return data;
}

static bool PointRect(const VIDEKit::Vector2D &point, const VIDEKit::Vector2D &topLeft, const VIDEKit::Vector2D &size) {
	if (point.x < topLeft.x || point.x > topLeft.x + size.x) return false;
	if (point.y < topLeft.y || point.y > topLeft.y + size.y) return false;

	return true;
}

static double clamp(double val, double min, double max) {
	return fmaxf(min, fminf(val, max));
}

#endif //_UTILS