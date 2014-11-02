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
      bool openGEX_Transform(mat4t &nodeToParent, bool &object_only, openDDL_identifier_structure *structure){
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

        }
        //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
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
      bool openGEX_Translate(mat4t &nodeToParent, int &coordinates, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;

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
      bool openGEX_Rotate(mat4t &nodeToParent, int &coordinates, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;

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
      bool openGEX_Scale(mat4t &nodeToParent, int &coordinates, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Node structure
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Node(resource_dict &dict, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Node structure
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_BoneNode(resource_dict &dict, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;

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
        float *values = NULL;
        int numValues;
        int coordinates = 3;
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
            no_error = openGEX_Transform(nodeToParent, object_only, substructure);
            break;
          case 33://Translation
            no_error = openGEX_Translate(nodeToParent, coordinates, object_only, substructure);
            break;
          case 25://Rotation
            no_error = openGEX_Rotate(nodeToParent, coordinates, object_only, substructure);
            break;
          case 26://Scale
            no_error = openGEX_Scale(nodeToParent, coordinates, object_only, substructure);
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