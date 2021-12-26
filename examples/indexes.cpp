#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>

// This shows how to parse a vertex buffer, and an index buffer.

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
//ease of use cus why not ^
int main()
{
	std::string vbufferpath;
	std::string ibufferpath;
	std::string outputFile;
	std::string outfile;

	std::cout << "Vertex Buffer path: ";
	std::cin >> vbufferpath;
	std::cout << "Index Buffer path: ";
	std::cin >> ibufferpath;
	std::cout << "Output File Name: ";
	std::cin >> outputFile;

	//----------------------------------------------
	//	      vertex buffer parsing
	//----------------------------------------------

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

		outfile = outputFile + ".obj";
		filePutContents(outfile, outXYZ, true);
		int curoff = ftell(vbFile);
		fseek(vbFile, (curoff + 9), SEEK_SET);

		e += 1;
	}

	//getting the amount of vertices written to the file
	unsigned int number_of_lines = 0;
	FILE* infile = nullptr;
	fopen_s(&infile, outfile.c_str(), "r");
	int ch;
	while (EOF != (ch = getc(infile)))
		if ('\n' == ch)
			++number_of_lines;
	printf("%u\n", number_of_lines);
	int vertexCount = (number_of_lines - 1);
	fclose(infile);

	//----------------------------------------------
	//	      index buffer parsing
	//----------------------------------------------
  
	int curoff = 0;
	FILE* ibFile = nullptr;
	//getting the size of the index buffer file
	std::ifstream ibf;
	ibf.open(ibufferpath.c_str(), std::ios_base::binary);
	ibf.seekg(0, ibf.end);
	int ibsize = ibf.tellg();
	ibf.close();
	//open file and begin!
	fopen_s(&ibFile, ibufferpath.c_str(), "rb");
	int ie = 0;
	while (curoff < ibsize)
	{
		if (ie < 1)
		{
			fseek(ibFile, 0, SEEK_SET);
		}
		//init int16's
		int16_t v1; int16_t v2; int16_t v3;
		int16_t vert1; int16_t vert2; int16_t vert3;
		//known issue: just kinda pauses right here when done extracting, so check if model obj is still increasing in size or no (forgot how i fixed it before)
		curoff = ftell(ibFile);
		if (ie < vertexCount)
		{
			//read a tri
			fread((char*)&v1, 1, 2, ibFile);
			fread((char*)&v2, 1, 2, ibFile);
			fread((char*)&v3, 1, 2, ibFile);
			//make a tri
			vert1 = v1 + 1;
			vert2 = v2 + 1;
			vert3 = v3 + 1;
			//obj format writing
			std::string outIndex = "f " + std::to_string(vert1) + " " + std::to_string(vert2) + " " + std::to_string(vert3) + "\n";
			int n = vertexCount + 1;
			//checking if indexes go over amount of vertices
			if (vert1 > n)
			{
				std::cout << "clamped on vert 1 being larger than " + std::to_string(n) << std::endl;
				std::cout << "Indexes: " + outIndex << std::endl;
				return false;
			}
			else if (vert2 > n)
			{
				std::cout << "clamped on vert 2 being larger than " + std::to_string(n) << std::endl;
				std::cout << "Indexes: " + outIndex << std::endl;
				return false;
			}
			else if (vert3 > n)
			{
				std::cout << "clamped on vert 3 being larger than " + std::to_string(n) << std::endl;
				std::cout << "Indexes: " + outIndex << std::endl;
				return false;
			}
			curoff = ftell(ibFile);
			//c++ fuckery i guess
			fseek(ibFile, (curoff + 1 - 1), SEEK_SET);
			ie += 1;
			filePutContents(outfile, outIndex, true);
			//output a tri
		}
	}
	std::cout << "reached end\n";
	exit(1);
	//this didnt fix it, darn
}
