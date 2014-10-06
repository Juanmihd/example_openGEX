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

    public:
      openGEX_loader();

      void loadFile(const char * name){
        this->url = name;
      }
    };
  }
}