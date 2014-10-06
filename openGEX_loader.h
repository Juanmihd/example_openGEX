////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_loader.h
/// @author Juanmi Huertas Delgado
/// @brief This is the loader to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////
#include "openGEX_lexer.h"

/// @brief This class is the openGEX loader, it will open the file and call the lexer and so
namespace octet { namespace loaders {
    class openGEX_loader {
      /// Source for the image for reloads
      string url;
      /// Buffer of the file to process
      dynarray<uint8_t> buffer;
      /// This is the lexer that decodes each block of text from the buffer
      openGEX_lexer lexer;
    public:
      /// Constructor of the openGEX_loader class
      openGEX_loader(){
      }

      /// Loads the file with the "name" name of file. First it test if it exists
      bool loadFile(const char * name){
        app_utils::get_url(buffer, name);
        //Test file
        return true;
      }

    };
  }
}