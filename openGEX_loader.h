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
      /// Document that has the file
      TiXmlDocument doc;
    public:
      /// Constructor of the openGEX_loader class
      openGEX_loader();

      /// Loads the file with the "name" name of file. First it test if it exists
      bool loadFile(const char * name){
        url = name;
        url.truncate(url.filename_pos());
        const char *path = app_utils::get_path(url);
        char buf[256];
        getcwd(buf, sizeof(buf));
        doc.LoadFile(path);

        TiXmlElement *top = doc.RootElement();

        //Test if the file exists
        if (!top){
          printf("file %s not found\n", path);
          return false;
        }
        //Test file
        return true;
      }
    };
  }
}