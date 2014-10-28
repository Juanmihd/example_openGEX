////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
/// NOTE: In the compiler several functions will have the boolean "no_error" this boolean is true when there is NO error, and false when error
///
////////////////////////////////////////////////////////////////////////////////

#ifndef OPENGEX_LEXER_INCLUDED
#define OPENGEX_LEXER_INCLUDED

#include "openDDL_lexer.h"
#include "openGEX_identifiers.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
////////////////////////////////////////////////////////////////////////////////
namespace octet
{
  namespace loaders{
    class openGEX_lexer : public openDDL_lexer{
      typedef gex_ident::gex_ident_enum gex_ident_list;

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will initialize some structures of the lexer (dictionary of identifiers of openGEX)
      ////////////////////////////////////////////////////////////////////////////////
      void init_gex(){
        //Load the identifiers of openGEX in identifeirs_ dictionary
        for (int i = gex_ident_list::id_Animation; i != gex_ident_list::ident_last; ++i){
          add_identifier(gex_ident::ident_name(i).c_str(), i);
        }
      }

    public:
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Constructor of lexer
      ////////////////////////////////////////////////////////////////////////////////
      openGEX_lexer(){
        init_ddl();
        init_gex();
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Constructor of lexer
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_data(){
        int numStructures = openDDL_file.size();
        int tempID;
        openDDL_structure * bigStructure;
        printf("Starting to reading the file containin %i megaStructures:\n", numStructures);
        for (int i = 0; i < numStructures; ++i){
          bigStructure = openDDL_file[i];
          printf("\n-- Structure %i:\n", i);
          printf("Type: ");
          printf((bigStructure->get_type_structure() == 0 ? "identificator\n" : "data_type\n"));
          tempID = ((openDDL_identifier_structure*)bigStructure)->get_identifierID();
          if (tempID < 0)
            printf("(((ERROR)))\n");
          else{
            printf("The identifier is: ");
            printf("%s", gex_ident::ident_name(tempID).c_str());
            printf("\n");
          }
          tempID = bigStructure->get_nameID();
          if (tempID >= 0){
            printf("The name is ");
            printf("%s", names_.get_key(tempID));
            printf("\n");
          }
        }
        printf("\n");
        return true;
      }
    };
  }
}

#endif