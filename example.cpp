// This example may or may not work, I haven't tested it.
// There could be some missing imports as well.
// This also only shows how to parse a vertex buffer.

//ease of use cus why not
void filePutContents(const std::string& name, const std::string& content, bool append = false) {
	std::ofstream outfile;
	if (append)
		outfile.open(name, std::ios_base::app);
	else
		outfile.open(name);
	outfile << content;
}
//again, ease of use

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
      
		int16_t ix;
		int16_t iy;
		int16_t iz;

		fread((char*)&ix, 1, 2, vbFile);
		fread((char*)&iy, 1, 2, vbFile);
		fread((char*)&iz, 1, 2, vbFile);
		//divide by int16 max to get vertex position as float value

		float xPos1((float)(ix / 32767));
		float yPos1((float)(iy / 32767));
		float zPos1((float)(iz / 32767));

		std::string outXYZ;
		outXYZW = "v " + to_str(xPos, 6) + " " + to_str(yPos, 6) + " " + to_str(zPos, 6) + "\n";		
		filePutContents((outputFile + ".obj"), outXYZW, true);
    
		int curoff = ftell(vbFile);
		fseek(vbFile, (curoff + 9), SEEK_SET);
		
		e += 1;
	}
}
