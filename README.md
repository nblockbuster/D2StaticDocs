# Destiny 2 Static Model Extractor

**This wouldn't be possible without the (massive) help from:**
## [Monteven](https://github.com/MontagueM/), [42](https://github.com/hiim42) & [Ginsor](https://twitter.com/ginsorkr)

### Prerequisites
- An unpacker for Destiny 2 (post-beyond light).
- Note: This unpacker must either be custom-made or modified to identify & export vertex buffers, index buffers, "main model" files, "subfile" files, and (optionally) loadzone files.
- Basic to intermediate C++/general coding knowledge.
- (perferrably) a good knowledge of how 3d models work in general

Note: These file codenames are probably unique and not called this by anyone besides me & maybe 42, who had helped me with this adventure tremendously.

## Model Structures

**"Main Model"** files are basically the top-level file for 1 model. This file contains a hash that points to the model's subfile at 0x8, and 3 float32s for scale at 0x3C.

**"Subfile"** files are the next step towards a complete model, being referenced in a Main Model file. Subfiles have 3-4 hashes that each point to a different buffer of the model. The index buffer, vertex buffer, and 2 others, most likely being the  UVs & vertex color.
The buffer hashes can be found at 366D8080 + 4.

**Vertex Buffers** are the most basic form of a model. The file is a list of points, read as a int16 and then divided by the int16 max (32767) to get a float.
Vertex positions are the first 8 bytes of each 16 byte line, with the last 8 bytes of it being the normals. (I haven't figured out normals, so can ignore these)
Reading the X, Y & Z of a vertex is very simple, and should go something like this C++ example:
```c++
fread((char*)&ix, 1, 2, vbuffer);
fread((char*)&iy, 1, 2, vbuffer);
fread((char*)&iz, 1, 2, vbuffer);

float xPos1((float)(ix / 32767));
float yPos1((float)(iy / 32767));
float zPos1((float)(iz / 32767));
```
This is the most simple way to get a model, and all you need to do is skip the next 10 bytes and repeat until the end of the file.

**Index Buffers** are simply a list of int16s, with 3 int16s/triangle/face (kinda obvious) *but* you either need to increase each index by 1, so that whatever program you use to open models doesnt have a freak out when it sees an index of 0.
