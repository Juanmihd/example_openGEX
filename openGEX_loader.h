////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_loader.h
/// @author Juanmi Huertas Delgado
/// @brief This is the loader to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////

#ifndef OPENGEX_LOADER_INCLUDED
#define OPENGEX_LOADER_INCLUDED

#include "openGEX_lexer.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openGEX loader, it will open the file and call the lexer and so
////////////////////////////////////////////////////////////////////////////////
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

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Loads the file with the "name" name of file. First it test if it exists
      /// @param name is a constant pointer to a character (it will be the address of the file to open)
      /// @return It will return true if everything went well and false if there was some problems while reading the file
      ////////////////////////////////////////////////////////////////////////////////
      bool loadFile(const char * name){
        app_utils::get_url(buffer, name);
        //Process the file (lexer) (return false if there was any problem)
        printf("Starting the lexer process\n");
        if (!lexer.lexer_file(buffer)){
          printf("Error loading the openGEX file (openDDL process)!\n");
          return false;
        }
        printf("The lexer DDL process has finished successfully!\nAnd now... starting the openGEX process!\n");
        resource_dict dict;
        if (!lexer.openGEX_data(dict)){
          printf("Error loading the openGEX file (openGEX process)!\n");
          return false;
        }
        printf("HUGE SUCCESS!!\n");
        //If it has arrived there, it was ok! so return true
        return true;
      }

      bool process_resources(resource_dict &dict){
        printf("And now start the openGEX process...\n");
        if (!lexer.openGEX_data(dict)){
          printf("Error loading the openGEX file (openGEX process)!\n");
          return false;
        }
        else
          printf("The openGEX lecture of data went great!\n");
        //Import the file into octet (return false if there was any problem)
      }

    };
  }
}

#endif