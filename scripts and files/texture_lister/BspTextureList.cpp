// This program outputs a list of all used textures in a BSP file
// To use, just pass in a filepath to a BSP file

#include <fstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <set>
#include <string>
#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <istream>

// BSP file structure info here: http://www.flipcode.com/archives/Quake_2_BSP_File_Format.shtml

struct point3f
{
    float x;
    float y;
    float z;
};

struct point3s
{
    int16_t x;
    int16_t y;
    int16_t z;
};

struct bsp_lump
{
    uint32_t offset; // offset (in bytes) of the data from the beginning of the file
    uint32_t length; // length (in bytes) of the data
};

struct bsp_header
{
    uint32_t magic; // magic number ("IBSP")
    uint32_t version; // version of the BSP format (38)
    bsp_lump lump[19]; // directory of the lumps
};

struct bsp_texinfo
{
    point3f u_axis;
    float u_offset;
    point3f v_axis;
    float v_offset;
    uint32_t flags;
    uint32_t value;
    char texture_name[32];
    uint32_t next_texinfo;
};


int main(int argc, char** argv)
{
    // Validate input arguments
    if (argc < 2)
    {
        std::cerr << "No input file specified\n";
        return 0;
    }
    std::string filename = argv[1];

    // Open the BSP file
    std::ifstream file(filename, std::ios::binary);
    if (!file.good() || !file.is_open())
    {
        std::cerr << "Could not open file " << filename.c_str() << "\n";
        return 0;
    }

    // Read the entire BSP file into a buffer
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(fileSize);
    file.read(reinterpret_cast<char*>(&buffer[0]), buffer.size());
    file.close();

    if (buffer.size() < sizeof(bsp_header))
    {
        std::cerr << "Invalid BSP file; header not large enough\n";
        return 0;
    }

    // Do a check on the magic number to make sure it's a BSP file
    const char magic[] = "IBSP";
    bsp_header* headerPtr = reinterpret_cast<bsp_header*>(&buffer[0]);
    if (memcmp(&headerPtr->magic, &magic[0], 4) != 0)
    {
        std::cerr << "Invalid BSP file; magic number mismatch\n";
        return 0;
    }

    // Get the texture node location info
    uint32_t texOffset = headerPtr->lump[5].offset;
    uint32_t texLength = headerPtr->lump[5].length;
    size_t texNodes = texLength / sizeof(bsp_texinfo);
    if (texNodes == 0)
    {
        std::cerr << "Invalid BSP file; no texture data\n";
        return 0;
    }

    // Get a list of all used textures
    std::set<std::string> textures;
    for (size_t i = 0; i < texNodes; ++i)
    {
        bsp_texinfo* texPtr = reinterpret_cast<bsp_texinfo*>(&buffer[texOffset + i * sizeof(bsp_texinfo)]);
        textures.insert(texPtr->texture_name);
    }

    // Output the list of all used textures
    for (auto it = textures.begin(); it != textures.end(); ++it)
    {
        std::cout << "textures/" << *it << "\n";
    }
	system("pause");
    return 0;
}