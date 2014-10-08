////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////

#include "openDDL_lexer.h"

/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
namespace octet
{
  namespace loaders{
    class openGEX_lexer : openDDL_lexer{


    public:
      /// @brief Constructor of lexer
      openGEX_lexer(){}

      /// @brief This will be the function that creates de process of the lexer receiving as parameter the array of characters
      bool lexer_file(dynarray<uint8_t> file){
        sizeRead = 0;
        buffer = file;
        currentChar = &buffer[0];
        bufferSize = buffer.size();
        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file()){
          //Process token (in openDDL is a structure) when you find it (when it's not a whiteSpace)
          process_structure();
          //get new token
          get_next_char();
        }

        return true;
      }
    };
  }
}