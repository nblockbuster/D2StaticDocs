# Destiny 2 Static Model Docs

**This wouldn't be possible without the (massive) help from:**
## [Monteven](https://github.com/MontagueM), [42](https://github.com/hiim42) & [Ginsor](https://twitter.com/ginsorkr)

### Prerequisites
- [An unpacker for Destiny 2](https://github.com/MontagueM/DestinyUnpackerCPP) (post-beyond light).
- Intermediate C++/general coding knowledge.
- (preferably) a good knowledge of how 3d models work in general

Notes: 
- These file codenames are probably unique and not called this by anyone besides me & maybe 42, who had helped me with this adventure tremendously.
- I do all of my coding and documentation in little endian, so classes will be like `AD938080` instead of `808093AD`
- Loadzone files are somewhat *pain*. If you want, you can try and parse them but be warned, it's not very beginner friendly

### Generic Destiny File Structures
Some terms you'll want to learn:
- [Hashes](generic.md#hashes)
- [Pointer Types](generic.md#pointers)

# Model Structures

## Main Model
Main Model files are basically the top-level file for 1 model. They are the files that are referenced in static loadzone files.

| Offset  | Description              | Type          |
| ------- | ------------------------ | ------------- |
| 0x08    | Subfile Hash             | 32 bit hash   |
| 0x18    | Material table           | Table Pointer |
| 0x50-5C | Vertex position offsets? | Vector4       |
| 0x50    | X                        | float32       |
| 0x54    | Y                        | float32       |
| 0x58    | Z                        | float32       |
| 0x5C    | W (Scale)                | float32       |

## Subfiles
Subfiles are the next step towards a complete model, being referenced in a Main Model file. Subfiles have 3-4 hashes that each point to a different buffer of the model. The index buffer, vertex buffer, and 2 others, most likely being the  UVs & vertex color.
The buffer hashes can be found at wherever 366D8080 is + 4.

| Offset | Description               | Type          |
| ------ | ------------------------- | ------------- |
| 0x08   | Material Assignment Table | Table Pointer |
| 0x18   | LOD/Parts Table           | Table Pointer |
| 0x28   | Buffer Table              | Table Pointer |
| 0x48   | UV X Scale                | float32       |
| 0x50   | UV Y Scale                | float32       |
| 0x54   | UV X Offset               | float32       |
| 0x58   | UV Y Offset               | float32       |

## Material Assignments
Size: 0x6
| Offset | Description | Type  |
| ------ | ----------- | ----- |
| 0x00   | Part Index  | short |
| 0x02   | Unknown     | byte  |
| 0x03   | Unknown     | byte  |
| 0x04   | Unknown     | short |

## Levels of Details / Parts
Size: 0xC
The LOD table can be found in the subfile of a model at `0x18` as a table pointer.
The table is a list of 4 byte floats, with the first 2 being the LODs and the last 2 being the material splits.

| Offset | Description         | Type |
| ------ | ------------------- | ---- |
| 0x00   | Index Buffer Offset | u32  |
| 0x04   | Index Buffer Size   | u32  |
| 0x0A   | Detail Level        | byte |
| 0x0B   | Primitive Type      | byte |

[C++ Implementation](https://github.com/nblockbuster/D2StaticExtractor/blob/loadzone_bubble_testing/main.cpp#L437)

 - Primitive type can either be Triangles or Triangle Strips (0x3 or 0x5 respectively) 
 - Detail level is the LOD level, with 1 being the highest, 7 being the lowest, and 0xA being everything else.

## Buffers
Size: 0x14
| Offset | Description         | Type                 |
| ------ | ------------------- | -------------------- |
| 0x00   | Index Buffer        | 32 bit hash          |
| 0x04   | Vertex Buffer       | 32 bit hash          |
| 0x08   | UV Buffer           | 32 bit nullable hash |
| 0x0C   | Vertex Color Buffer | 32 bit nullable hash |

*Nullable here means that the hash can be 0xFFFFFFFF, which means that the buffer doesn't exist.*

## Vertex Buffers
Vertex Buffers are the most basic form of a model. The file is a list of points, read as a int16 and then divided by the int16 max (32767) to get a float. (Sometimes known as a half float)
Vertex positions are the first 8 bytes of each 16 byte line, with the last 8 bytes of it being the normals.
Reading the X, Y & Z of a vertex is very simple, and should go something like this C++ example:

```c++
memcpy((char*)&intx, vbuffer_data + i, 2);
memcpy((char*)&inty, vbuffer_data + i + 2, 2);
memcpy((char*)&intz, vbuffer_data + i + 4, 2);

float x = (float)intx / 32767;
float y = (float)inty / 32767;
float z = (float)intz / 32767;
```
This is the most simple way to get a model, and all you need to do is skip the next 10 bytes (vertex normals) and repeat until the end of the file.

**[Full example of how to parse a vertex buffer in C++](https://github.com/nblockbuster/D2StaticExtractor/blob/loadzone_bubble_testing/parsers.cpp#L3)**

## Index Buffers 
Index Buffers describe the faces of the model and how they connect the vertices. They are simply a list of int16s, with 3 int16s/triangle/face (kinda obvious) *but* for .obj files you need to increase each index by 1, or just use a better format like FBX or Collada.

**[Full example of parsing an index buffer in C++](https://github.com/nblockbuster/D2StaticExtractor/blob/loadzone_bubble_testing/parsers.cpp#L84)**


## In-Memory parsing & FBX

For examples of in-memory parsing & FBX SDK usage, check out [the extractor](https://github.com/nblockbuster/D2StaticExtractor/blob/main/main.cpp)'s code.

## Classes & Entry types

The different buffers use data from the package's entry table to identify them as what they are.
I use this way of identifying entry types, with numType being `(entryB >> 9) & 0x7F`, and numSubType being `(entryB >> 6) & 0x7`

| Buffer         | NumType | NumSubType |
| -------------- | ------- | ---------- |
| Vertex Buffers | 40      | 4          |
| Index Buffers  | 40      | 6          |

The other way of files being identified is through the file's entryA data, as a hex string.
Loadzones, main model files, and subfiles all use this way of identifying the files.
| File type   | Class    |
| ----------- | -------- |
| Loadzones   | AD938080 |
| Main Models | 446D8080 |
| Subfiles    | 306D8080 |
| Materials   | AA6D8080 |

## Materials
How to parse materials can be found [here.](https://github.com/nblockbuster/MontevenDynamicExtractor/blob/main/texture.cpp#L80)
I'm not going to go over how to parse them here, as it's a bit more complicated than the other files.