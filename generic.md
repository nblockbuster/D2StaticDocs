# Hashes
Hashes in Destiny are 32 or 64 bit integers that are used to reference other files or strings.

### 32 bit hashes
Format: `0x[8 digit hex]`
| Length (bits) | Description          |
| ------------- | -------------------- |
| 10            | Unknown ID           |
| 9             | Reference Package ID |
| 7             | Reference Entry ID   |
| 13            | Reference ID         |
Hashes are used to find what package and where in the package a file is located.
To retrieve the package ID and index in the entry table from a hash, you can use this C++ function:
```c++
void getHashInfo(uint32_t hash, uint32_t& packageID, uint32_t& entryID) {
    packageID = (hash >> 0xD) & 0x3FF;
    entryID = hash % 8192;
}
```

### 64 bit hashes
Format: `0x[16 digit hex]`
64 bit hashes are a bit more complicated than 32 bit hashes, as they are mostly-static hashes that are assigned a 32 bit hash.
In order to get the 32 bit hash from a 64 bit hash, you need to read each package's [hash64 table.](https://github.com/MontagueM/DestinyUnpackerCPP/blob/main/package.cpp#L421)
After that, the 32 bit hash is just a normal 32 bit hash.

#

# Pointers
I'm only going to be covering the most basic pointers, as there are more types I don't want to cover.
## Table Pointers
Table pointers are used to reference a table of data in the same file.
They are 16 bytes long, and are formatted like this:
| Offset | Description              | Type |
| ------ | ------------------------ | ---- |
| 0x0    | Table size               | u64  |
| 0x8    | Relative offset to table | u64  |
## Relative Pointers
Relative pointers are used to reference a spot in the same file.
They are 8 bytes long, and are formatted like this:
| Offset | Description     | Type |
| ------ | --------------- | ---- |
| 0x0    | Relative offset | u64  |