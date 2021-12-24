# Destiny 2 Static Model Docs

**This wouldn't be possible without the (massive) help from:**
## [Monteven](https://github.com/MontagueM), [42](https://github.com/hiim42) & [Ginsor](https://twitter.com/ginsorkr)

### Prerequisites
- An unpacker for Destiny 2 (post-beyond light).
- Note: This unpacker must either be custom-made or modified to identify & export vertex buffers, index buffers, "main model" files, "subfile" files, and (optionally) loadzone files.
- Basic to intermediate C++/general coding knowledge.
- (preferably) a good knowledge of how 3d models work in general

Notes: 
- These file codenames are probably unique and not called this by anyone besides me & maybe 42, who had helped me with this adventure tremendously.
- I do all of my coding and documentation in little endian, so classes will be like `AD938080` instead of `808093AD`
- Loadzone files are somewhat *pain*. If you want, you can try and parse them but be warned, it's not very beginner friendly

## Model Structures

#### Main Model
files are basically the top-level file for 1 model. This file contains a hash that points to the model's subfile at 0x8, and 3 float32s for scale at 0x3C.

#### Subfiles
are the next step towards a complete model, being referenced in a Main Model file. Subfiles have 3-4 hashes that each point to a different buffer of the model. The index buffer, vertex buffer, and 2 others, most likely being the  UVs & vertex color.
The buffer hashes can be found at wherever 366D8080 is + 4.

#### Vertex Buffers
are the most basic form of a model. The file is a list of points, read as a int16 and then divided by the int16 max (32767) to get a float.
Vertex positions are the first 8 bytes of each 16 byte line, with the last 8 bytes of it being the normals. (I haven't figured out normals, so can ignore these)
Reading the X, Y & Z of a vertex is very simple, and should go something like this C++ example:
```c++
fread((char*)&intx, 1, 2, vbuffer);
fread((char*)&inty, 1, 2, vbuffer);
fread((char*)&intz, 1, 2, vbuffer);

float x((float)(intx / 32767));
float y((float)(inty / 32767));
float z((float)(intz / 32767));
```
This is the most simple way to get a model, and all you need to do is skip the next 10 bytes and repeat until the end of the file.

### [Full example of how to parse a vertex buffer in C++](example.cpp)

#### Index Buffers 
are simply a list of int16s, with 3 int16s/triangle/face (kinda obvious) *but* you either need to increase each index by 1, so that whatever program you use to open models doesnt have a freak out when it sees an index of 0.

## Classes & Entry types

The different buffers use data from the package's entry table to identify them as what they are.
I use this way of identifying entry types, with numType being `(entryB >> 9) & 0x7F`, and numSubType being `(entryB >> 6) & 0x7`

| Buffer | NumType | NumSubType |
|--------------|------|------|
| Vertex Buffers | 40 | 4 | 
| Index Buffers | 40 | 6 |

The other way of files being identified is through the file's entryA data, as a hex string.
Loadzones, main model files, and subfiles all use this way of identifying the files.
| File type | Class |
|-----------|------------|
| Loadzones | AD938080 |
| Main Models | 446D8080 |
| Subfiles | 306D8080 |

#### If a file's entryA ends in 80 instead of 8080, it will almost always use an entry type.


