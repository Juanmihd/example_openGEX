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

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will process with openGEX info a data type structure (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_data_type_structure(openDDL_data_type_structure * structure){
        int tempID;
        //Obtaining the data_type of the structure
        printf("The type is: ");
        tempID = (structure)->get_typeID();
        if (tempID < 0)
          printf("(((ERROR)))\n");
        else{
          printf("%s", ddl_token::token_name(tempID).c_str());
          printf("\n");
        }
        //Check the name of the structure!
        tempID = structure->get_nameID();
        if (tempID >= 0){
          printf("The name is ");
          printf("%s", names_.get_key(tempID));
          printf("\n");
        }
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will process with openGEX info a identifier structure (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_identifier_structure(openDDL_identifier_structure * structure){
        int tempID;
        //Obtaining the identifier of the structure
        printf("The identifier is: ");
        tempID = (structure)->get_identifierID();
        if (tempID < 0)
          printf("(((ERROR)))\n");
        else{
          printf("%s", gex_ident::ident_name(tempID).c_str());
          printf("\n");
        }
        //Check the name of the structure!
        tempID = structure->get_nameID();
        if (tempID >= 0){
          printf("The name is ");
          printf("%s", names_.get_key(tempID));
          printf("\n");
        }
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will check which type of structure to openGEX (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_structure(openDDL_structure * structure){
        bool no_error = true;
        //Check the type of the structure!
        if (structure->get_type_structure() == 0){ //That means that it's a identifier structure!
          no_error = openGEX_identifier_structure((openDDL_identifier_structure*) structure);
        }
        else{// That means that it's a data_type structure!!

        }
        return true;
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
      /// @brief This function will analize all the data obtained by the openDDL lexer process
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_data(){
        int numStructures = openDDL_file.size();
        openDDL_structure * bigStructure;
        printf("Starting to reading the file containin %i megaStructures:\n", numStructures);
        for (int i = 0; i < numStructures; ++i){
          //Get next structure
          bigStructure = openDDL_file[i];
          //Check the type of the structure and the identificator or data_type!
          printf("\n-- Structure %i:\n", i+1);
          openGEX_structure(bigStructure);
        }
        printf("\n");
        return true;
      }
    };
  }
}

#endif