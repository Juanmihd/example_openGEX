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
      bool printf_data_type_structure(openDDL_data_type_structure * structure){
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
      bool printf_identifier_structure(openDDL_identifier_structure * structure){
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
            resource_dict dict;
            openGEX_structure(dict, structure->get_substructure(i));
          }
        }
         
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Metric structure
      /// @param  structure This is the structure to be analized, it has to be Metric.
      /// @return True if everything went well, false if there was some problem
      ///   Note: This function will check the property of type key, and, depending on it's value
      ///   it will update the proper info (distance, angle, time, or up)
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Metric(openDDL_identifier_structure *structure){
        int tempID;
        openDDL_data_type_structure *data_structure = NULL;
        bool no_error = true;
        //Check the properties
        int numProperties = structure->get_number_properties();
        if (numProperties == 1){
          openDDL_properties *currentProperty;
          currentProperty = structure->get_property(0);
          //Check that the property is correct!
          tempID = identifiers_.get_value(currentProperty->identifierID);
          if (tempID != 43){ // 43 = key
            printf("(((ERROR: The property in Metric has to be key and it's %i)))\n",tempID);
            return false;
          }
          else{
            //Check the value of the property key (distance, angle, time or up)
            char * value = currentProperty->literal.value.string_;
            int size = currentProperty->literal.size_string_;
            //Now it has to check which key is. So let's check the size first (faster!)
            switch (size){
            case 2: //check with up
              if (same_word(value, size, "up", size)){
                if(DEBUGOPENGEX) printf("Obatining up. Expecting data_list one element: string.\n");
                // Get data structure
                data_structure = (openDDL_data_type_structure *)structure->get_substructure(0);
                if (data_structure->get_type_structure() == 1 && data_structure->get_typeID() == 11){
                  // Obtain data
                  char * value = data_structure->get_data_list(0)->data_list[0]->value.string_;
                  int value_size = data_structure->get_data_list(0)->data_list[0]->size_string_;
                  //Analyze data (it can be "z" or "y")
                  if (value_size == 1){
                    if (value[0] == 'z') z_up_direction = true;
                    else if (value[0] == 'y') z_up_direction = false;
                    else no_error = false;
                    if (DEBUGOPENGEX) printf(z_up_direction ? "The new direction is z!\n" : "The new direction is y!\n");
                  }
                  else no_error = false;
                }
                else no_error = false;
                //After analyzing the value for metric, if there was an error...
                if (!no_error){
                  printf("(((ERROR: The substructure is not correct for the key=up in Metric)))\n");
                  return false;
                }
              }
              else{
                printf("(((ERROR: The property key in Metric has a wrong value!)))\n");
              }
              break;
            case 4: //check with time
              if (same_word(value, size, "time", size)){
                if (DEBUGOPENGEX) printf("Obtaining time. Expecting data_list one element: float.\n");
                // Get data structure
                data_structure = (openDDL_data_type_structure *)structure->get_substructure(0);
                if (data_structure->get_type_structure() == 1 && data_structure->get_typeID() == 9){
                  // Obtain data
                  float value = data_structure->get_data_list(0)->data_list[0]->value.float_;
                  time_multiplier = value;
                  if (DEBUGOPENGEX) printf("The new time multiplier is %f\n", time_multiplier);
                }
                else{
                  printf("(((ERROR: The substructure is not correct for the key=up in Metric)))\n");
                  return false;
                }
              }
              else{
                printf("(((ERROR: The property key in Metric has a wrong value!)))\n");
              }
              break;
            case 5: //check with angle
              if (same_word(value, size, "angle", size)){
                if (DEBUGOPENGEX) printf("Obtaining angle. Expecting data_list one element: float.\n");
                // Get data structure
                data_structure = (openDDL_data_type_structure *)structure->get_substructure(0);
                if (data_structure->get_type_structure() == 1 && data_structure->get_typeID() == 9){
                  // Obtain data
                  float value = data_structure->get_data_list(0)->data_list[0]->value.float_;
                  angle_multiplier = value;
                  if (DEBUGOPENGEX) printf("The new angle multiplier is %f\n", angle_multiplier);
                }
                else{
                  printf("(((ERROR: The substructure is not correct for the key=up in Metric)))\n");
                  return false;
                }
              }
              else{
                printf("(((ERROR: The property key in Metric has a wrong value!)))\n");
              }
              break;
            case 8: //check with distance
              if (same_word(value, size, "distance", size)){
                if (DEBUGOPENGEX) printf("Obtaining distance. Expecting data_list one element: float.\n");
                // Get data structure
                data_structure = (openDDL_data_type_structure *)structure->get_substructure(0);
                if (data_structure->get_type_structure() == 1 && data_structure->get_typeID() == 9){
                  // Obtain data
                  float value = data_structure->get_data_list(0)->data_list[0]->value.float_;
                  distance_multiplier = value;
                  if (DEBUGOPENGEX) printf("The new distance multiplier is %f\n", distance_multiplier);
                }
                else{
                  printf("(((ERROR: The substructure is not correct for the key=up in Metric)))\n");
                  return false;
                }
              }
              else{
                printf("(((ERROR: The property key in Metric has a wrong value!)))\n");
              }
              break;
            default:
              {
                printf("(((ERROR: The property key in Metric has a wrong value!)))\n");
              }
              break;
            }
          }
        }
        //If it has a different number of properties, it's an error!
        else{
          printf("(((ERROR: Metric has a wrong number of properties)))\n");
          return false;
        }
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Name structure
      /// @param  name  This is a pointer to char, it will return here the value of the Name
      /// @param  structure This is the structure to be analized, it has to be Name.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Name(char *name, int nameSize, openDDL_identifier_structure *structure){
        bool no_error = true;
        if (structure->get_number_properties() != 0){
          printf("(((ERROR: A structure of the type Name cannot have properties)))\n");
          no_error = false;
        }
        else if (structure->get_number_substructures() != 1){
          printf("(((ERROR: A structure of the type Name has to have one substructure)))\n");
          no_error = false;
        }
        else{
          //Obtain the data list of the substructure
          openDDL_data_list * data_list_name = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
          if (data_list_name->value_type != value_type_DDL::STRING){
            printf("(((ERROR: The type of the substructure data of the structure Name has to be string)))\n");
            no_error = false;
          }
          else{
            name = data_list_name->data_list[0]->value.string_;
            nameSize = data_list_name->data_list[0]->size_string_;
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a ObjectRef structure
      /// @param  name  This is a pointer to char, it will return here the value of the ObjectRef
      /// @param  structure This is the structure to be analized, it has to be ObjectRef.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_ObjectRef(mesh *ref, openDDL_identifier_structure *structure){
        bool no_error = true;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a MaterialRef structure
      /// @param  name  This is a pointer to char, it will return here the value of the MaterialRef
      /// @param  structure This is the structure to be analized, it has to be MaterialRef.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_MaterialRef(material *ref, openDDL_identifier_structure *structure){
        bool no_error = true;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Morph structure
      /// @param  values  This is a pointer to float, it will return here the values of the Morph
      /// @param  numValues  This will return the size of values
      /// @param  structure This is the structure to be analized, it has to be Morph.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Morph(float *values, int &numValues, openDDL_identifier_structure *structure){
        bool no_error = true;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Transform structure
      /// @param  name  This is a pointer to char, it will return here the value of the Transform
      /// @param  object_only  This is a boolean saying if this is going to be applied only to one object
      /// @param  structure This is the structure to be analized, it has to be Transform.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Transform(mat4t &transformMatrix, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;
        //Check that the structure is correct!
        //Get the value of the properties!
        if (structure->get_number_properties() > 1){
          no_error = false;
          printf("(((ERROR! The structure Transform can have 0 or 1 properties only!!)))\n");
        }
        else if (structure->get_number_properties() == 1){
          //If has one property, obtain it, it should be a "object" type of property!
          openDDL_properties * current_property = structure->get_property(0);
          object_only = current_property->literal.value.bool_;
        }
        //Check that the substructures are correct!
        if (structure->get_number_substructures() != 1){
          printf("(((ERROR!! The data substructure of the structure Transform has to be only one!!)))");
          no_error = false;
        } else{
        //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
          float values[16];
          openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
          for (int i = 0; i < 16; ++i){
            values[i] = data_list_values->data_list[i]->value.float_;
          }
          //Obtain the matrix from this values
          transformMatrix.init_transpose(values);
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Translation structure
      /// @param  name  This is a pointer to char, it will return here the value of the Translation
      /// @param  coordinates  This represent which coordinate is used in the transform. (3 = all of them)
      /// @param  object_only  This is a boolean saying if this is going to be applied only to one object
      /// @param  structure This is the structure to be analized, it has to be Translation.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Translate(mat4t &transformMatrix, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;
        int coordinates = 3;
        //Check that the structure is correct!
        //Get the value of the properties!
        int numProperties = structure->get_number_properties();
        if (numProperties > 2){
          no_error = false;
          printf("(((ERROR! The structure Translate can have 0, 1 or 2 properties only!!)))\n");
        }
        else{
          for (int i = 0; i < numProperties && no_error; ++i){
            openDDL_properties * current_property = structure->get_property(i);
            int typeProperty = identifiers_.get_value(current_property->identifierID);
            if (typeProperty == 49){//object
              object_only = current_property->literal.value.bool_;
            }
            else if (typeProperty == 44){//kind
              if (current_property->literal.size_string_ == 1)
                switch (current_property->literal.value.string_[0]){
                case 'x':
                  coordinates = 0;
                  break;
                case 'y':
                  coordinates = 1;
                  break;
                case 'z':
                  coordinates = 2;
                  break;
                }
              else{
                coordinates = 3;
              }
            }
            else{
              printf("(((ERROR: This cannot be a property of this structure!)))\n");
              no_error = false;
            }
          }
        }
        //Check that the substructures are correct!
        if (structure->get_number_substructures() != 1){
          printf("(((ERROR!! The data substructure of the structure Transform has to be only one!!)))");
          no_error = false;
        }
        else{
          //Obtain the matrix from this values
          transformMatrix.loadIdentity();
          if (coordinates == 3){
            //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
            openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
            for (int i = 0; i < 3; ++i){
              transformMatrix[i][3] = data_list_values->data_list[i]->value.float_;
            }
          }
          else{
            //If it's only one coordinate, change that one with the value in the structure
            openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
            transformMatrix[coordinates][3] = data_list_values->data_list[0]->value.float_;
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Translation structure
      /// @param  name  This is a pointer to char, it will return here the value of the Translation
      /// @param  coordinates  This represent which coordinate is used in the transform. (3 = all of them, 4 = quaternion)
      /// @param  object_only  This is a boolean saying if this is going to be applied only to one object
      /// @param  structure This is the structure to be analized, it has to be Translation.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Rotate(mat4t &transformMatrix, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;
        int coordinates = 3;
        //Check that the structure is correct!
        //Get the value of the properties!
        int numProperties = structure->get_number_properties();
        if (numProperties > 2){
          no_error = false;
          printf("(((ERROR! The structure Rotate can have 0, 1 or 2 properties only!!)))\n");
        }
        else{
          for (int i = 0; i < numProperties && no_error; ++i){
            openDDL_properties * current_property = structure->get_property(i);
            int typeProperty = identifiers_.get_value(current_property->identifierID);
            if (typeProperty == 49){//object
              object_only = current_property->literal.value.bool_;
            }
            else if (typeProperty == 44){//kind
              if (current_property->literal.size_string_ == 1){
                switch (current_property->literal.value.string_[0]){
                case 'x':
                  coordinates = 0;
                  break;
                case 'y':
                  coordinates = 1;
                  break;
                case 'z':
                  coordinates = 2;
                  break;
                }
              }
              else if (current_property->literal.size_string_ == 4)
                coordinates = 3;
              else
                coordinates = 4;
            }
            else{
              printf("(((ERROR: This cannot be a property of this structure!)))\n");
              no_error = false;
            }
          }
        }
        //Check that the substructures are correct!
        if (structure->get_number_substructures() != 1){
          printf("(((ERROR!! The data substructure of the structure Transform has to be only one!!)))");
          no_error = false;
        }
        else{
          if (coordinates == 4){ //Quaternion!
            vec4 values;
            //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
            openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
            for (int i = 0; i < 4; ++i){
              values[i] = data_list_values->data_list[i]->value.float_;
            }
            //Obtain the matrix from this values
            quat quaternion(values);
            transformMatrix = mat4t (quaternion);
          }
          else if (coordinates == 3){
            //If it's axis, change that one with the value in the structure
            float x,y,z;
            float angle;
            //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
            openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
            angle = data_list_values->data_list[0]->value.float_;
            x = data_list_values->data_list[1]->value.float_;
            y = data_list_values->data_list[2]->value.float_;
            z = data_list_values->data_list[3]->value.float_;
            //Obtain the matrix from this values
            float cosAngle = cosf(angle * (3.14159265f / 180));
            float sinAngle = sinf(angle * (3.14159265f / 180));
            transformMatrix = mat4t (
              vec4(x*x*(1 - cosAngle) + cosAngle,   x*y*(1 - cosAngle) + z*sinAngle, x*z*(1 - cosAngle) - y*sinAngle, 0.0f),
              vec4(x*y*(1 - cosAngle) - z*sinAngle, y*y*(1 - cosAngle) + cosAngle,   y*z*(1 - cosAngle) + x*sinAngle, 0.0f),
              vec4(x*z*(1 - cosAngle) + y*sinAngle, y*z*(1 - cosAngle) - x*sinAngle, z*z*(1 - cosAngle) + cosAngle,   0.0f),
              vec4(                           0.0f,                            0.0f,                          0.0f,   1.0f)
              );
          }
          else {
            //If it's only one coordinate, change that one with the value in the structure
            float angle;
            openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
            angle = data_list_values->data_list[0]->value.float_;
            //Obtain the matrix from this values
            float cosAngle = cosf(angle * (3.14159265f / 180));
            float sinAngle = sinf(angle * (3.14159265f / 180));
            switch (coordinates){
            case 0: //x
              transformMatrix = mat4t(
                vec4(1.0f,      0.0f,      0.0f, 0.0f),
                vec4(0.0f,  cosAngle, -sinAngle, 0.0f),
                vec4(0.0f, sinAngle,  -cosAngle, 0.0f),
                vec4(0.0f,      0.0f,      0.0f, 1.0f)
                );
              break;
            case 1: //y
              transformMatrix = mat4t(
                vec4( cosAngle, 0.0f, sinAngle, 0.0f),
                vec4(     0.0f, 1.0f,     0.0f, 0.0f),
                vec4(-sinAngle, 0.0f, cosAngle, 0.0f),
                vec4(     0.0f, 0.0f,     0.0f, 1.0f)
                );
              break;
            case 2: //z
              transformMatrix = mat4t(
                vec4(cosAngle, -sinAngle, 0.0f, 0.0f),
                vec4(sinAngle,  cosAngle, 0.0f, 0.0f),
                vec4(    0.0f,      0.0f, 0.0f, 0.0f),
                vec4(    0.0f,      0.0f, 0.0f, 1.0f)
                );
              break;
            }
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Translation structure
      /// @param  name  This is a pointer to char, it will return here the value of the Translation
      /// @param  coordinates  This represent which coordinate is used in the transform. (3 = all of them)
      /// @param  object_only  This is a boolean saying if this is going to be applied only to one object
      /// @param  structure This is the structure to be analized, it has to be Translation.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Scale(mat4t &transformMatrix, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;
        int coordinates = 3;
        //Check that the structure is correct!
        //Get the value of the properties!
        int numProperties = structure->get_number_properties();
        if (numProperties > 2){
          no_error = false;
          printf("(((ERROR! The structure Scale can have 0, 1 or 2 properties only!!)))\n");
        }
        else{
          for (int i = 0; i < numProperties && no_error; ++i){
            openDDL_properties * current_property = structure->get_property(i);
            int typeProperty = identifiers_.get_value(current_property->identifierID);
            if (typeProperty == 49){//object
              object_only = current_property->literal.value.bool_;
            }
            else if (typeProperty == 44){//kind
              if (current_property->literal.size_string_ == 1)
                switch (current_property->literal.value.string_[0]){
                case 'x':
                  coordinates = 0;
                  break;
                case 'y':
                  coordinates = 1;
                  break;
                case 'z':
                  coordinates = 2;
                  break;
              }
              else{
                coordinates = 3;
              }
            }
            else{
              printf("(((ERROR: This cannot be a property of this structure!)))\n");
              no_error = false;
            }
          }
        }
        //Check that the substructures are correct!
        if (structure->get_number_substructures() != 1){
          printf("(((ERROR!! The data substructure of the structure Transform has to be only one!!)))");
          no_error = false;
        }
        else{
          if (coordinates == 3){
            //Obtain the matrix from this values
            transformMatrix.loadIdentity();
            //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
            openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
            for (int i = 0; i < 3; ++i){
              transformMatrix[i][i] = data_list_values->data_list[i]->value.float_;
            }
          }
          else{
            //If it's only one coordinate, change that one with the value in the structure
            //Obtain the matrix from this values
            transformMatrix.loadIdentity();
            openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
            transformMatrix[coordinates][coordinates] = data_list_values->data_list[0]->value.float_;
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Node structure
      /// @param  dict This is the resource where everything needs to be stored.
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Node(resource_dict &dict, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a VertexArray structure
      /// @param  dict This is the resource where everything needs to be stored.
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_VertexArray(vec3 *positions, vec3 *normals, vec3 *uvw, int &num_vertexes, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;
        //Get the value of the properties!
        char * attrib_value = NULL;
        int attrib_size = 0;
        int morph_index = 0;
        int current_attrib = 0;
        bool secondary_position = false;
        int numProperties = structure->get_number_properties();
        if (numProperties > 2){
          no_error = false;
          printf("(((ERROR! The structure Scale can have 0, 1 or 2 properties only!!)))\n");
        }
        else{
          for (int i = 0; i < numProperties && no_error; ++i){
            openDDL_properties * current_property = structure->get_property(i);
            int typeProperty = identifiers_.get_value(current_property->identifierID);
            if (typeProperty == 36){//attrib
              //attrib properties can be "position", "normal", "tangent", "bitangent", "color", and "texcoord"
              //also, they can be finished in an optional [0] or [1] where having [1] means an secondary value
              //This will translate those words to the following values:
              // position = 0, normal = 1, texcoord = 2, tangent = 3, bitangent = 4, color = 5
              // This will make easier the next process of understanding the floats of this substructure
              attrib_value = current_property->literal.value.string_;
              attrib_size = current_property->literal.size_string_;
              if (attrib_value[attrib_size - 1] == ']'){//Then the array will have 3 more elements ([i])
                secondary_position = (attrib_value[attrib_size - 2] == 1);
                attrib_size -= 3;
              }
              //Check with the size first, to identify the candidates values
              switch (attrib_size){
              case 5://color
                if (same_word("color", attrib_value, attrib_size)){
                  current_attrib = 5;
                }
                else{
                  printf("(((ERRROR!! This is not a valid value for attrib)))\n");
                  no_error = false;
                }
                break;
              case 6://normal
                if (same_word("normal", attrib_value, attrib_size)){
                  current_attrib = 1;
                }
                else{
                  printf("(((ERRROR!! This is not a valid value for attrib)))\n");
                  no_error = false;
                }
                break;
              case 7://tangent
                if (same_word("tangent", attrib_value, attrib_size)){
                  current_attrib = 3;
                }
                else{
                  printf("(((ERRROR!! This is not a valid value for attrib)))\n");
                  no_error = false;
                }
                break;
              case 8://position or texcoord
                if (same_word("position", attrib_value, attrib_size)){
                  current_attrib = 0;
                }
                else if (same_word("texcoord", attrib_value, attrib_size)){
                  current_attrib = 2;
                }
                else{
                  printf("(((ERRROR!! This is not a valid value for attrib)))\n");
                  no_error = false;
                }
                break;
              case 9://bitangent
                if (same_word("bitangent", attrib_value, attrib_size)){
                  current_attrib = 4;
                }
                else{
                  printf("(((ERRROR!! This is not a valid value for attrib)))\n");
                  no_error = false;
                }
                break;
              }
            }
            else if (typeProperty == 47){//morph
              morph_index = current_property->literal.value.u_integer_literal_;
            }
            else{
              printf("(((ERROR: This cannot be a property of this structure!)))\n");
              no_error = false;
            }
          }
        }
        //Check the substructure (only one!!!)
        if (structure->get_number_substructures() != 1){
          no_error = false;
          printf("(((ERROR! The VertexArray has to have a substructure, only one, but at least one!)))\n");
        }
        else{
          openDDL_data_type_structure *substructure = (openDDL_data_type_structure *) structure->get_substructure(0);
          openDDL_data_list *data_list;
          int size_data_list = substructure->get_integer_literal();
          int number_data_lists = substructure->get_number_lists();
          if (size_data_list == 1){
            data_list = substructure->get_data_list(0);
            num_vertexes = data_list->data_list.size();
            openDDL_data_literal * new_data_list = data_list->data_list[0];
            float a, b, c;
            if (current_attrib == 0){ //pos
              if (positions == NULL)
                positions = new vec3[num_vertexes];
              for (int i = 0; i < num_vertexes; ++i){
                a = new_data_list->value.float_;
                ++new_data_list;
                b = new_data_list->value.float_;
                ++new_data_list;
                c = new_data_list->value.float_;
                ++new_data_list;
                positions[i] = vec3(a, b, c);
              }
            }
            else if (current_attrib == 1){//normal
              if (normals == NULL)
                normals = new vec3[num_vertexes];
              for (int i = 0; i < num_vertexes; ++i){
                a = new_data_list->value.float_;
                ++new_data_list;
                b = new_data_list->value.float_;
                ++new_data_list;
                c = new_data_list->value.float_;
                ++new_data_list;
                normals[i] = vec3(a, b, c);
              }
            }
            else if (current_attrib == 2){//uv
              if (uvw == NULL)
                uvw = new vec3[num_vertexes];
              for (int i = 0; i < num_vertexes; ++i){
                a = new_data_list->value.float_;
                ++new_data_list;
                b = new_data_list->value.float_;
                ++new_data_list;
                uvw[i] = vec3(a, b, 1);
              }
            }
          }
          else{
            num_vertexes = number_data_lists;
            if (size_data_list == 2 && current_attrib == 2){ //uv
              if (uvw == NULL)
                uvw = new vec3[num_vertexes];
              for (int i = 0; i < number_data_lists; ++i){
                data_list = substructure->get_data_list(i);
                uvw[i] = vec3(data_list->data_list[0]->value.float_,
                               data_list->data_list[1]->value.float_,
                               1);
              }
            }
            else if (size_data_list == 3){
              if (current_attrib == 0){ //pos
                if (positions == NULL)
                  positions = new vec3[num_vertexes];
                for (int i = 0; i < number_data_lists; ++i){
                  data_list = substructure->get_data_list(i);
                  positions[i] = vec3(data_list->data_list[0]->value.float_,
                                       data_list->data_list[1]->value.float_, 
                                       data_list->data_list[2]->value.float_);
                }
              }
              else if (current_attrib == 1){ //normal
                if (normals == NULL)
                  normals = new vec3[num_vertexes];
                for (int i = 0; i < number_data_lists; ++i){
                  data_list = substructure->get_data_list(i);
                  normals[i] = vec3(data_list->data_list[0]->value.float_,
                                     data_list->data_list[1]->value.float_,
                                     data_list->data_list[2]->value.float_);
                }
              }
              else{
                no_error = false;
                printf("(((ERROR! There is some problem with the amount of elements in the VertexArray...)))\n");
              }
            }
            else if (size_data_list >= 4){
              //This case is not yet studied
              printf("(((ERROR! This case has not yet been developed!)))\n");
            }
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a IndexArray structure
      /// @param  dict This is the resource where everything needs to be stored.
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_IndexArray(uint32_t **indices, int &num_indices, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;
        //Get the value of the properties!
        bool clock_wise = false;
        int material_index = 0;
        int numProperties = structure->get_number_properties();
        int restart = -1;
        if (numProperties > 2){
          no_error = false;
          printf("(((ERROR! The structure Scale can have 0, 1 or 2 properties only!!)))\n");
        }
        else{
          for (int i = 0; i < numProperties && no_error; ++i){
            openDDL_properties * current_property = structure->get_property(i);
            int typeProperty = identifiers_.get_value(current_property->identifierID);
            if (typeProperty == 46){//material
              material_index = current_property->literal.value.u_integer_literal_;
            }
            else if (typeProperty == 51){//restart
              restart = current_property->literal.value.u_integer_literal_;
            }
            else if (typeProperty == 41){//front
              //front can ve ccw or cw, so first check the size of the string
              if (current_property->literal.size_string_ == 2){
                if (same_word("cw", current_property->literal.value.string_, 2))
                  clock_wise = true;
                else{
                  no_error = false;
                  printf("(((ERRROR: The string in the property front is wrong!)))\n");
                }
              }
              else if(current_property->literal.size_string_ == 3){
                if (same_word("ccw", current_property->literal.value.string_, 3))
                  clock_wise = false;
                else{
                  no_error = false;
                  printf("(((ERRROR: The string in the property front is wrong!)))\n");
                }
              }
              else{
                no_error = false;
                printf("(((ERRROR: The string in the property front has a wrong size!)))\n");
              }
            }
            else{
              printf("(((ERROR: This cannot be a property of this structure!)))\n");
              no_error = false;
            }
          }
        }
        //Check substructures (it has to have one! and it will be a data_list or data_list_array of uints)
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Skin structure
      /// @param  dict This is the resource where everything needs to be stored.
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Skin(mesh *current_mesh, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Mesh structure
      /// @param  dict This is the resource where everything needs to be stored.
      /// @param  lod Level of detail (it has to be different for any mesh in a same GeometryObject
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Mesh(mesh *current_mesh, int &lod, openDDL_identifier_structure *structure){
        bool no_error = true;
        int tempID;
        char * new_primitive;
        int size_primitive;
        //Check properties (lod and primitive)
        int numProperties = structure->get_number_properties();
        for (int i = 0; i < numProperties; ++i){
          openDDL_properties *currentProperty;
          currentProperty = structure->get_property(i);
          tempID = identifiers_.get_value(currentProperty->identifierID);
          switch (tempID){
          case 45:
            //Property lod
            lod = currentProperty->literal.value.u_integer_literal_;
            break;
          case 50:
            //Property primitive
            size_primitive = currentProperty->literal.size_string_;
            new_primitive = currentProperty->literal.value.string_;
            break;
          default:
            printf("(((ERROR: Property %i non valid!)))\n", tempID);
            break;
          }
        }
        //Check substructures (VertexArray (1 or more), IndexArray (0 or 1), Skin (0 or 1))
        int numSubstructures = structure->get_number_substructures();
        int numVertexArray = 0;
        int numIndexArray = 0;
        int numSkin = 0;
        vec3 *positions = NULL;
        vec3 *normals = NULL;
        vec3 *uvw = NULL;
        uint32_t **indices = NULL;
        int num_vertexes, num_indices;
        //Check all the substructures (all of them has to be of mesh type)
        for (int i = 0; i < numSubstructures && no_error; ++i){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          switch (tempID){
          case 35://VertexArray
            ++numVertexArray;
            no_error = openGEX_VertexArray(positions, normals, uvw, num_vertexes, substructure);
            break;
          case 12://IndexArray
            if (numIndexArray == 0){
              ++numIndexArray;
              no_error = openGEX_IndexArray(indices, num_indices, substructure);
            }
            else{
              no_error = false;
              printf("(((ERROR: The structure Mesh can only have one IndexArray substructure)))\n");
            }
            break;
          case 28://Skin
            if (numSkin == 0){
              ++numSkin;
              no_error = openGEX_Skin(current_mesh, substructure);
            }
            else{
              no_error = false;
              printf("(((ERROR: The structure Mesh can only have one Skin substructure)))\n");
            }
            break;
          default:
            no_error = false;
            printf("(((ERROR: The structure Mesh can only have as substructure a Mesh)))\n");
            break;
          }
        }
        if (numVertexArray != 1){
          no_error = false;
          printf("(((ERROR: The structure Mesh has to have one VertexArray substructure)))\n");
        }
        else{ //Post processing after reading all the substructures!
          mat4t identity;
          identity.loadIdentity();
          mesh::sink<float> sink_(current_mesh, identity);
          sink_.reserve(num_vertexes,num_indices);
          for (int i = 0; i < num_vertexes; ++i)
            sink_.add_vertex(positions[i], normals[i], uvw[i]);
          for (int i = 0; i < num_indices; ++i)
            sink_.add_triangle(indices[i][0], indices[i][1], indices[i][2]);
        }

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Node structure
      /// @param  dict This is the resource where everything needs to be stored.
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_BoneNode(resource_dict &dict, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a GeometryNode structure
      /// @param  dict This is the resource where everything needs to be stored.
      /// @param  structure This is the structure to be analized, it has to be GeometryNode.
      /// @param  father This is the scene_node father to this structure (by default NULL).
      /// @return True if everything went well, false if there was some problem
      ///   Note: This function will check the properties of the structure
      ///   And it will check for the referencies. It will prepare to build the node
      ///   assigning a pointer to a mesh that it will be created later
      ///   GeometryObject will contain the node to the mesh!
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_GeometryNode(resource_dict &dict, openDDL_identifier_structure *structure, scene_node *father = NULL){
        int tempID;
        bool no_error = true;
        bool values_specified[3] = { false, false, false }; //values => visible, shadow, motion_blur
        bool values_properties[3] = { false, false, false };
        //Creating new node
        scene_node *node;
        node = new scene_node();
        //If it's not a Top-Level class, add it to his father
        if (father != NULL){
          father->add_child(node);
        }
        //Creating mesh, material and skeleton
        mesh * current_mesh = 0;
        material * current_material = 0;
        skeleton * current_skeleton = 0;
        //Creating matrix of transforms
        mat4t nodeToParent;
        nodeToParent.loadIdentity(); //and initialize it to identity!

        //Obtain the properties (may not have)
        int numProperties = structure->get_number_properties();
        for (int i = 0; i < numProperties; ++i){
          openDDL_properties *currentProperty;
          currentProperty = structure->get_property(i);
          tempID = identifiers_.get_value(currentProperty->identifierID);
          switch (tempID){
          case 57:
            //Property visible
            values_specified[0] = true;
            values_properties[0] = currentProperty->literal.value.bool_;
            break;
          case 52:
            //Property shadow
            values_specified[1] = true;
            values_properties[1] = currentProperty->literal.value.bool_;
            break;
          case 48:
            //Property motion_blur
            values_specified[2] = true;
            values_properties[2] = currentProperty->literal.value.bool_;
            break;
          default:
            printf("(((ERROR: Property %i non valid!)))\n", tempID);
            break;
          }
        }
        //Obtain the name of the structure
        char * name = structure->get_name();
        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        //Some variables to check the quantity of some substructures
        int numMorph = 0; //0 or 1
        int numName = 0; //0 or 1
        int numObjectRef = 0; //It has to be 1!!
        //This is to get some info from the substructures
        mat4t transformMatrix;
        float *values = NULL;
        int numValues;
        char * nameNode = NULL;
        int sizeName = 0;
        bool object_only = false;
        //Check all the substructures
        for (int i = 0; i < numSubstructures; ++i){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          switch (tempID){
          //Get Name (may not have)
          case 21://Name¡
            if (numName == 0){
              ++numName;
              no_error = openGEX_Name(nameNode, sizeName, substructure);
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
          //Get ObjectRef (geometryObject) (only 1)
          case 23://ObjectRef
            if (numObjectRef == 0){
              ++numObjectRef;
              no_error = openGEX_ObjectRef(current_mesh, substructure);
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
          //Get MaterialRef
          case 17://MaterialRef
            no_error = openGEX_MaterialRef(current_material, substructure);
            break;
          //Get Morph (may have one or none)
          case 20://Morph
            if (numMorph == 0){
              ++numMorph;
              no_error = openGEX_Morph(values, numValues, substructure);
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
          //Get Transforms (may not have)
          case 32://Transform
            no_error = openGEX_Transform(transformMatrix, object_only, substructure);
            nodeToParent.multMatrix(transformMatrix);
            break;
          case 33://Translation
            no_error = openGEX_Translate(transformMatrix, object_only, substructure);
            nodeToParent.multMatrix(transformMatrix);
            break;
          case 25://Rotation
            no_error = openGEX_Rotate(transformMatrix, object_only, substructure);
            nodeToParent.multMatrix(transformMatrix);
            break;
          case 26://Scale
            no_error = openGEX_Scale(transformMatrix, object_only, substructure);
            nodeToParent.multMatrix(transformMatrix);
            break;
          //Get Animation
          case 0://Animation
            //IGNORE ANIMATIONS FOR NOW!!!!
            break;
          //Get Nodes (children)
          case 4://BoneNode
            no_error = openGEX_BoneNode(dict, substructure, node);
            break;
          case 7://CameraNode
            //IGNORE CAMERAS FOR NOW!!!!
            break;
          case 10://GeometryNode
            no_error = openGEX_GeometryNode(dict, substructure, node);
            break;
          case 14://LightNode
            //IGNORE LIGHTS FOR NOW!!!!
            break;
          case 22://Nodes
            no_error = openGEX_Node(dict, substructure, node);
            break;
          }
        }
        //Sum up after reading all substructures
        if (numObjectRef != 1){
          printf("(((ERROR!! The GeometricNode structure has to have one ObjectRef!)))\n");
          no_error = false;
        } else{
          if (numName == 0){ //it has no name, so generate an autoname
            if (DEBUGOPENGEX) printf("As it has no name, let's generate an auto name \n");
            nameNode = new char[2];
            nameNode = "No";
          }
          //Creating new mesh_instance
          mesh_instance * object = new mesh_instance(node, current_mesh, current_material, current_skeleton);
          dict.set_resource(nameNode, object);
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a GeometryNode structure
      /// @param  structure This is the structure to be analized, it has to be GeometryNode.
      /// @return True if everything went well, false if there was some problem
      ///   Note: This function will check the properties of the structure
      ///   And it will check for the referencies. It will prepare to build the node
      ///   assigning a pointer to a mesh that it will be created later
      ///   GeometryObject will contain the node to the mesh!
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_GeometryObject(resource_dict &dict, openDDL_identifier_structure *structure){
        int tempID;
        bool no_error = true;
        bool values_specified[3] = { false, false, false }; //values => visible, shadow, motion_blur
        bool values_properties[3] = { false, false, false };
        //Creating mesh, material and skeleton
        mesh * current_mesh = 0;
        //Creating matrix of transforms
        mat4t nodeToParent;
        nodeToParent.loadIdentity(); //and initialize it to identity!

        //Obtain the properties (may not have)
        int numProperties = structure->get_number_properties();
        for (int i = 0; i < numProperties; ++i){
          openDDL_properties *currentProperty;
          currentProperty = structure->get_property(i);
          tempID = identifiers_.get_value(currentProperty->identifierID);
          switch (tempID){
          case 57:
            //Property visible
            values_specified[0] = true;
            values_properties[0] = currentProperty->literal.value.bool_;
            break;
          case 52:
            //Property shadow
            values_specified[1] = true;
            values_properties[1] = currentProperty->literal.value.bool_;
            break;
          case 48:
            //Property motion_blur
            values_specified[2] = true;
            values_properties[2] = currentProperty->literal.value.bool_;
            break;
          default:
            printf("(((ERROR: Property %i non valid!)))\n", tempID);
            break;
          }
        }
        //Obtain the name of the structure
        char * name = structure->get_name();
        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        dynarray<int> lod (numSubstructures);
        //Check all the substructures (all of them has to be of mesh type)
        for (int i = 0; i < numSubstructures && no_error; ++i){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          if (tempID != 18){//Mesh
            no_error = false;
            printf("(((ERROR: The structure GeometricObject can only have as substructure a Mesh)))\n");
          }
          else{
            lod[i] = 0;
            no_error = openGEX_Mesh(current_mesh, lod[i], substructure);
          }
        }

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will process the identifier structure and store it info into octet (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_identifier_structure(resource_dict &dict, openDDL_identifier_structure * structure){
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
            no_error = openGEX_GeometryNode(dict, structure);
            break;
          case 11: //GeometryObject
            if (DEBUGOPENGEX) printf("GeometryObject\n");
            //Process GeometryObject structure
            no_error = openGEX_GeometryObject(dict, structure);
            break;
          case 16: //Material
            if (DEBUGOPENGEX) printf("Material\n");
            //Process Material structure
            break;
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will check which type of structure to openGEX (previously analized by openDDL lexer)
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_structure(resource_dict &dict, openDDL_structure * structure){
        ++nesting;
        bool no_error = true;
        //Check the type of the structure!
        if (structure->get_type_structure() == 0){ //That means that it's a identifier structure!
          if (DEBUGSTRUCTURE) no_error = printf_identifier_structure((openDDL_identifier_structure*)structure); 
          no_error = openGEX_identifier_structure(dict, (openDDL_identifier_structure*)structure);
        }
        else{// That means that it's a data_type structure!!
          if (DEBUGSTRUCTURE) no_error = printf_data_type_structure((openDDL_data_type_structure*)structure);
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
      bool openGEX_data(resource_dict &dict){
        bool no_error = true;
        int numStructures = openDDL_file.size();
        openDDL_structure * topLevelStructure;
        if (DEBUGSTRUCTURE) printf("Starting to reading the file containin %i top-level structures:\n", numStructures);
        for (int i = 0; i < numStructures && no_error; ++i){
          //Get next structure
          nesting = 0;
          topLevelStructure = openDDL_file[i];
          //Check the type of the structure and the identificator or data_type!
          if (DEBUGSTRUCTURE) printf("\n-- Top-level structure %i:\n", i + 1);
          no_error = openGEX_structure(dict, topLevelStructure);
        }
        if (DEBUGSTRUCTURE) printf("\n");
        return no_error;
      }
    };

  }
}

#endif