#include "isometric.hpp"
using namespace px;
static void loadRegion(std::string const& filePath, Region c) {
        std::ifstream file;
        file.open(filePath);
      
        if (!file.is_open()) {
          throw std::runtime_error("Cannot open file " + filePath);
        }
        
        std::string buf;
        bool is_bracket_opened = false;
        bool is_brace_opened = false;
        bool is_tile_written = false;

        size_t line = 1;
        size_t tile_idx = 0;
        size_t chunk_idx = 0;

        while (std::getline(file, buf)) {
          for (auto x : buf) {
            if (x == '{') {
              if (is_bracket_opened) {
                throw std::runtime_error("Reading " + filePath + ":" + std::to_string(line) + ": No nested curly brackets");
              }
              is_bracket_opened = true;
            }
            else if (x == '}') {
              if (!is_bracket_opened) {
                throw std::runtime_error("Reading " + filePath + ":" + std::to_string(line) + ": Too many curly bracket closures");
              }
              is_bracket_opened = false;
              chunk_idx++;
              tile_idx = 0;
            }
            else if (x == '[') {
              if (!is_bracket_opened) {
                throw std::runtime_error("Reading " + filePath + ":" + std::to_string(line) + ": Container is outside chunk curly brackets\n\tOk: {...[0]...}\n\tBad: {...} [0]");
              }
              if (is_brace_opened) {
                throw std::runtime_error("Reading " + filePath + ":" + std::to_string(line) + ": No nested braces");
              }
              is_brace_opened = true;
            } else if (x == ']') {
              if (!is_bracket_opened) {
                throw std::runtime_error("Reading " + filePath + ":" + std::to_string(line) + ": Ill defined brackets:\n\tOk:{[0]}\n\tBad:{[0}]");
              }
              if (!is_brace_opened) {
                throw std::runtime_error("Reading " + filePath + ":" + std::to_string(line) + ": Too many brace closures");
              }
              is_brace_opened = false;
              is_tile_written = false;
            } else if (x == ' ') {

            } else {
              if (is_brace_opened) {
                if (is_tile_written)
                  throw std::runtime_error("Reading " + filePath + ":" + std::to_string(line) + ": Container is already full");
                std::cout << "Tile " << tile_idx << ": [" << x << "]" << std::endl;

                c.chunks[chunk_idx]->set_tile(tile_idx, (int)x-48);

                is_tile_written = true;
                tile_idx++;
              } else {
                throw std::runtime_error("Reading " + filePath + ":" + std::to_string(line) + ": Invalid charachter \'" + x + "\' outside braces");
              }
            }
          }
          line++;
        }

        if (chunk_idx < int(pow(Region::REGION_SIZE, 2))) {
          throw std::runtime_error("File " + filePath + " ends without defining " + std::to_string(int(pow(Region::REGION_SIZE, 2)) - chunk_idx) + " other chunks");
        }
 
      }