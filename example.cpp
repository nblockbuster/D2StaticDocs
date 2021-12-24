#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>

// This only shows how to parse a vertex buffer.

void filePutContents(const std::string& name, const std::string& content, bool append = false) {
	std::ofstream outfile;
	if (append)
		outfile.open(name, std::ios_base::app);
	else
		outfile.open(name);
	outfile << content;
}

template <typename T>
std::string to_str(const T a_value, const int n = 6)
{
	double a = a_value;
	std::stringstream out;
	out << std::fixed << std::setprecision(n) << a;
	return out.str();
}

int main()
{
	std::string vbufferpath;
	std::string outputFile;

	std::cout << "Vertex Buffer path: ";
	std::cin >> vbufferpath;
	std::cout << "Output File Name: ";
	std::cin >> outputFile;

	FILE* vbFile = nullptr;
	fopen_s(&vbFile, vbufferpath.c_str(), "rb");
	int e = 0;
	int c;
	while ((c = fgetc(vbFile)) != EOF)
	{

		if (e < 1)
			fseek(vbFile, 0, SEEK_SET);

		//initialize ints to read the position data to

		int16_t ix;
		int16_t iy;
		int16_t iz;

		//read position data

		fread((char*)&ix, 1, 2, vbFile);
		fread((char*)&iy, 1, 2, vbFile);
		fread((char*)&iz, 1, 2, vbFile);

		//convert to float

		float xPos1((float)ix);
		float yPos1((float)iy);
		float zPos1((float)iz);

		//leftover, scale is found in main model at 0x3C

		int scaleX = 1;
		int scaleY = 1;
		int scaleZ = 1;

		//divide by int16 max to get vertex position as float value

		double xPos((xPos1 / 32767) * scaleX);
		double yPos((yPos1 / 32767) * scaleY);
		double zPos((zPos1 / 32767) * scaleZ);

		std::string outXYZ = "v " + to_str(xPos, 6) + " " + to_str(yPos, 6) + " " + to_str(zPos, 6) + "\n";

		std::string outfile = outputFile + ".obj";
		filePutContents(outfile, outXYZ, true);
		int curoff = ftell(vbFile);
		fseek(vbFile, (curoff + 9), SEEK_SET);

		e += 1;
	}
}
