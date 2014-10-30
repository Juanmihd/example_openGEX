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
#include "openGEX_structures.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
////////////////////////////////////////////////////////////////////////////////
namespace octet
{
  namespace loaders{
    enum { DEBUGDATA = 0, DEBUGSTRUCTURE = 0, DEBUGOPENGEX = 1 };
    class openGEX_lexer : public openDDL_lexer{
      typedef gex_ident::gex_ident_enum gex_ident_list;
      
      //Some values needed to process correctly the file
      //This are the values that are obtained by Metric structures and that define the measurement and orientation
      float distance_multiplier; //default value = 1.0f
      float angle_multiplier; //default value = 1.0f
      float time_multiplier; //default value = 1.0f
      bool z_up_direction; //default value = true  (z), if false (y)
      //This will be used to be able to access to the currentStructure while working with the data_lists.
      openDDL_structure * currentStructure;

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will initialize some structures of the lexer (dictionary of identifiers of openGEX)
      ////////////////////////////////////////////////////////////////////////////////
      void init_gex(){
        //Load the identifiers of openGEX in identifeirs_ dictionary
        for (int i = gex_ident_list::id_Animation; i != gex_ident_list::ident_last; ++i){
          add_identifier(gex_ident::ident_name(i).c_str(), i);
        }
        //Set the default values of the Metric structures
        distance_multiplier = 1.0f;
        angle_multiplier = 1.0f;
        time_multiplier = 1.0f;
        z_up_direction = true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This function prints properly a openDDL_data_literal!
      /// @param  value The literal to be printed
      ////////////////////////////////////////////////////////////////////////////////
      void printfDDLliteral(openDDL_data_literal value){
        switch (value.value_type){
        case value_type_DDL::UINT:
          printf("%u", value.value.u_integer_literal_);
          break;
        case value_type_DDL::INT:
          printf("%i", value.value.integer_);
          break;
        case value_type_DDL::BOOL:
          printf("%s", value.value.bool_ ? "true" : "false");
          break;
        case value_type_DDL::FLOAT:
          printf("%f", value.value.float_);
          break;
        case value_type_DDL::STRING:
          printf("%.*s", value.size_string_, value.value.string_);
          break;
        case value_type_DDL::REF:
          if (value.global_ref_){
            printf("Global ->");
          }
          else{
            printf("Local ->");
          }
          break;
        case value_type_DDL::TYPE:
          printf("%i", value.value.type_);
          break;
        default:
          printf("...ERROR...");
          break;
        }
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This is an aux function to print the nesting!
      ////////////////////////////////////////////////////////////////////////////////
      void printfNesting(){
        for (int i = 0; i < nesting; ++i)
          printf("  ");
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will print the data type structure (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool printf_openGEX_data_type_structure(openDDL_data_type_structure * structure){
        currentStructure = structure;
        int tempID;
        //Obtaining the data_type of the structure
        if (DEBUGSTRUCTURE) printfNesting();
        if (DEBUGSTRUCTURE) printf("The type is: ");
        tempID = (structure)->get_typeID();
        if (tempID < 0)
          printf("(((ERROR)))\n");
        else{
          if (DEBUGSTRUCTURE) printf("%s", ddl_token::token_name(tempID).c_str());
          if (DEBUGSTRUCTURE) printf("\n");
        }
        //Check the name of the structure!
        tempID = structure->get_nameID();
        if (tempID >= 0){
          if (DEBUGSTRUCTURE) printfNesting();
          if (DEBUGSTRUCTURE) printf("The name is ");
          if (DEBUGSTRUCTURE) printf("%s", names_.get_key(tempID));
          if (DEBUGSTRUCTURE) printf(" = %s", structure->get_name());
          if (DEBUGSTRUCTURE) printf("\n");
        }
        else{
          //if (DEBUGSTRUCTURE) printfNesting();
          //if (DEBUGSTRUCTURE) printf("It has no name! (CHECK!)\n");
        }

        //Check what kind of data_type is (if it has integer_literal or not!)
        int integer_literal = structure->get_integer_literal();
        
        //Check now the data inside the data type structure
        int numLists = structure->get_number_lists();
        if (numLists < 0){
          printf("___Weird error with the ammount of data_lists!\n");
        }
        else{
          openDDL_data_list * currentDataList;
          int sizeDataList;
          for (int i = 0; i < numLists; ++i){
            currentDataList = structure->get_data_list(i);
            sizeDataList = currentDataList->data_list.size();
            if(DEBUGDATA) printfNesting();
            if(DEBUGDATA) printf("Occurency %i with %i elements.\n", i+1, sizeDataList);
            for (int j = 0; j < sizeDataList; ++j){
              if (DEBUGDATA) printfNesting();
              if (DEBUGDATA) printf("\t");
              if (DEBUGDATA) printfDDLliteral(*(currentDataList->data_list[j]));
            }
            if (DEBUGDATA) printf("\n");
          }
        }

        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will print the identifier structure (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool printf_openGEX_identifier_structure(openDDL_identifier_structure * structure){
        currentStructure = structure;
        if (DEBUGSTRUCTURE) printf("\n");
        int tempID;
        //Obtaining the identifier of the structure
        if (DEBUGSTRUCTURE) printfNesting();
        if (DEBUGSTRUCTURE) printf("The identifier is: ");
        tempID = (structure)->get_identifierID();
        if (tempID < 0)
          printf("(((ERROR)))\n");
        else{
          if (DEBUGSTRUCTURE) printf("%s", gex_ident::ident_name(tempID).c_str());
          if (DEBUGSTRUCTURE) printf("\n");
        }
        //Check the name of the structure!
        tempID = structure->get_nameID();
        if (tempID >= 0){
          if (DEBUGSTRUCTURE) printfNesting();
          if (DEBUGSTRUCTURE) printf("The name is ");
          if (DEBUGSTRUCTURE) printf("%s", structure->get_name());
          if (DEBUGSTRUCTURE) printf("\n");
        }
        else{
          //if (DEBUGSTRUCTURE) printfNesting();
          //if (DEBUGSTRUCTURE) printf("It has no name! (CHECK!)\n");
        }
        //Check the properties
        int numProperties = structure->get_number_properties();
        if (numProperties > 0){
          openDDL_properties * currentProperty;
          //Let's work with all the properties!
          if (DEBUGSTRUCTURE) printfNesting();
          if (DEBUGSTRUCTURE) printf("The ammount of properties is: %i\n", numProperties);
          for (int i = 0; i < numProperties; ++i){
            currentProperty = structure->get_property(i);
            if (DEBUGSTRUCTURE) printfNesting();
            if (DEBUGSTRUCTURE) printf("Property <");
            tempID = currentProperty->identifierID;
            if (DEBUGSTRUCTURE) printf("%s", identifiers_.get_key(tempID));
            if (DEBUGSTRUCTURE) printf("> with value <");
            if (DEBUGSTRUCTURE) printfDDLliteral(currentProperty->literal);
            if (DEBUGSTRUCTURE) printf(">\n");
          }
        }

        //Check the substructures
        int numSubstructures = structure->get_number_substructures();
        if (numSubstructures > 0){
          if (DEBUGSTRUCTURE) printfNesting();
          if (DEBUGSTRUCTURE) printf("The ammount of substructures is: %i\n", numSubstructures);
          for (int i = 0; i < numSubstructures; ++i){
            openGEX_structure(structure->get_substructure(i));
          }
        }
         
        return true;
      }

      bool openGEX_Metric(openDDL_identifier_structure * structure){

        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will process the identifier structure and store it info into octet (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_identifier_structure(openDDL_identifier_structure * structure){
        currentStructure = structure;
        bool no_error = true;
        int tempID;
        //Obtaining the identifier of the structure
        tempID = (structure)->get_identifierID();
        if (tempID < 0){
          printf("(((ERROR: The identifier of the structure is invalid!)))\n");
          return false;
        }

        if (structure->get_father_structure() == NULL){ //It's a top-level structure!
          switch (tempID){
          case 19: //Metric 
            if (DEBUGOPENGEX) printf("Metric\n");
            //Process Metric structure
            no_error = openGEX_Metric(structure);
            break;
          case 10: //GeometryNode 
            if (DEBUGOPENGEX) printf("GeometryNode\n");
            //Process GeometryNode structure
            break;
          case 11: //GeometryObject
            if (DEBUGOPENGEX) printf("GeometryObject\n");
            //Process GeometryObject structure
            break;
          case 16: //Material
            if (DEBUGOPENGEX) printf("Material\n");
            //Process Material structure
            break;
          }
        }

        //Check the name of the structure!
        tempID = structure->get_nameID();
        if (tempID >= 0){
          if (DEBUGSTRUCTURE) printfNesting();
          if (DEBUGSTRUCTURE) printf("The name is ");
          if (DEBUGSTRUCTURE) printf("%s", structure->get_name());
          if (DEBUGSTRUCTURE) printf("\n");
        }
        else{
          //if (DEBUGSTRUCTURE) printfNesting();
          //if (DEBUGSTRUCTURE) printf("It has no name! (CHECK!)\n");
        }
        //Check the properties
        int numProperties = structure->get_number_properties();
        if (numProperties > 0){
          openDDL_properties * currentProperty;
          //Let's work with all the properties!
          if (DEBUGSTRUCTURE) printfNesting();
          if (DEBUGSTRUCTURE) printf("The ammount of properties is: %i\n", numProperties);
          for (int i = 0; i < numProperties; ++i){
            currentProperty = structure->get_property(i);
            if (DEBUGSTRUCTURE) printfNesting();
            if (DEBUGSTRUCTURE) printf("Property <");
            tempID = currentProperty->identifierID;
            if (DEBUGSTRUCTURE) printf("%s", identifiers_.get_key(tempID));
            if (DEBUGSTRUCTURE) printf("> with value <");
            if (DEBUGSTRUCTURE) printfDDLliteral(currentProperty->literal);
            if (DEBUGSTRUCTURE) printf(">\n");
          }
        }

        //Check the substructures
        int numSubstructures = structure->get_number_substructures();
        if (numSubstructures > 0){
          if (DEBUGSTRUCTURE) printfNesting();
          if (DEBUGSTRUCTURE) printf("The ammount of substructures is: %i\n", numSubstructures);
          for (int i = 0; i < numSubstructures; ++i){
            openGEX_structure(structure->get_substructure(i));
          }
        }

        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will check which type of structure to openGEX (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_structure(openDDL_structure * structure){
        ++nesting;
        bool no_error = true;
        //Check the type of the structure!
        if (structure->get_type_structure() == 0){ //That means that it's a identifier structure!
          if (DEBUGSTRUCTURE) no_error = printf_openGEX_identifier_structure((openDDL_identifier_structure*) structure);
        }
        else{// That means that it's a data_type structure!!
          if (DEBUGSTRUCTURE) no_error = printf_openGEX_data_type_structure((openDDL_data_type_structure*)structure);
        }
        --nesting;
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
        openDDL_structure * topLevelStructure;
        if (DEBUGSTRUCTURE) printf("Starting to reading the file containin %i top-level structures:\n", numStructures);
        for (int i = 0; i < numStructures; ++i){
          //Get next structure
          nesting = 0;
          topLevelStructure = openDDL_file[i];
          //Check the type of the structure and the identificator or data_type!
          if (DEBUGSTRUCTURE) printf("\n-- Top-level structure %i:\n", i + 1);
          openGEX_structure(topLevelStructure);
        }
        if (DEBUGSTRUCTURE) printf("\n");
        return true;
      }
    };

  }
}

#endif