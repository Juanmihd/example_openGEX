////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_loader.h
/// @author Juanmi Huertas Delgado
/// @brief This is the loader to load the files of OpenGEX
/// THis file contains the clas openGEX_loader, that's the class used to read openGEX files
////////////////////////////////////////////////////////////////////////////////

#ifndef OPENGEX_LOADER_INCLUDED
#define OPENGEX_LOADER_INCLUDED

#include "openGEX_parser.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openGEX loader. 
///   To use this class you have to first load the file and then process it
///   To load the file simply use the "load_file" function (passing the name of the file)
///   Once that the file has been loaded correctly, you can analyze it and process it, with 
///   the function "process_resources". This function will copy into a dictionary of resources
///   all of the mesh_instances, and so of the method. 
////////////////////////////////////////////////////////////////////////////////
namespace octet 
{ 
  namespace loaders 
  {
    namespace openGEX_loader
    {
      class openGEX_loader {
        /// Source for the image for reloads
        string url;
        /// Buffer of the file to process
        dynarray<uint8_t> buffer;
      public:
        /// This is the lexer that decodes each block of text from the buffer
        openGEX_parser lexer;
        /// Constructor of the openGEX_loader class
        openGEX_loader(){
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the file with the "name" name of file. First it test if it exists
        /// @param name is a constant pointer to a character (it will be the address of the file to open)
        /// @return It will return true if everything went well and false if there was some problems while reading the file
        ////////////////////////////////////////////////////////////////////////////////
        bool load_file(const char * name){
          app_utils::get_url(buffer, name);
          //Process the file (lexer) (return false if there was any problem)
          printf("Starting the lexer process\n");
          if (!lexer.lexer_file(buffer)){
            printf("Error loading the openGEX file (openDDL process)!\n");
            return false;
          }
          printf("The lexer DDL process has finished successfully!\n");
          return true;
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Process the resources of the file previously processed with the openDDL_lexer
        /// @param dict is a constant pointer to a character (it will be the address of the file to open)
        /// @param skin_skeleton is a constant pointer to a character (it will be the address of the file to open)
        /// @return It will return true if everything went well and false if there was some problems while reading the file
        ////////////////////////////////////////////////////////////////////////////////
        bool process_resources(resource_dict *dict, bool animation, bool skin_skeleton = false){
          printf("And now start the openGEX process...\n");
          if (!lexer.openGEX_data(dict, animation, skin_skeleton)){
            printf("Error loading the openGEX file (openGEX process)!\n");
            return false;
          }
          else
            printf("The openGEX lecture of data went great!\n");
          //Import the file into octet (return false if there was any problem)
          return true;
        }
      };
    }
  }
}

#endif