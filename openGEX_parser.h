////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_parser.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
/// NOTE: In the compiler several functions will have the boolean "no_error" this boolean is true when there is NO error, and false when error
///
///   ToDo: Some parts of the openGEX lexer process are "order" dependent. After checking the current exporters in openGEX, it seems that the generated code it has always a 
///           proper order, but, giving a look to the specification, the importer should be able to read the structures properly without mattering the order of the files. 
///           this could be helped improving the openDDL_lexer and changing some small parts of these code (check some ToDo comments in the code)
////////////////////////////////////////////////////////////////////////////////

#ifndef openGEX_parser_INCLUDED
#define openGEX_parser_INCLUDED

#include "openDDL_lexer.h"
#include "openGEX_identifiers.h"

namespace octet
{
  namespace loaders
  {
    namespace openGEX_loader
    {
      //Some enum to get the values of attrib and Param structures
      enum GEX_ATTRIB { GEX_NO_VALUE = -1, 
        GEX_DIFFUSE = 0, GEX_SPECULAR = 1, GEX_EMISSION = 2, GEX_OPACITY = 3, GEX_TRANSPARENCY = 4, //Color Texture Material
        GEX_NORMAL = 5, //Texture Material
        GEX_LIGHT = 6, //Color Light
        GEX_PROJECTION = 7 //Texture Light
      };
      enum GEX_PARAM {
        GEX_SPEC_POWER = 0, //Material
        GEX_INTENSITY = 1, //LightObject
        GEX_FOV = 2, GEX_NEAR = 3, GEX_FAR = 4, //CameraObject
        GEX_BEGIN = 5, GEX_END = 6, GEX_SCALE = 7, GEX_OFFSET = 8 //Atten
      };
      enum TYPE_TRANSFORM { _TRANSFORM = 0, _TRANSLATE = 1, _ROTATE = 2, _SCALE = 3, _MORPH = 4};
      enum SUBTYPE_TRANSFORM { GEX_X = 0, GEX_Y = 1, GEX_Z = 2, GEX_AXIS = 3, GEX_QUATERNION = 4, GEX_XYZ = 5};
      enum { DEBUGDATA = 0, DEBUGSTRUCTURE = 0, DEBUGOPENGEX = 0, MAX_BONE_COUNT = 8 };

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
  ////////////////////////////////////////////////////////////////////////////////
      class openGEX_parser : public openDDL_lexer{
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This struct contains a couple of references to transforms, and the type
      ////////////////////////////////////////////////////////////////////////////////
      struct ref_transform{
        atom_t ref;
        TYPE_TRANSFORM type;
        SUBTYPE_TRANSFORM subtype;
        mat4t matrix;
      };

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This struct contains a couple of int and float, with the index and weight of the bone
      ////////////////////////////////////////////////////////////////////////////////
      struct index_weight{
        int index;
        float weight;
      };

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This struct contains a couple of references to skin and skeleton
      ////////////////////////////////////////////////////////////////////////////////
      struct ref_skin_skeleton{
        skin *ref_skin;
        skeleton *ref_skeleton;
        dynarray<dynarray<index_weight>> boneIndexWeigthArray;
        int maxCount;
      };

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This struct contains a couple of index of the material and a reference to a mesh
      ////////////////////////////////////////////////////////////////////////////////
      struct index_mesh{
        unsigned int index;
        ref<mesh> ref_mesh;
      };

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This struct contains the info required by a mesh_instance to be created
      ////////////////////////////////////////////////////////////////////////////////
      struct info_mesh_instance : public resource{
        ref<mesh_instance> ref_instance;
        ref<scene_node> node;
        atom_t name;
        dynarray<char *> ref_materials;
        dynarray<index_mesh> index_and_meshes;
      };

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This struct contains the info required by a camera_instance to be created
      ////////////////////////////////////////////////////////////////////////////////
      struct info_camera_instance : public resource{
        ref<camera_instance> ref_instance;
        ref<scene_node> node;
        atom_t name;
        dynarray<int> index;
      };

      typedef gex_ident::gex_ident_enum gex_ident_list;
      //This will be used to handle the references to meshes and materials (and more will be probably added)
      dictionary<dynarray<ref<info_mesh_instance>>> info_meshes_from_objectRef;  //This contains all the info required for a mesh_instance, knowing the mesh
      //This is the list of materials!
      dictionary<ref<material>> ref_materials;      
      //from a materialRef (Material) gets which objectRef is using it
      dictionary<dynarray<ref<mesh_instance>>> ref_materials_inv;             
      //This will be used to handle the references to cameras
      dictionary<dynarray<ref<info_camera_instance>>> info_cameras_from_objectRef;  //This contains all the info required for a mesh_instance, knowing the mesh

      //This dictionary is indexed with the openDDL name, not the structure name! (CAUTION!!)
      dictionary<ref<scene_node>> dict_nodes;
      dictionary<ref<scene_node>> dict_bone_nodes;

      //This is (a pointer to) the dictionary where everything will need to be stored! The octet dict! Be careful!
      resource_dict *dict;

      //This boolean will be used to check if the bones & skin & skeleton has to be processed
      bool check_skin_skeleton;
      bool check_animation;

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
      /// @brief This function will translate from the char* to the proper GEX_ATTRIB
      /// @param  attrib  The attrib to be translated
      /// @param  size_attrib The size of the word
      /// return  The proper GEX_ATTRIB
      ////////////////////////////////////////////////////////////////////////////////
      GEX_ATTRIB obtain_attrib(char *attrib, int size_attrib){
        switch (size_attrib){
        case 5://light
          if (same_word("light", attrib, size_attrib)){
           return GEX_LIGHT;
          }
          else{
            return GEX_NO_VALUE;
          }
          break;
        case 6://normal
          if (same_word("normal", attrib, size_attrib)){
            return GEX_NORMAL;
          }
          else{
            return GEX_NO_VALUE;
          }
          break;
        case 7://diffuse or opacity
          if (same_word("diffuse", attrib, size_attrib)){
            return GEX_DIFFUSE;
          }
          else if (same_word("opacity", attrib, size_attrib)){
            return GEX_OPACITY;
          }
          else{
            return GEX_NO_VALUE;
          }
          break;
        case 8://specular or emission
          if (same_word("specular", attrib, size_attrib)){
            return GEX_SPECULAR;
          }
          else if (same_word("emission", attrib, size_attrib)){
            return GEX_EMISSION;
          }
          else{
            return GEX_NO_VALUE;
          }
          break;
        case 10://projection
          if (same_word("projection", attrib, size_attrib)){
            return GEX_PROJECTION;
          }
          else{
            return GEX_NO_VALUE;
          }
          break;
        case 12://transparency
          if (same_word("transparency", attrib, size_attrib)){
            return GEX_TRANSPARENCY;
          }
          else{
            return GEX_NO_VALUE;
          }
          break;
        default:
          return GEX_NO_VALUE;
          break;
        }
      }

      //Aux function
      char get_char_from_int(int number){
        return number + 48;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This is an aux function to obtain the transfor Matrix of a rotation with given values and a type
      /// @param  transformMatrix This is a matrix, it will return here the content of this transform
      /// @param  ref  This is an ref_transform with an atom (equivalent to a string) representing the reference of the transform (name in openDDL)
      /// @param  coordinates This is the coordinated being used
      /// @param  value_list This is the list of values
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool get_translate_matrix(mat4t &transformMatrix, ref_transform ref, float *value_list){
        //Obtain the matrix from this values
        transformMatrix.loadIdentity();
        if (ref.subtype == GEX_XYZ){
          for (int i = 0; i < 3; ++i){
            transformMatrix[i][3] = value_list[i];
          }
        }
        else{
          //If it's only one coordinate, change that one with the value in the structure
          int coordinates = ref.subtype == GEX_X? 0 : ref.subtype == GEX_Y? 1 : 2;
          transformMatrix[coordinates][3] = value_list[0];
        }
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This is an aux function to obtain the transfor Matrix of a rotation with given values and a type
      /// @param  transformMatrix This is a matrix, it will return here the content of this transform
      /// @param  ref  This is an ref_transform with an atom (equivalent to a string) representing the reference of the transform (name in openDDL)
      /// @param  value_list This is the list of values
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool get_rotate_matrix(mat4t &transformMatrix, ref_transform ref, float *value_list){
        transformMatrix.loadIdentity();
        if (ref.subtype == GEX_QUATERNION){ //Quaternion!
          vec4 values;
          //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
          for (int i = 0; i < 4; ++i){
            values[i] = value_list[i];
          }
          //Obtain the matrix from this values
          quat quaternion(values);
          transformMatrix = mat4t(quaternion);
          return true;
        }
        else if (ref.subtype == GEX_AXIS){
          //If it's axis, change that one with the value in the structure
          float x, y, z;
          float angle;
          //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
          angle = value_list[0];
          x = value_list[1];
          y = value_list[2];
          z = value_list[3];
          //Obtain the matrix from this values
          float cosAngle = cosf(angle * (3.14159265f / 180));
          float sinAngle = sinf(angle * (3.14159265f / 180));
          transformMatrix = mat4t(
            vec4(x*x*(1 - cosAngle) + cosAngle, x*y*(1 - cosAngle) + z*sinAngle, x*z*(1 - cosAngle) - y*sinAngle, 0.0f),
            vec4(x*y*(1 - cosAngle) - z*sinAngle, y*y*(1 - cosAngle) + cosAngle, y*z*(1 - cosAngle) + x*sinAngle, 0.0f),
            vec4(x*z*(1 - cosAngle) + y*sinAngle, y*z*(1 - cosAngle) - x*sinAngle, z*z*(1 - cosAngle) + cosAngle, 0.0f),
            vec4(0.0f, 0.0f, 0.0f, 1.0f)
            );
        }
        else {
          //If it's only one coordinate, change that one with the value in the structure
          float angle;
          angle = value_list[0];
          //Obtain the matrix from this values
          float cosAngle = cosf(angle);
          float sinAngle = sinf(angle);
          switch (ref.subtype){
          case GEX_X: //x
            transformMatrix = mat4t(
              vec4(1.0f, 0.0f, 0.0f, 0.0f),
              vec4(0.0f, cosAngle, -sinAngle, 0.0f),
              vec4(0.0f, sinAngle, cosAngle, 0.0f),
              vec4(0.0f, 0.0f, 0.0f, 1.0f)
              );
            break;
          case GEX_Y: //y
            transformMatrix = mat4t(
              vec4(cosAngle, 0.0f, sinAngle, 0.0f),
              vec4(0.0f, 1.0f, 0.0f, 0.0f),
              vec4(-sinAngle, 0.0f, cosAngle, 0.0f),
              vec4(0.0f, 0.0f, 0.0f, 1.0f)
              );
            break;
          case GEX_Z: //z
            transformMatrix = mat4t(
              vec4(cosAngle, -sinAngle, 0.0f, 0.0f),
              vec4(sinAngle, cosAngle, 0.0f, 0.0f),
              vec4(0.0f, 0.0f, 1.0f, 0.0f),
              vec4(0.0f, 0.0f, 0.0f, 1.0f)
              );
            break;
          }
        }
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This is an aux function to obtain the transfor Matrix of a rotation with given values and a type
      /// @param  transformMatrix This is a matrix, it will return here the content of this transform
      /// @param  ref  This is an ref_transform with an atom (equivalent to a string) representing the reference of the transform (name in openDDL)
      /// @param  coordinates This is the coordinated being used
      /// @param  value_list This is the list of values
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool get_scale_matrix(mat4t &transformMatrix, ref_transform ref, float *value){
        if (ref.subtype == GEX_XYZ){
          //Obtain the matrix from this values
          transformMatrix.loadIdentity();
          for (int i = 0; i < 3; ++i){
            transformMatrix[i][i] = value[i];
          }
        }
        else{
          //If it's only one coordinate, change that one with the value in the structure
          //Obtain the matrix from this values
          int coordinates = ref.subtype == GEX_X ? 0 : ref.subtype == GEX_Y ? 1 : 2;
          transformMatrix.loadIdentity();
          transformMatrix[coordinates][coordinates] = value[0];
        }
        return true;
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
            printf("%s", value.value.ref_);
          }
          else{
            printf("Local ->");
            printf("%s", value.value.ref_);
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
      /// @param  structure This is the structure to be printed, it has to be a data_type structure
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
      /// @param  structure This is the structure to be printed, it has to be a identifier structure
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
          openDDL_properties * current_property;
          //Let's work with all the properties!
          if (DEBUGSTRUCTURE) printfNesting();
          if (DEBUGSTRUCTURE) printf("The ammount of properties is: %i\n", numProperties);
          for (int i = 0; i < numProperties; ++i){
            current_property = structure->get_property(i);
            if (DEBUGSTRUCTURE) printfNesting();
            if (DEBUGSTRUCTURE) printf("Property <");
            tempID = current_property->identifierID;
            if (DEBUGSTRUCTURE) printf("%s", identifiers_.get_key(tempID));
            if (DEBUGSTRUCTURE) printf("> with value <");
            if (DEBUGSTRUCTURE) printfDDLliteral(current_property->literal);
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
            openGEX_structure(structure->get_substructure(i));
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
          openDDL_properties *current_property;
          current_property = structure->get_property(0);
          //Check that the property is correct!
          tempID = identifiers_.get_value(current_property->identifierID);
          if (tempID != 43){ // 43 = key
            printf("(((ERROR: The property in Metric has to be key and it's %i)))\n",tempID);
            return false;
          }
          else{
            //Check the value of the property key (distance, angle, time or up)
            char * value = current_property->literal.value.string_;
            int size = current_property->literal.size_string_;
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
      /// @brief This will obtain all the info from a Time structure
      /// @param  values This function will return in a array of array of floats, the values of the structure Time
      /// @param  curve This will get the type of the curve, this variable has to come with the value "linear" prestored (default value)
      /// @param  structure This is the structure to be analized, it has to be Time.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Time(dynarray<dynarray<float>> &values, atom_t &curve, openDDL_identifier_structure *structure){
        bool no_error = true;
        //Check properties
        int num_properties = structure->get_number_properties();
        if (num_properties > 1){
          no_error = false;
          printf("(((ERROR! The structure Time can have only one property as maximum!)))\n");
        }
        else if (num_properties == 1){
          openDDL_properties *current_property = structure->get_property(0);
          //check type of property
          if (identifiers_.get_value(current_property->identifierID) == 39){ //curve property
            curve = app_utils::get_atom(current_property->literal.value.string_);
          }
          else{
            no_error = false;
            printf("(((ERROR! The property is not a curve property, and it should!)))\n");
          }
        }
        //Check substructures
        if (no_error){
          //But first, check the type of curve
          unsigned int num_values;
          if (curve == app_utils::get_atom("linear")){ //if it's linear, it will have only one Key substructure!
            num_values = 1;
          }
          else{ //So if it's bezier it will have 3 Key substructures!
            num_values = 3;
          }
          //Resize by the ammount number of Key substructures
          values.resize(num_values);

          //For each substructure of type key...
          for (unsigned int i_key = 0; i_key < num_values; ++i_key){
            //get the key substructure
            openDDL_identifier_structure * key_substructure = (openDDL_identifier_structure *)structure->get_substructure(i_key);
            atom_t kind_value = app_utils::get_atom("value");
            //get the values of the substructure
            openDDL_data_type_structure *values_substructure = (openDDL_data_type_structure *)key_substructure->get_substructure(0);
            //Obtain each of the data_lists
            for (int i_data_list = 0; i_data_list < values_substructure->get_number_lists(); ++i_data_list){
              openDDL_data_list *data_list = values_substructure->get_data_list(i_data_list);
              //obtain all the data inside the data list
              for (unsigned int i = 0; i < data_list->data_list.size(); ++i){
                values[i_key].push_back(data_list->data_list[i]->value.float_);
              }
            }
          }
        }
        return no_error;
      }
        
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Value structure
      /// @param  values This function will return in a array of array of floats, the values of the structure Value
      /// @param  curve This will get the type of the curve, this variable has to come with the value "linear" prestored (default value)
      /// @param  structure This is the structure to be analized, it has to be Value.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Value(dynarray<dynarray<dynarray<float>>> &values, atom_t &curve, openDDL_identifier_structure *structure, ref_transform current_transform){
        bool no_error = true;
        //Check properties
        int num_properties = structure->get_number_properties();
        if (num_properties > 1){
          no_error = false;
          printf("(((ERROR! The structure Time can have only one property as maximum!)))\n");
        }
        else if (num_properties == 1){
          openDDL_properties *current_property = structure->get_property(0);
          //check type of property
          if (identifiers_.get_value(current_property->identifierID) == 39){ //curve property
            curve = app_utils::get_atom(current_property->literal.value.string_);
          }
          else{
            no_error = false;
            printf("(((ERROR! The property is not a curve property, and it should!)))\n");
          }
        }
        //Check substructures
        if (no_error){
          //But first, check the type of curve
          unsigned int num_values;
          if (curve == app_utils::get_atom("linear")){ //if it's linear, it will have only one Key substructure!
            num_values = 1;
          }else if(curve==app_utils::get_atom("bezier")){ //So if it's bezier it will have 3 Key substructures!
              num_values = 3;
          }
          else{ //And if it's tcb, it has 4 keys!
            num_values = 4;
          }
          //Resize by the ammount number of Key substructures
          values.resize(num_values);

          //For each substructure of type key...
          for (unsigned int i_key = 0; i_key < num_values; ++i_key){
            //get the key substructure
            openDDL_identifier_structure * key_substructure = (openDDL_identifier_structure *)structure->get_substructure(i_key);
            atom_t kind_value = app_utils::get_atom("value");
            openDDL_data_type_structure *values_substructure = (openDDL_data_type_structure *)key_substructure->get_substructure(0);
            int number_lists = values_substructure->get_number_lists();
            //if there is only one list, it's a data_list, so divide that data_list into different data_array_lists with one value each
            if (number_lists != 1){
              //Obtain each of the data_lists
              values[i_key].resize(number_lists);
              for (int i_data_list = 0; i_data_list < number_lists; ++i_data_list){
                openDDL_data_list *data_list = values_substructure->get_data_list(i_data_list);
                unsigned int data_list_size = data_list->data_list.size();
                //obtain all the data inside the data list
                values[i_key][i_data_list].resize(data_list_size);
                for (unsigned int i = 0; i < data_list_size; ++i){
                  float value_temp = data_list->data_list[i]->value.float_;
                  values[i_key][i_data_list][i] = value_temp;
                }
              }
            }
            else{//if the final list is just one, avoid 
              //Obtain each of the data_lists
              openDDL_data_list *data_list = values_substructure->get_data_list(0);
              unsigned int data_list_size = data_list->data_list.size();
              values[i_key].resize(data_list_size);
              for (unsigned int i = 0; i < data_list_size; ++i){
                values[i_key][i].resize(1);
                float value_temp = data_list->data_list[i]->value.float_;
                values[i_key][i][0] = value_temp;
              }
            }
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Track structure
      /// @param  list_ref This is the list of references to transforms of the father structure
      /// @param  structure This is the structure to be analized, it has to be Track.
      /// @param  father This is the scene_node of the item that posses the animation containing this track.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Track(dynarray<ref_transform> list_ref, openDDL_identifier_structure *structure, scene_node * father){
        bool no_error = true;
        dynarray<mat4t> new_transforms;
        //This are the curves from each of the substructures. By default, "linear"
        atom_t curve_time = app_utils::get_atom("linear");
        atom_t curve_value = curve_time;
        //Check properties (target)
        if (structure->get_number_properties() != 1){//If it has 0 or more than 1 properties, it's an error
          no_error = false;
          printf("(((ERROR!! -> The Track structures has to have one property!)))\n");
        }
        else{//It has only one property, so analize it
          openDDL_properties * current_property = structure->get_property(0);
          if (identifiers_.get_value(current_property->identifierID) != 53){ //If the property it's not target, it's an error
            no_error = false;
            printf("(((ERROR--> The Track structure can have only target property)))\n");
          }
          else{
            //as is a target property, get the reference of it
            current_property->literal.value.string_;
            atom_t reference = app_utils::get_atom(current_property->literal.value.ref_);
            ref_transform current_transform;
            current_transform.ref = atom_;
            //Check what is the transform referenced
            for (int i_ref = 0; i_ref < list_ref.size(); ++i_ref){
              if (list_ref[i_ref].ref == reference){
                current_transform = list_ref[i_ref];
              }
            }
            //Check substructures (Time & Value)
            if (structure->get_number_substructures() != 2){ //It has to have 2 substructures. Error otherwise
              no_error = false;
              printf("(((ERROR!! -> The Track structures has to have two substructures!)))\n");
            }
            else{
              //So it has two structures, check them (it has to be one of Time and one of Value)
              dynarray<dynarray<float>> values_time;
              dynarray<dynarray<dynarray<float>>> values_value;
              bool time_detected = false;
              bool value_detected = false;
              for (unsigned int j = 0; j < 2; ++j){
                openDDL_identifier_structure * substructure = (openDDL_identifier_structure *)structure->get_substructure(j);
                int tempID = substructure->get_identifierID();
                switch (tempID){
                case 30: //Time
                  if (time_detected){
                    no_error = false;
                    printf("(((ERROR! This structure Track can only have one Time substructure!)))\n");
                  }
                  else{
                    time_detected = true;
                    no_error = openGEX_Time(values_time, curve_time, substructure);
                  }
                  break;
                case 34: //Value
                  if (value_detected){
                    no_error = false;
                    printf("(((ERROR! This structure Track can only have one Value substructure!)))\n");
                  }
                  else{
                    value_detected = true;
                    no_error = openGEX_Value(values_value, curve_value, substructure, current_transform);
                  }
                  break;
                default:
                  no_error = false;
                  printf("(((ERROR! The substructures for a structure of type Track has to be Time or Value!)))\n");
                  break;
                }
              }
              //Post process current Track before getting the next Track
              //Obtain all the new transform matrices from the list of "list-ref" and changing only the current transform affected by this track
              printf("I have %i list\n", list_ref.size());
              int num_transforms = list_ref.size();
              int i_transform;
              bool stop = false;
              mat4t transformA, transformB, new_transform, final_transform;
              transformA.loadIdentity();
              transformB.loadIdentity();
              for (i_transform = 0; i_transform < num_transforms && !stop; ++i_transform){
                if (list_ref[i_transform].ref == atom_)
                  transformA.multMatrix(list_ref[i_transform].matrix);
                else
                  stop = true;
              }
              ++i_transform;
              for (; i_transform < num_transforms; ++i_transform){
                transformB.multMatrix(list_ref[i_transform].matrix);
              }
              //The current transform is current_transform
              animation *new_animation = new animation();
              dynarray<float> final_values;
              int size_animation = values_time[0].size();
              final_values.reserve(16 * size_animation);
              for (int i = 0; i < size_animation; ++i){
                final_transform.loadIdentity();
                final_transform.multMatrix(transformA);
                switch (current_transform.type){
                case _TRANSFORM:
                  new_transform.loadIdentity();
                  new_transform.init_transpose(values_value[0][i].data());
                  break;
                case _TRANSLATE:
                  get_translate_matrix(new_transform, current_transform, values_value[0][i].data());
                  break;
                case _ROTATE:
                  get_rotate_matrix(new_transform, current_transform, values_value[0][i].data());
                  break;
                case _SCALE:
                  get_scale_matrix(new_transform, current_transform, values_value[0][i].data());
                  break;
                }
                final_transform.multMatrix(new_transform);
                final_transform.multMatrix(transformB);
                for (int j = 0; j < 16; ++j){
                  final_values.push_back(final_transform.get()[j]);
                }
              }
              new_animation->add_channel(father, father->get_sid(), atom_transform, app_utils::get_atom("pos_x"),values_time[0],final_values);
              animation_instance *new_instance = new animation_instance(new_animation, father);
              dict->set_resource(app_utils::get_atom_name(current_transform.ref), new_instance);
            }
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Animation structure
      /// @param  list_ref This is the list of references to transforms of the father structure
      /// @param  structure This is the structure to be analized, it has to be Animation.
      /// @param  father This is the scene_node of the item that posses this animation.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Animation(dynarray<ref_transform> list_ref, openDDL_identifier_structure *structure, scene_node * father){
        bool no_error = false;
        //Initialize variables
        int clip = 0;
        float begin = -1;
        float end = -1;
        //Check properties (clip, begin, end)
        unsigned int num_properties = structure->get_number_properties();
        for (unsigned int i = 0; i < num_properties; ++i){
          openDDL_properties * current_property = structure->get_property(i);
          int tempID = identifiers_.get_value(current_property->identifierID);
          switch(tempID){
          case 38: //clip
            clip = current_property->literal.value.integer_;
            break;
          case 37: //begin
            begin = current_property->literal.value.float_;
            break;
          case 40: //end
            end = current_property->literal.value.float_;
            break;
          default:
            no_error = false;
            printf("(((ERROR-> The struct Animation has an invalid property!!)))\n");
            break;
          }
        }
        //Check substructures (series of tracks)
        unsigned int num_substructures = structure->get_number_substructures();
        for (unsigned int i_substructure = 0; i_substructure < num_substructures; ++i_substructure){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i_substructure);
          if (substructure->get_identifierID() == 31){//Track
            no_error = openGEX_Track(list_ref, substructure, father);
          }
          else{//This is not a Track structure
            no_error = false;
            printf("(((ERROR! The structures of Animation time only accept Track substructures!)))\n");
          }
        }//End of the study of substructures
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Name structure
      /// @param  name  This is a pointer to char, it will return here the value of the Name
      /// @param  nameSize  This is the size of the name that it's obtained by this structure
      /// @param  structure This is the structure to be analized, it has to be Name.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Name(char *&name, int &nameSize, openDDL_identifier_structure *structure){
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
            delete name;
            name = data_list_name->data_list[0]->value.string_;
            nameSize = data_list_name->data_list[0]->size_string_;
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Color structure
      /// @param  color  This will return a vector with the color info (R,G,B,A)
      /// @param  attrib_output  This will return the attribute of this color (it will depend if it belongs to a material, light...)
      /// @param  structure This is the structure to be analized, it has to be Color.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Color(vec4 &color, GEX_ATTRIB &attrib_output, openDDL_identifier_structure *structure){
        bool no_error = true;
        //Check properties (has to have one, attrib)
        if (structure->get_number_properties() == 1){
          openDDL_properties * current_property = structure->get_property(0);
          int tempID = identifiers_.get_value(current_property->identifierID);
          if (tempID == 36){ //attrib
            //attrib might have different values:
            // "diffuse", "specular", "emission", "opacity", "transparency", "light"
            int size_attrib = current_property->literal.size_string_;
            char * value_attrib = current_property->literal.value.string_;
            attrib_output = obtain_attrib(value_attrib, size_attrib);
            if (attrib_output == GEX_NO_VALUE){
              printf("(((ERROR! The attrib value has some mistake!)))\n");
              no_error = false;
            }
          }
          else{
            printf("(((ERROR! The property of a Color structure has to be attrib!)))\n");
            no_error = false;
          }
        }
        else{
          printf("(((ERROR! The structure Color has to have one (and only one) property!)))\n");
          no_error = false;
        }
        //Check substructures (has to have one, float[3] or float[4])
        if (structure->get_number_substructures() == 1){
          openDDL_data_type_structure *substructure = (openDDL_data_type_structure *) structure->get_substructure(0);
          int size_display = substructure->get_integer_literal();
          if (size_display != 3 && size_display != 4){
            no_error = false;
            printf("(((ERROR! The substructure of the Color structure is wrong!)))\n");
          }
          else{
            color = vec4(0);
            openDDL_data_list *data_list = substructure->get_data_list(0);
            for (int i = 0; i < size_display; ++i){
              color[i] = data_list->data_list[i]->value.float_;
            }
          }
        }
        else{
          printf("(((ERROR! The structure Color has to have one (and only one) substructure!)))\n");
          no_error = false;
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Param structure
      /// @param  value  This will return the value of the parameter contained in this structure
      /// @param  type  This will return the type of the parameter contained in this structure
      /// @param  structure This is the structure to be analized, it has to be Param.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Param(float &value, GEX_PARAM &type, openDDL_identifier_structure *structure){
        bool no_error = true;
        //Check the property (has to have only one, param)
        if (structure->get_number_properties() == 1){
          openDDL_properties * current_property = structure->get_property(0);
          if (identifiers_.get_value(current_property->identifierID == 36)){ //attrib
            //Some of the possible values of attrib are:
            //"specular_power" (for Material), 
            //"intensity" (for LightObject), 
            //"fov", "near", "far" (for CameraObject), 
            //"begin", "end", "scale", "offset" (for Atten)
            int size_attrib = current_property->literal.size_string_;
            char * value_attrib = current_property->literal.value.string_;
            switch (size_attrib){
            case 3://fov, far or end
              if (same_word("fov", value_attrib, size_attrib)){
                type = GEX_FOV;
              }
              else if (same_word("far", value_attrib, size_attrib)){
                type = GEX_FAR;
              }
              else if (same_word("end", value_attrib, size_attrib)){
                type = GEX_END;
              }
              else{
                printf("(((ERROR: The property attrib of the structure Param has a wrong value!)))\n");
                no_error = false;
              }
              break;
            case 4://near
              if (same_word("near", value_attrib, size_attrib)){
                type = GEX_NEAR;
              }
              else{
                printf("(((ERROR: The property attrib of the structure Param has a wrong value!)))\n");
                no_error = false;
              }
              break;
            case 5://begin or scale
              if (same_word("begin", value_attrib, size_attrib)){
                type = GEX_BEGIN;
              }
              else if (same_word("scale", value_attrib, size_attrib)){
                type = GEX_SCALE;
              }
              else{
                printf("(((ERROR: The property attrib of the structure Param has a wrong value!)))\n");
                no_error = false;
              }
              break;
            case 6://offset
              if (same_word("offset", value_attrib, size_attrib)){
                type = GEX_OFFSET;
              }
              else{
                printf("(((ERROR: The property attrib of the structure Param has a wrong value!)))\n");
                no_error = false;
              }
              break;
            case 9://intensity
              if (same_word("intensity", value_attrib, size_attrib)){
                type = GEX_INTENSITY;
              }
              else{
                printf("(((ERROR: The property attrib of the structure Param has a wrong value!)))\n");
                no_error = false;
              }
              break;
            case 14://specular_power
              if (same_word("specular_power", value_attrib, size_attrib)){
                type = GEX_SPEC_POWER;
              }
              else{
                printf("(((ERROR: The property attrib of the structure Param has a wrong value!)))\n");
                no_error = false;
              }
              break;
            default:
              break;
            }
          }
          else{
            printf("(((ERROR! The structure Param has to a property of type attrib)))\n");
            no_error = false;
          }
        }
        else{
          printf("(((ERROR! The structure Param has to have one (and only one) property!)))\n");
          no_error = false;
        }
        //CHeck the substructures (has to hav eonly one, with one float!)
        if (structure->get_number_substructures() == 1){
          openDDL_data_type_structure *substructure = (openDDL_data_type_structure *)structure->get_substructure(0);
          int size_display = substructure->get_integer_literal();
          if (size_display == 1){
            value = substructure->get_data_list(0)->data_list[0]->value.float_;
          }
          else{
            no_error = false;
            printf("(((ERROR! The substructure of the Param structure is wrong!)))\n");
          }
        }
        else{
          printf("(((ERROR! The structure Param has to have one (and only one) substructure!)))\n");
          no_error = false;
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Texture structure
      /// @param  texture_url  This is a pointer to char, it will return here the url of the Texture
      /// @param  size_url  This is a pointer to char, it will return here the size of the url of the Texture
      /// @param  index  This will return the index of the structure (this has to come with a value of 0, default value)
      /// @param  type  This will return the type of the parameter contained in this structure
      /// @param  structure This is the structure to be analized, it has to be Texture.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Texture(char *&texture_url, int &size_url, uint16_t &index, GEX_ATTRIB &type, openDDL_identifier_structure *structure){
        bool no_error = true;
        //Check properties (may have two, attrib (OBLIGATORY), texcoord (optional)
        int num_properties = structure->get_number_properties();
        type = GEX_NO_VALUE;
        for (int i = 0; i < num_properties; ++i){
          openDDL_properties * current_property = structure->get_property(i);
          int tempID = identifiers_.get_value(current_property->identifierID);
          int size_attrib;
          char * value_attrib;
          switch (tempID){
          case 36: //attrib
            size_attrib = current_property->literal.size_string_;
            value_attrib = current_property->literal.value.string_;
            type = obtain_attrib(value_attrib, size_attrib);
            if (type == GEX_NO_VALUE){
              printf("(((ERROR! The attrib value has some mistake!)))\n");
              no_error = false;
            }
            break;
          case 54: //textcoord
            index = current_property->literal.value.u_integer_literal_;
            break;
          default:
            printf("(((ERROR! The structure Texture has some wrong property!)))\n");
            no_error = false;
            break;
          }
        }
        //Check substructures (has to have a string, and may have some transforms, and some animations)
        int number_substructures = structure->get_number_substructures();
        int counter_substructure;
        bool string_founded = false;
        bool object_only = false;
        dynarray<mat4t> transformMatrixes;
        dynarray<ref_transform> list_ref;
        if (transformMatrixes.size() < 1)
          transformMatrixes.resize(1);
        mat4t nodeToParent;
        nodeToParent.loadIdentity();
        for (counter_substructure = 0; counter_substructure < number_substructures && !string_founded; ++counter_substructure){
          ref_transform current_ref;
          current_ref.ref = atom_;
          openDDL_structure *substructure = structure->get_substructure(counter_substructure);
          if (substructure->get_type_structure() == DATA_TYPE_TYPE){
            openDDL_data_list *data_list = ((openDDL_data_type_structure *)substructure)->get_data_list(0);
            size_url = data_list->data_list[0]->size_string_;
            texture_url = data_list->data_list[0]->value.string_;
          }
          else if (substructure->get_type_structure() == IDENTIFIER_TYPE){
            int tempID = ((openDDL_identifier_structure *)substructure)->get_identifierID();
            switch (tempID){
              //Get Transforms (may not have)
            case 32://Transform
              no_error = openGEX_Transform(current_ref, transformMatrixes, object_only, (openDDL_identifier_structure *)substructure);
              nodeToParent.multMatrix(transformMatrixes[0]);
              if (current_ref.ref != atom_){
                list_ref.push_back(current_ref);
              }
              break;
            case 33://Translation
              no_error = openGEX_Translate(current_ref, transformMatrixes[0], object_only, (openDDL_identifier_structure *)substructure);
              nodeToParent.multMatrix(transformMatrixes[0]);
              if (current_ref.ref != atom_){
                list_ref.push_back(current_ref);
              }
              break;
            case 25://Rotation
              no_error = openGEX_Rotate(current_ref, transformMatrixes[0], object_only, (openDDL_identifier_structure *)substructure);
              nodeToParent.multMatrix(transformMatrixes[0]);
              if (current_ref.ref != atom_){
                list_ref.push_back(current_ref);
              }
              break;
            case 26://Scale
              no_error = openGEX_Scale(current_ref, transformMatrixes[0], object_only, (openDDL_identifier_structure *)substructure);
              nodeToParent.multMatrix(transformMatrixes[0]);
              if (current_ref.ref != atom_){
                list_ref.push_back(current_ref);
              }
              break;
              //Get Animation
            case 0://Animation
            //IGNORE ANIMATIONS FOR NOW!!!! TO DO!
              break;
            default:
              printf("(((ERROR!! The structure Texture has an invalid substructure!)))\n");
              no_error = false;
              break;
            }
          }
          else{
            printf("(((ERROR!! If this is happening, it's supposed to be an error of the importer. Tell Juanmi Huertas please: juanmihd@gmail.com)))\n");
            no_error = false;
          }
        }
        for (; counter_substructure < number_substructures; ++counter_substructure){
          ref_transform current_ref;
          current_ref.ref = atom_;
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(counter_substructure);
          int tempID = (substructure)->get_identifierID();
          switch (tempID){
            //Get Transforms (may not have)
          case 32://Transform
            no_error = openGEX_Transform(current_ref, transformMatrixes, object_only, (openDDL_identifier_structure *)substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            if (current_ref.ref != atom_){
              list_ref.push_back(current_ref);
            }
            break;
          case 33://Translation
            no_error = openGEX_Translate(current_ref, transformMatrixes[0], object_only, (openDDL_identifier_structure *)substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            if (current_ref.ref != atom_){
              list_ref.push_back(current_ref);
            }
            break;
          case 25://Rotation
            no_error = openGEX_Rotate(current_ref, transformMatrixes[0], object_only, (openDDL_identifier_structure *)substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            if (current_ref.ref != atom_){
              list_ref.push_back(current_ref);
            }
            break;
          case 26://Scale
            no_error = openGEX_Scale(current_ref, transformMatrixes[0], object_only, (openDDL_identifier_structure *)substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            if (current_ref.ref != atom_){
              list_ref.push_back(current_ref);
            }
            break;
          case 0://Animation
            //IGNORE ANIMATIONS FOR NOW!!!! TO DO!
            break;
          default:
            printf("(((ERROR!! The structure Texture has an invalid substructure!)))\n");
            no_error = false;
            break;
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a ObjectRef structure
      /// @param  object_ref  This is a pointer to char, it will return here the value of the ObjectRef
      /// @param  structure This is the structure to be analized, it has to be ObjectRef.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_ObjectRef(char *&object_ref, openDDL_identifier_structure *structure){
        bool no_error = true;
        //This structure cannot have properties
        if (structure->get_number_properties() > 0){
          no_error = true;
          printf("(((ERROR: A ObjectRef structure cannot have properties!)))\n");
        }
        //And it has to have one single substructure of data type ref
        if (structure->get_number_substructures() == 1){
          openDDL_data_type_structure * substructure = (openDDL_data_type_structure *)structure->get_substructure(0);
          openDDL_data_list * data_list_ref = substructure->get_data_list(0);
          object_ref = data_list_ref->data_list[0]->value.ref_;
        }
        else{
          no_error = true;
          printf("(((ERROR: A ObjectRef structure has to have one single substructure!)))\n");
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a MaterialRef structure
      /// @param  material_ref  This is a pointer to char, it will return here the value of the MaterialRef
      /// @param  index  This will return the index of the material (this has to come with a value of 0, default value)
      /// @param  structure This is the structure to be analized, it has to be MaterialRef.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_MaterialRef(char *&material_ref, uint32_t &index, openDDL_identifier_structure *structure){
        bool no_error = true;
        //This structure cannot have properties
        if (structure->get_number_properties() > 1){
          no_error = true;
          printf("(((ERROR: A MaterialRef structure cannot have more than 1 properties!)))\n");
        }
        else if (structure->get_number_properties() == 1){
          openDDL_properties * current_property = structure->get_property(0);
          int tempID = identifiers_.get_value(current_property->identifierID);
          if (tempID == 42){ //index
            index = current_property->literal.value.float_;
          }
        }
        //And it has to have one single substructure of data type ref
        if (structure->get_number_substructures() == 1){
          openDDL_data_type_structure * substructure = (openDDL_data_type_structure *)structure->get_substructure(0);
          material_ref = substructure->get_data_list(0)->data_list[0]->value.ref_;
        }
        else{
          no_error = true;
          printf("(((ERROR: A MaterialRef structure has to have one single substructure!)))\n");
        }
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
      /// @param  ref  This is an ref_transform with an atom (equivalent to a string) representing the reference of the transform (name in openDDL)
      /// @param  transformMatrix This is an array of matrixes, it will return here the content of this transform
      /// @param  object_only  This is a boolean saying if this is going to be applied only to one object
      /// @param  structure This is the structure to be analized, it has to be Transform.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Transform(ref_transform &ref, dynarray<mat4t> &transformMatrix, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;
        //Check that the structure is correct!
        char * name = structure->get_name();
        if (name != NULL)
          ref.ref = app_utils::get_atom(name);
        else
          ref.ref = atom_;
        ref.type = _TRANSFORM;
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
          printf("(((ERROR!! The data substructure of the structure Transform has to be only one!!)))\n");
          no_error = false;
        } else{
        //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
          openDDL_data_type_structure * substructure = (openDDL_data_type_structure *)structure->get_substructure(0);
          unsigned int size_data_list = substructure->get_number_lists();
          if (transformMatrix.size() < size_data_list)
            transformMatrix.resize(size_data_list);
          for (unsigned int i = 0; i < size_data_list; ++i){
            float values[16];
            openDDL_data_list * data_list_values = substructure->get_data_list(i);
            for (int j = 0; j < 16; ++j){
              values[j] = data_list_values->data_list[j]->value.float_;
            }
            //Obtain the matrix from this values
            transformMatrix[i].init_transpose(values);
          }
        }
        ref.matrix = transformMatrix[0];
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Translation structure
      /// @param  ref  This is an ref_transform with an atom (equivalent to a string) representing the reference of the transform (name in openDDL)
      /// @param  transformMatrix This is a matrix, it will return here the content of this transform
      /// @param  object_only  This is a boolean saying if this is going to be applied only to one object
      /// @param  structure This is the structure to be analized, it has to be Translation.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Translate(ref_transform &ref, mat4t &transformMatrix, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;
        int coordinates = 3;
        //Check that the structure is correct!
        char * name = structure->get_name();
        if (name != NULL)
          ref.ref = app_utils::get_atom(name);
        else
          ref.ref = atom_;
        ref.type = _TRANSLATE;
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
                  ref.subtype = GEX_X;
                  break;
                case 'y':
                  coordinates = 1;
                  ref.subtype = GEX_Y;
                  break;
                case 'z':
                  coordinates = 2;
                  ref.subtype = GEX_Z;
                  break;
                }
              else{
                ref.subtype = GEX_XYZ;
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
          printf("(((ERROR!! The data substructure of the structure Transform has to be only one!!)))\n");
          no_error = false;
        }
        else{
          dynarray<float> value_list;
          value_list.resize(3);
          //Obtain the values from the substructures (float[16]) that will be converted into a mat4t!!!
          openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
          for (int i = 0; i < data_list_values->data_list.size(); ++i){
            value_list[i] = data_list_values->data_list[i]->value.float_;
          }
          no_error = get_translate_matrix(transformMatrix, ref, value_list.data());
        }
        ref.matrix = transformMatrix;
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Translation structure
      /// @param  ref  This is an ref_transform with an atom (equivalent to a string) representing the reference of the transform (name in openDDL)
      /// @param  transformMatrix This is a matrix, it will return here the content of this transform
      /// @param  object_only  This is a boolean saying if this is going to be applied only to one object
      /// @param  structure This is the structure to be analized, it has to be Translation.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Rotate(ref_transform &ref, mat4t &transformMatrix, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;
        int coordinates = 3;
        //Check that the structure is correct!
        char * name = structure->get_name();
        if (name != NULL)
          ref.ref = app_utils::get_atom(name);
        else
          ref.ref = atom_;
        ref.type = _ROTATE;
        ref.subtype = GEX_AXIS;
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
                  ref.subtype = GEX_X;
                  break;
                case 'y':
                  ref.subtype = GEX_Y;
                  break;
                case 'z':
                  ref.subtype = GEX_Z;
                  break;
                }
              }
              else if (current_property->literal.size_string_ == 4)
                ref.subtype = GEX_AXIS;
              else
                ref.subtype = GEX_QUATERNION;
            }
            else{
              printf("(((ERROR: This cannot be a property of this structure!)))\n");
              no_error = false;
            }
          }
        }
        //Check that the substructures are correct!
        if (structure->get_number_substructures() != 1){
          printf("(((ERROR!! The data substructure of the structure Transform has to be only one!!)))\n");
          no_error = false;
        }
        else{
          openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
          dynarray<float> value_list;
          int size_list = data_list_values->data_list.size();
          value_list.reserve(size_list);
          for (int i = 0; i < size_list; ++i){
            value_list[i] = data_list_values->data_list[i]->value.float_;
          }
          no_error = get_rotate_matrix(transformMatrix, ref, value_list.data());
        }
        ref.matrix = transformMatrix;
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Translation structure
      /// @param  ref  This is an ref_transform with an atom (equivalent to a string) representing the reference of the transform (name in openDDL)
      /// @param  transformMatrix This is a matrix, it will return here the content of this transform
      /// @param  object_only  This is a boolean saying if this is going to be applied only to one object
      /// @param  structure This is the structure to be analized, it has to be Translation.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Scale(ref_transform &ref, mat4t &transformMatrix, bool &object_only, openDDL_identifier_structure *structure){
        bool no_error = true;
        int coordinates = 3;
        //Check that the structure is correct!
        char * name = structure->get_name();
        if (name != NULL)
          ref.ref = app_utils::get_atom(name);
        else
          ref.ref = atom_;
        ref.type = _SCALE;
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
                  ref.subtype = GEX_X;
                  break;
                case 'y':
                  coordinates = 1;
                  ref.subtype = GEX_Y;
                  break;
                case 'z':
                  coordinates = 2;
                  ref.subtype = GEX_Z;
                  break;
              }
              else{
                ref.subtype = GEX_XYZ;
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
          printf("(((ERROR!! The data substructure of the structure Transform has to be only one!!)))\n");
          no_error = false;
        }
        else{
          //Obtain the values from the substructures (float�) that will be converted into a mat4t!!!
          openDDL_data_list * data_list_values = ((openDDL_data_type_structure *)structure->get_substructure(0))->get_data_list(0);
          dynarray<float> value_list;
          int size_list = data_list_values->data_list.size();
          value_list.reserve(size_list);
          for (int i = 0; i < size_list; ++i){
            value_list[i] = data_list_values->data_list[i]->value.float_;
          }
          no_error = get_scale_matrix(transformMatrix, ref, value_list.data());
        }
        ref.matrix = transformMatrix;
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Node structure
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @param  father This is the father scene_node. By default  NULL
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Node(openDDL_identifier_structure *structure, scene_node *father = NULL){
        int tempID;
        bool no_error = true;
        //Creating new node
        scene_node *node;
        node = new scene_node();
        //If it's not a Top-Level class, add it to his father
        if (father != NULL){
          father->add_child(node);
        }
        //Obtain the name of the structure
        char * name = structure->get_name();
        node->set_sid(app_utils::get_atom(name));
        //BoneNode has no properties!
        int numProperties = structure->get_number_properties();
        if (numProperties != 0){
          no_error = true;
          printf("(((ERROR! BoneNode cannot have properties!!");
        }
        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        //Some variables to check the quantity of some substructures
        int numNames = 0; //0 or 1
        //Creating matrix of transforms
        mat4t nodeToParent;
        nodeToParent.loadIdentity(); //and initialize it to identity!
        //This is to get some info from the substructures
        dynarray<ref_transform> list_ref;
        dynarray<mat4t> transformMatrixes;
        if (transformMatrixes.size() < 1)
          transformMatrixes.resize(1);
        dynarray<uint32_t> mat_index;
        mat_index.resize(10);
        int num_mat_index = 0;
        char * nameNode = NULL;
        int sizeName = 0;
        bool object_only = false;
        //Check all the substructures
        for (int i = 0; i < numSubstructures; ++i){
          ref_transform current_ref;
          current_ref.ref = atom_;
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          switch (tempID){
            //Get Name (may not have)
          case 21://Name
            if (numNames == 0){
              ++numNames;
              no_error = openGEX_Name(nameNode, sizeName, substructure);
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
            //Get Transforms (may not have)
          case 32://Transform
            no_error = openGEX_Transform(current_ref, transformMatrixes, object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 33://Translation
            no_error = openGEX_Translate(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 25://Rotation
            no_error = openGEX_Rotate(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 26://Scale
            no_error = openGEX_Scale(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
            //Get Animation
          case 0://Animation
            //IGNORE ANIMATIONS FOR NOW!!!! TO DO!
            break;
            //Get Nodes (children)
          case 4://BoneNode
            no_error = openGEX_BoneNode(substructure, node);
            break;
          case 7://CameraNode
            //IGNORE CAMERAS FOR NOW!!!! TO DO!
            break;
          case 10://GeometryNode
            no_error = openGEX_GeometryNode(substructure, node);
            break;
          case 14://LightNode
            //IGNORE LIGHTS FOR NOW!!!! TO DO!
            break;
          case 22://Nodes
            no_error = openGEX_Node(substructure, node);
            break;
          }
        }
        //Sum up after reading all substructures
        if (numNames == 0){ //it has no name, so get the structure name
          if (DEBUGOPENGEX) printf("As it has no name, assign the structure name \n");
          nameNode = name;
        }
        // We are working with transpose matrix in octet!!! So transpose it to be able to work properly!
        node->access_nodeToParent().multMatrix(nodeToParent.transpose4x4());

        //Add the id for the animations
        node->set_sid(app_utils::get_atom(nameNode));
        //Add the current bone to the dictionary of bones
        dict_nodes[name] = node;

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a BoneNode structure
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @param  father This is the father scene_node. By default  NULL
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_BoneNode(openDDL_identifier_structure *structure, scene_node *father = NULL){
        int tempID;
        bool no_error = true;
        //Creating new node
        scene_node *node;
        node = new scene_node();
        //If it's not a Top-Level class, add it to his father
        if (father != NULL){
          father->add_child(node);
        }
        //Obtain the name of the structure
        char * name = structure->get_name();
        node->set_sid(app_utils::get_atom(name));
        //BoneNode has no properties!
        int numProperties = structure->get_number_properties();
        if (numProperties != 0){
          no_error = true;
          printf("(((ERROR! BoneNode cannot have properties!!");
        }
        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        //Some variables to check the quantity of some substructures
        int numNames = 0; //0 or 1
        //Creating matrix of transforms
        mat4t nodeToParent;
        nodeToParent.loadIdentity(); //and initialize it to identity!
        //This is to get some info from the substructures
        dynarray<mat4t> transformMatrixes;
        if (transformMatrixes.size() < 1)
          transformMatrixes.resize(1);
        dynarray<uint32_t> mat_index;
        dynarray<ref_transform> list_ref;
        mat_index.resize(10);
        int num_mat_index = 0;
        char * nameNode = NULL;
        int sizeName = 0;
        bool object_only = false;
        //Check all the substructures
        for (int i = 0; i < numSubstructures; ++i){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          ref_transform structure_ref;
          structure_ref.ref = atom_;
          tempID = substructure->get_identifierID();
          switch (tempID){
            //Get Name (may not have)
          case 21://Name
            if (numNames == 0){
              ++numNames;
              no_error = openGEX_Name(nameNode, sizeName, substructure);
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
            //Get Transforms (may not have)
          case 32://Transform
            no_error = openGEX_Transform(structure_ref, transformMatrixes, object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(structure_ref);
            break;
          case 33://Translation
            no_error = openGEX_Translate(structure_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(structure_ref);
            break;
          case 25://Rotation
            no_error = openGEX_Rotate(structure_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(structure_ref);
            break;
          case 26://Scale
            no_error = openGEX_Scale(structure_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(structure_ref);
            break;
            //Get Animation
          case 0://Animation
            if (check_animation)
              no_error = openGEX_Animation(list_ref, substructure, node);
            break;
            //Get Nodes (children)
          case 4://BoneNode
            no_error = openGEX_BoneNode(substructure, node);
            break;
          case 7://CameraNode
            //IGNORE CAMERAS FOR NOW!!!! TO DO!
            break;
          case 10://GeometryNode
            no_error = openGEX_GeometryNode(substructure, node);
            break;
          case 14://LightNode
            //IGNORE LIGHTS FOR NOW!!!! TO DO!
            break;
          case 22://Nodes
            no_error = openGEX_Node(substructure, node);
            break;
          }
        }
        //Sum up after reading all substructures
        if (numNames == 0){ //it has no name, so get the structure name
          if (DEBUGOPENGEX) printf("As it has no name, assign the structure name \n");
          nameNode = name;
        }
        // We are working with transpose matrix in octet!!! So transpose it to be able to work properly!
        node->access_nodeToParent().multMatrix(nodeToParent.transpose4x4());
        //At this point the node has the children, the parent, and the matrix of transform!
        //Add the current bone to the dictionary of bones
        dict_nodes[name] = node;
        dict_bone_nodes[name] = node;
        
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a VertexArray structure
      /// @param  vertices This will be an array (with a pointer) of vertices (return!)
      /// @param  num_vertexes This will be the size of the array of vertices (return!)
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @param  father This is the father scene_node. By default  NULL
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_VertexArray(mesh::vertex *&vertices, int &num_vertexes, int &current_attrib, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;
        //Get the value of the properties!
        char * attrib_value = NULL;
        int attrib_size = 0;
        int morph_index = 0;
        current_attrib = 0;
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
              if (vertices == NULL)
                vertices = new mesh::vertex[num_vertexes];
              for (int i = 0; i < num_vertexes; ++i){
                a = new_data_list->value.float_;
                ++new_data_list;
                b = new_data_list->value.float_;
                ++new_data_list;
                c = new_data_list->value.float_;
                ++new_data_list;
                vertices[i].pos = vec3(a, b, c);
              }
            }
            else if (current_attrib == 1){//normal
              if (vertices == NULL)
                vertices = new mesh::vertex[num_vertexes];
              for (int i = 0; i < num_vertexes; ++i){
                a = new_data_list->value.float_;
                ++new_data_list;
                b = new_data_list->value.float_;
                ++new_data_list;
                c = new_data_list->value.float_;
                ++new_data_list;
                vertices[i].normal = vec3(a, b, c);
              }
            }
            else if (current_attrib == 2){//uv
              if (vertices == NULL)
                vertices = new mesh::vertex[num_vertexes];
              for (int i = 0; i < num_vertexes; ++i){
                a = new_data_list->value.float_;
                ++new_data_list;
                b = new_data_list->value.float_;
                ++new_data_list;
                vertices[i].uv = vec2(a, b);
              }
            }
          }
          else{
            num_vertexes = number_data_lists;
            if (size_data_list == 2 && current_attrib == 2){ //uv
              if (vertices == NULL)
                vertices = new mesh::vertex[num_vertexes];
              for (int i = 0; i < number_data_lists; ++i){
                data_list = substructure->get_data_list(i);
                vertices[i].uv = vec2(data_list->data_list[0]->value.float_,
                                      data_list->data_list[1]->value.float_);
              }
            }
            else if (size_data_list == 3){
              if (current_attrib == 0){ //pos
                if (vertices == NULL)
                  vertices = new mesh::vertex[num_vertexes];
                for (int i = 0; i < number_data_lists; ++i){
                  data_list = substructure->get_data_list(i);
                  vertices[i].pos = vec3(data_list->data_list[0]->value.float_,
                                        data_list->data_list[1]->value.float_, 
                                        data_list->data_list[2]->value.float_);
                }
              }
              else if (current_attrib == 1){ //normal
                if (vertices == NULL)
                  vertices = new mesh::vertex[num_vertexes];
                for (int i = 0; i < number_data_lists; ++i){
                  data_list = substructure->get_data_list(i);
                  vertices[i].normal = vec3(data_list->data_list[0]->value.float_,
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
      /// @param  indices This will be an array (with a pointer) of indices (return!)
      /// @param  num_indices This will be the size of the array of indices (return!)
      /// @param  material_index This will contain the index of the material that will be used (return!)
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @param  father This is the father scene_node. By default  NULL
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_IndexArray(uint32_t *&indices, int &num_indices, unsigned int &material_index, openDDL_identifier_structure *structure, scene_node *father = NULL){
        bool no_error = true;
        //Get the value of the properties!
        bool clock_wise = false;
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
              material_index = current_property->literal.value.float_;
            }
            else if (typeProperty == 51){//restart
              restart = current_property->literal.value.float_;
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
        if (structure->get_number_substructures() != 1){
          no_error = false;
          printf("(((ERROR! The VertexArray has to have a substructure, only one, but at least one!)))\n");
        }
        else{
          openDDL_data_type_structure *substructure = (openDDL_data_type_structure *)structure->get_substructure(0);
          openDDL_data_list *data_list;
          int size_data_list = substructure->get_integer_literal();
          int number_data_lists = substructure->get_number_lists();
          if (size_data_list == 1){
            data_list = substructure->get_data_list(0);
            num_indices = data_list->data_list.size();
            if (indices == NULL)
              indices = new uint32_t [num_indices];
            for (int i = 0; i < num_indices; ++i){
              indices[i] = data_list->data_list[i]->value.u_integer_literal_;
            }
          }
          else{
            num_indices = number_data_lists * size_data_list;
            if (indices == NULL)
              indices = new uint32_t[num_indices];
            for (int i = 0; i < number_data_lists; ++i){
              data_list = substructure->get_data_list(i);
              for (int j = 0; j < size_data_list; ++j){
                indices[i*size_data_list + j] = data_list->data_list[j]->value.u_integer_literal_;
              }
              if(DEBUGOPENGEX) printf("(%u, %u, %u)\n", indices[i*size_data_list], indices[i*size_data_list + 1], indices[i*size_data_list + 2]);
            }
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a BoneRefArray structure
      /// @param  ref_array This is an array of atoms_
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_BoneRefArray(dynarray<atom_t> &ref_array, openDDL_identifier_structure *structure){
        bool no_error = true;
        //Check properties (it cannot have properties!)
        if (structure->get_number_properties() != 0){
          no_error = false;
          printf("(((ERROR -> The structure BoneRefArray cannot have properties!)))\n");
        }
        //Check substructures (it can have only one substructure (array of bones)
        if (structure->get_number_substructures() == 1){
          //Obtain the data_list_array
          openDDL_data_type_structure *substructure = (openDDL_data_type_structure *)structure->get_substructure(0);
          openDDL_data_list *data_list = substructure->get_data_list(0);
          int num_ref = data_list->data_list.size();
          ref_array.resize(num_ref);
          for (int i = 0; i < num_ref; ++i){
            ref_array[i] = app_utils::get_atom(data_list->data_list[i]->value.ref_);
          }
        }
        else{
          no_error = false;
          printf("(((ERROR -> The structure BoneRefArray has a wrong number of substructures)))\n");
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Skeleton structure
      /// @param  bone_array This is an array of atoms_ of the bones
      /// @param  matrixTransforms This is an array of matrixes for each bone
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @param  instance  This is an atom (equivalente to string! but cheaper!) with the name of the mesh_instance that contains the mesh
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Skeleton(dynarray<atom_t> &bone_array , dynarray<mat4t> &matrixTransforms, openDDL_identifier_structure *structure, atom_t instance){
        bool no_error = true;
        //Check properties (Skeleton structure has no properties)
        if (structure->get_number_properties() != 0){
          no_error = false;
          printf("(((ERRROR -> The structure Skeleton cannot have properties!)))\n");
        }
        //Check substructures (Skeleton structure has to have one BoneRefArray and one Transform
        bool contains_bone_ref = false;
        bool contains_transform = false;
        bool object_only = false;
        int number_substructures = structure->get_number_substructures();
        for (int i = 0; i < number_substructures; ++i){
          ref_transform current_ref;
          current_ref.ref = atom_;
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          int tempID = substructure->get_identifierID();
          switch (tempID){
          case 5:  //BoneRefArray
            if (!contains_bone_ref){
              contains_bone_ref = true;
              no_error = openGEX_BoneRefArray(bone_array, substructure);
            }
            else{
              no_error = false;
              printf("(((ERROR-> The structure Skeleton can only contain one single structure of BoneRefArray!)))\n");
            }
            break;
          case 32: //Transform
            if (!contains_transform){
              contains_transform = true;
              no_error = openGEX_Transform(current_ref, matrixTransforms, object_only, substructure);
            }
            else{
              no_error = false;
              printf("(((ERROR-> The structure Skeleton can only contain one single structure of Transform!)))\n");
            }
            break;
          default:
            no_error = false;
            printf("(((ERROR! The structure Skeleton has an invalid substructure!)))\n");
            break;
          }
        }
        //Chek that no substructure is missing!
        if (!contains_bone_ref || !contains_transform){
          no_error = false;
          printf("(((ERROR!-> The structure Skeleton is missing some of their substructures!)))\n");
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Skin structure
      /// @param  skin_skeleton This is a special structure with both the references to a octet::skin and octet::skeleton
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @param  instance  This is an atom (equivalente to string! but cheaper!) with the name of the mesh_instance that contains the mesh
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Skin(ref_skin_skeleton &skin_skeleton, openDDL_identifier_structure *structure, atom_t instance){
        bool no_error = true;
        //Check properties (this Structure cannot have properties!)
        int num_properties = structure->get_number_properties();
        if (num_properties != 0){
          no_error = false;
          printf("(((ERROR!! The structure Skin cannot have properties!)))\n");
        }
        //Check substructures, it has to have 1 and only 1 of each one of these: 
        //            Skeleton, BoneCountArray, BoneIndexArray, BoneWeightArray
        //And it may have one or none of Transform
        int num_substructures = structure->get_number_substructures();
        bool contains_transform = false;
        bool contains_skeleton = false;
        bool contains_bone_count = false;
        bool contains_bone_index = false;
        bool contains_bone_weight = false;
        bool object_only = false;
        dynarray<ref_transform> list_ref;
        dynarray<mat4t> transformMatrixes;
        dynarray<atom_t> bone_array;
        dynarray<mat4t> bindToModel;
        dynarray<int> boneCountArray;
        dynarray<int> boneIndexArray;
        dynarray<int> boneWeightArray;
        if (transformMatrixes.size() < 1)
          transformMatrixes.resize(1);
        //Get ready the skin and skeleton...
        skin_skeleton.ref_skin = new skin();
        skin_skeleton.ref_skeleton = new skeleton(); 
        skin_skeleton.maxCount = 0;
        //Now check all the substructures
        for (int i = 0; i < num_substructures; ++i){
          ref_transform current_ref;
          current_ref.ref = atom_;
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *) structure->get_substructure(i);
          openDDL_data_list * data_list_array;
          //Check the type of the substructure
          switch (substructure->get_identifierID()){
          case 32: //Transform
            if (!contains_transform){
              contains_transform = true;
              no_error = openGEX_Transform(current_ref, transformMatrixes, object_only, substructure);
              if (current_ref.ref != atom_){
                list_ref.push_back(current_ref);
              }
            }
            else{
              no_error = false;
              printf("(((ERROR: The structure Skin has two or more Transform. It's invalid, it can only have one Transform!)))\n");
            }
            break;
          case 27: //Skeleton
            if (!contains_skeleton){
              contains_skeleton = true;
              no_error = openGEX_Skeleton(bone_array, bindToModel, substructure, instance);
            }
            else{
              no_error = false;
              printf("(((ERROR: The structure Skin has two or more Skeleton. It's invalid, it can only have one Skeleton!)))\n");
            }
            break;
          case 2:  //BoneCountArray
            if (!contains_bone_count){
              contains_bone_count = true;
              data_list_array = ((openDDL_data_type_structure*)substructure->get_substructure(0))->get_data_list(0);
              int size_data_list_array = data_list_array->data_list.size();
              for (int i = 0; i < size_data_list_array; ++i){
                int value = data_list_array->data_list[i]->value.integer_;
                boneCountArray.push_back(value);
                if (value>skin_skeleton.maxCount) skin_skeleton.maxCount = value;
              }
            }
            else{
              no_error = false;
              printf("(((ERROR: The structure Skin has two or more BoneCountArray. It's invalid, it can only have one BoneCountArray!)))\n");
            }
            break;
          case 3:  //BoneIndexArray
            if (!contains_bone_index){
              contains_bone_index = true;
              data_list_array = ((openDDL_data_type_structure*)substructure->get_substructure(0))->get_data_list(0);
              int size_data_list_array = data_list_array->data_list.size();
              for (int i = 0; i < size_data_list_array; ++i){
                int value = data_list_array->data_list[i]->value.integer_;
                boneIndexArray.push_back(value);
              }
            }
            else{
              no_error = false;
              printf("(((ERROR: The structure Skin has two or more BoneIndexArray. It's invalid, it can only have one BoneIndexArray!)))\n");
            }
            break;
          case 6:  //BoneWeightArray
            if (!contains_bone_weight){
              contains_bone_weight = true;
              data_list_array = ((openDDL_data_type_structure*)substructure->get_substructure(0))->get_data_list(0);
              int size_data_list_array = data_list_array->data_list.size();
              for (int i = 0; i < size_data_list_array; ++i){
                float value = data_list_array->data_list[i]->value.float_;
                boneWeightArray.push_back(value);
              }
            }
            else{
              no_error = false;
              printf("(((ERROR: The structure Skin has two or more BoneWeightArray. It's invalid, it can only have one BoneWeightArray!)))\n");
            }
            break;
          default:
            no_error = false;
            printf("(((ERROR->The structure Skin has an invaled substructure!)))\n");
            break;
          }
        }
        //Post-processing info of the Skin. Check if it has Skeleton, and the Bone-----Array
        if (!contains_skeleton || !contains_bone_count || !contains_bone_index || !contains_bone_weight){
          no_error = false;
          printf("(((ERROR!! The structure Skin is missing some of their substructures!)))\n");
        }
        else{ //It contains all that it needs, so, check it!
          //First of all process the boneWeight and boneIndex
          int size_count = boneCountArray.size();
          skin_skeleton.boneIndexWeigthArray.resize(size_count);
          int realIndex = 0;
          for (int i = 0; i < size_count; ++i){
            int current_count = boneCountArray[i];
            skin_skeleton.boneIndexWeigthArray[i].resize(skin_skeleton.maxCount);
            int i_bone = 0;
            while (i_bone < current_count){
              skin_skeleton.boneIndexWeigthArray[i][i_bone].index = boneIndexArray[realIndex];
              skin_skeleton.boneIndexWeigthArray[i][i_bone].weight = boneCountArray[realIndex];
              ++realIndex;
              ++i_bone;
            }
            while (i_bone < skin_skeleton.maxCount){
              skin_skeleton.boneIndexWeigthArray[i][i_bone].index = 1;
              skin_skeleton.boneIndexWeigthArray[i][i_bone].weight = 0;
              ++i_bone;
            }
          }
          //Set the skin with the given transform (identity if it has no transform!)
          skin_skeleton.ref_skin->set_bindToModel(transformMatrixes[0].transpose4x4());
          //Post process everything!
          unsigned int number_bones = bone_array.size();

          for (unsigned int i_bone = 0; i_bone < number_bones; ++i_bone){
            atom_t atom_bone = bone_array[i_bone];
            scene_node *current_bone = dict_bone_nodes[app_utils::get_atom_name(bone_array[i_bone])];
            scene_node *bone_parent = current_bone->get_parent();
            int parent;
            if (bone_parent == NULL){
              parent = -1;
            }
            else{
              parent = skin_skeleton.ref_skeleton->get_bone_index(bone_parent->get_sid());
            }
            skin_skeleton.ref_skeleton->add_bone(current_bone, parent);
            skin_skeleton.ref_skin->add_joint(bindToModel[i_bone], current_bone->get_sid());
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Mesh structure
      /// @param  objectRef This is the name of the object that contains this mesh!
      /// @param  lod Level of detail (it has to be different for any mesh in a same GeometryObject
      /// @param  structure This is the structure to be analized, it has to be Node.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Mesh(char * objectRef, int &lod, openDDL_identifier_structure *structure){
        bool no_error = true;
        int tempID;
        uint16_t valuePrimitive = GL_TRIANGLES;
        //Check properties (lod and primitive)
        int numProperties = structure->get_number_properties();
        for (int i = 0; i < numProperties; ++i){
          openDDL_properties *current_property;
          current_property = structure->get_property(i);
          tempID = identifiers_.get_value(current_property->identifierID);
          switch (tempID){
          case 45:
            //Property lod
            lod = current_property->literal.value.u_integer_literal_;
            break;
          case 50:
            //Property primitive
            //The primitives can ve different types (check enum Primitives)
            char * new_primitive;
            int size_primitive;
            size_primitive = current_property->literal.size_string_;
            new_primitive = current_property->literal.value.string_;
            switch (size_primitive){ //Check the size of the primitive
            case 6: //points
              if (same_word("points", new_primitive, size_primitive)){
                valuePrimitive = GL_POINTS;
              }
              else{
                printf("(((ERROR! The property primitive has a wrong content!)))\n");
                no_error = false;
              }
              break;
            case 5: //lines or quads
              if (same_word("lines", new_primitive, size_primitive)){
                valuePrimitive = GL_LINES;
              }
              else if (same_word("quads", new_primitive, size_primitive)){
                valuePrimitive = GL_QUADS;
              }
              else{
                printf("(((ERROR! The property primitive has a wrong content!)))\n");
                no_error = false;
              }
              break;
            case 10://line_strip
              if (same_word("line_strip", new_primitive, size_primitive)){
                valuePrimitive = GL_LINE_STRIP;
              }
              else{
                printf("(((ERROR! The property primitive has a wrong content!)))\n");
                no_error = false;
              }
              break;
            case 9: //triangles
              if (same_word("triangles", new_primitive, size_primitive)){
                valuePrimitive = GL_TRIANGLES;
              }
              else{
                printf("(((ERROR! The property primitive has a wrong content!)))\n");
                no_error = false;
              }
              break;
            case 14://triangle_strip
              if (same_word("triangle_strip", new_primitive, size_primitive)){
                valuePrimitive = GL_TRIANGLE_STRIP;
              }
              else{
                printf("(((ERROR! The property primitive has a wrong content!)))\n");
                no_error = false;
              }
              break;
            default://ERROR!
              printf("(((ERROR! The property primitive has a wrong content!)))\n");
              no_error = false;
              break;
            }
            break;
          default:
            printf("(((ERROR: Property %i non valid!)))\n", tempID);
            no_error = false;
            break;
          }
        }
        //Check substructures (VertexArray (1 or more), IndexArray (0 or 1), Skin (0 or 1))
        bool position = false;
        bool normal = false;
        bool texcoord = false;
        int numSubstructures = structure->get_number_substructures();
        int numVertexArray = 0;
        int numIndexArray = 0;
        int numSkin = 0;
        ref_skin_skeleton skin_skeleton;
        skin_skeleton.ref_skeleton = NULL;
        skin_skeleton.ref_skin = NULL;
        mesh::vertex * vertices = NULL;
        dynarray<uint32_t *> indices;
        dynarray<unsigned int> material_indexes;
        dynarray<int> num_indices;
        int num_vertexes;
        //Check all the substructures (all of them has to be of mesh type)
        for (int i = 0; i < numSubstructures; ++i){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          switch (tempID){
          case 35://VertexArray
            ++numVertexArray;
            int current_attrib;
            //This will translate those words to the following values:
            // position = 0, normal = 1, texcoord = 2, tangent = 3, bitangent = 4, color = 5
            no_error = openGEX_VertexArray(vertices, num_vertexes, current_attrib, substructure);
            switch (current_attrib){
            case 0:
              position = true;
              break;
            case 1:
              normal = true;
              break;
            case 2:
              texcoord = true;
              break;
            }
            break;
          case 12://IndexArray
            indices.push_back(NULL);
            material_indexes.push_back(0);
            num_indices.push_back(0);
            no_error = openGEX_IndexArray(indices[numIndexArray], num_indices[numIndexArray], material_indexes[numIndexArray], substructure);
            ++numIndexArray;
            break;
          case 28://Skin
            if (numSkin == 0){
              ++numSkin;
              if (check_skin_skeleton) no_error = openGEX_Skin(skin_skeleton, substructure, app_utils::get_atom(objectRef));
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
        if (numVertexArray < 1){
          no_error = false;
          printf("(((ERROR: The structure Mesh has to have one VertexArray substructure)))\n");
        }
        else{ 
          //Post processing after reading all the substructures!
          unsigned int num_objects = info_meshes_from_objectRef[objectRef].size();
          //It has to process for every single 
          for (unsigned int index_objects = 0; index_objects < num_objects; ++index_objects){
            info_mesh_instance *info_current_object = info_meshes_from_objectRef[objectRef][index_objects];
            //Now create a mesh_isntance for every single IndexArray!
            for (int index_i = 0; index_i < numIndexArray; ++index_i){
              bool add_later_material = false;
              //First, create the mesh!
              mesh *current_mesh = new mesh(skin_skeleton.ref_skin); 
              mesh_instance * current_mesh_instance;
              current_mesh->allocate(sizeof(mesh::vertex) * num_vertexes, sizeof(uint32_t) * num_indices[index_i]);
              current_mesh->set_num_indices(num_indices[index_i]);
              current_mesh->set_num_vertices(num_vertexes);
              current_mesh->set_mode(valuePrimitive);
              current_mesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
              current_mesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);
              current_mesh->add_attribute(attribute_uv, 2, GL_FLOAT, 24);
              //Now fill it with the info that we have (if we don't have some info, add it manually)
              current_mesh->set_params(sizeof(mesh::vertex), num_indices[index_i], num_vertexes, valuePrimitive, GL_UNSIGNED_INT);
              gl_resource::wolock vl(current_mesh->get_vertices());
              gl_resource::wolock il(current_mesh->get_indices());
              uint32_t *idx = il.u32();
              mesh::vertex *vtx = (mesh::vertex *)vl.f32();
              if (texcoord)
                for (int i = 0; i < num_vertexes; ++i){
                  vtx[i] = vertices[i];
                }
              else
                for (int i = 0; i < num_vertexes; ++i){
                  vtx[i] = vertices[i];
                  vtx[i].uv = vec2(0, 1);
                }
              for (int i = 0; i < num_indices[index_i]; ++i){
                idx[i] = indices[index_i][i];
              }
              //Now, obtain the material!
              //This is the material of this mesh
              material *current_material = 0;
              char *current_ref_material = info_current_object->ref_materials[material_indexes[index_i]];
              //If there is no material, add the default material
              if (current_ref_material == "_DE_FA_UL_T"){
                current_material = new material(vec4(0.5, 0, 0));
                //Now, finally, create the mesh_instance!
                //If there is no skeleton
                if (skin_skeleton.ref_skeleton == NULL)
                  current_mesh_instance = new mesh_instance(info_current_object->node, current_mesh, current_material);
                else{
                  //current_mesh_instance = new mesh_instance(info_current_object->node, current_mesh, current_material);
                  current_mesh_instance = new mesh_instance(info_current_object->node, current_mesh, current_material, skin_skeleton.ref_skeleton);
                }
                const char *name = app_utils::get_atom_name(info_current_object->name);
                char *new_name = new char[20];
                if (index_i > 0){
                  new_name[0] = get_char_from_int(index_i);
                  for (int i = 1; i < 20 && *name != '/0'; ++i){
                    new_name[i] = *name;
                    ++name;
                  }
                  dict->set_resource(new_name, current_mesh_instance);
                }
                else{
                  dict->set_resource(name, current_mesh_instance);
                }
              }
              else{//if there is a material, is more complex, so add the references if it has not been already obtained
                  //Check if it's been obtained already or not
                if (ref_materials[current_ref_material]){
                  current_material = ref_materials[current_ref_material];
                  if (current_material == NULL)
                    add_later_material = true;
                }
                else{ //It's NULL!
                  add_later_material = true;
                }
                //Now, finally, create the mesh_instance!
                //If there is no skeleton
                if (skin_skeleton.ref_skeleton == NULL)
                  current_mesh_instance = new mesh_instance(info_current_object->node, current_mesh, current_material);
                else{
                  //current_mesh_instance = new mesh_instance(info_current_object->node, current_mesh, current_material);
                  current_mesh_instance = new mesh_instance(info_current_object->node, current_mesh, current_material, skin_skeleton.ref_skeleton);
                }
                if (add_later_material){
                  current_material = NULL;
                  ref_materials[current_ref_material] = current_material;
                  ref_materials_inv[current_ref_material].push_back(current_mesh_instance);
                }
                const char *name = app_utils::get_atom_name(info_current_object->name);
                char *new_name = new char[20];
                if (index_i > 0){
                  new_name[0] = get_char_from_int(index_i);
                  for (int i = 1; i < 20 && *name != '/0'; ++i){
                    new_name[i] = *name;
                    ++name;
                  }
                  dict->set_resource(new_name, current_mesh_instance);
                }
                else{
                    dict->set_resource(name, current_mesh_instance);
                }
              }
            }
          }
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a Material structure
      /// @param  structure This is the structure to be analized, it has to be Material.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_Material(openDDL_identifier_structure *structure){
        bool no_error = true;
        bool two_sided = false;
        int tempID;
        //Obtain the name of the structure
        char * name = structure->get_name();
        //Check properties
        int numProperties = structure->get_number_properties();
        if (numProperties > 1){
          printf("(((ERROR! Material structures are supposed to have only 1 property)))\n");
        }
        else if (numProperties == 1){
          openDDL_properties * current_property = structure->get_property(0);
          tempID = identifiers_.get_value(current_property->identifierID);
          if (tempID == 55){
            two_sided = current_property->literal.value.bool_;
          }
          else{
            printf("(((ERROR! The Material structure is expecting a two_sided property)))\n");
            no_error = false;
          }
        }
        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        char * nameNode = NULL;
        int sizeName = 0;
        vec4 value_color;
        GEX_ATTRIB value_attrib;
        float param_value;
        GEX_PARAM param_type = GEX_BEGIN;
        GEX_ATTRIB type_texture;
        uint16_t index_texture;
        char *texture_url = NULL;
        int size_url;
        //Check all the substructures (all of them has to be of mesh type)
        int numNames = 0;
        for (int i = 0; i < numSubstructures && no_error; ++i){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          switch (tempID){
          case 21: //Name
            if (numNames == 0){
              ++numNames;
              no_error = openGEX_Name(nameNode, sizeName, substructure);
            }
            else{
              no_error = false;
              printf("(((ERROR: The structure Material can only have as substructure a Mesh)))\n");
            }
            break;
          case 9:  //Color
            no_error = openGEX_Color(value_color, value_attrib, substructure);
            break;
          case 24: //Param
            no_error = openGEX_Param(param_value, param_type, substructure);
            break;
          case 29: //Texture
            no_error = openGEX_Texture(texture_url, size_url, index_texture, type_texture, substructure);
            break;
          default:
            no_error = false;
            printf("(((ERROR: The structure Material cannot have this type of substructure)))\n");
            break;
          }
        }
        //Post process all that info a material of octet
        material * new_material;
        if (texture_url != NULL){ //if the material has a texture
          new_material = new material(new image(texture_url));
        }
        else{//It has no texture
          new_material = new material(value_color);
        }
        //And copy all that into the dict and the mesh_instances referenced
        if (numNames == 0){ //if it has no name, add the name of the structure!
          nameNode = name;
        }
        dict->set_resource(nameNode, new_material);
        int num_instances = ref_materials_inv[name].size();
        for (int i = 0; i < num_instances; ++i){
          ref_materials_inv[name][i]->set_material(new_material);
        }

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a GeometryNode structure
      /// @param  structure This is the structure to be analized, it has to be GeometryNode.
      /// @param  father This is the father scene_node. By default  NULL
      /// @return True if everything went well, false if there was some problem
      ///   Note: This function will check the properties of the structure
      ///   And it will check for the referencies. It will prepare to build the node
      ///   assigning a pointer to a mesh that it will be created later
      ///   GeometryObject will contain the node to the mesh!
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_GeometryNode(openDDL_identifier_structure *structure, scene_node *father = NULL){
        int tempID;
        bool no_error = true;
        bool values_specified[3] = { false, false, false }; //values => visible, shadow, motion_blur
        bool values_properties[3] = { false, false, false };
        ref<info_mesh_instance> info_current_instance = new info_mesh_instance;
        dynarray<mesh_instance *> mesh_instances;
        mesh_instance * current_mesh_instance;
        current_mesh_instance = new mesh_instance;
        //Creating new node
        scene_node *node;
        node = new scene_node();
        //Assign that to the current info
        //Obtain the name of the structure
        char * name = structure->get_name();
        node->set_sid(app_utils::get_atom(name));
        info_current_instance->node = node;
        unsigned int capacity_materials = 5;
        info_current_instance->ref_materials.resize(capacity_materials);
        for (int i = 0; i < 5; ++i){
          info_current_instance->ref_materials[i] = NULL;
        }
        //If it's not a Top-Level class, add it to his father
        if (father != NULL){
          father->add_child(node);
        }
        //Obtain the properties (may not have)
        int numProperties = structure->get_number_properties();
        for (int i = 0; i < numProperties; ++i){
          openDDL_properties *current_property;
          current_property = structure->get_property(i);
          tempID = identifiers_.get_value(current_property->identifierID);
          switch (tempID){
          case 57:
            //Property visible
            values_specified[0] = true;
            values_properties[0] = current_property->literal.value.bool_;
            break;
          case 52:
            //Property shadow
            values_specified[1] = true;
            values_properties[1] = current_property->literal.value.bool_;
            break;
          case 48:
            //Property motion_blur
            values_specified[2] = true;
            values_properties[2] = current_property->literal.value.bool_;
            break;
          default:
            printf("(((ERROR: Property %i non valid!)))\n", tempID);
            break;
          }
        }

        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        //Some variables to check the quantity of some substructures
        int numMorph = 0; //0 or 1
        int numNames = 0; //0 or 1
        int numObjectRef = 0; //It has to be 1!!
        //Creating matrix of transforms
        mat4t nodeToParent;
        nodeToParent.loadIdentity(); //and initialize it to identity!
        //This is to get some info from the substructures
        dynarray<mat4t> transformMatrixes;
        if (transformMatrixes.size() < 1)
          transformMatrixes.resize(1);
        dynarray<ref_transform> list_ref;
        float *values = NULL;
        int numValues;
        unsigned int mat_index;
        int num_mat_index = 0;
        char * nameNode = NULL;
        int sizeName = 0;
        bool object_only = false;
        char * object_ref = NULL;
        char * ref_material = NULL;

        //Check all the substructures
        for (int i = 0; i < numSubstructures; ++i){
          ref_transform current_ref;
          current_ref.ref = atom_;
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          switch (tempID){
          //Get Name (may not have)
          case 21://Name
            if (numNames == 0){
              ++numNames;
              no_error = openGEX_Name(nameNode, sizeName, substructure);
              info_current_instance->name = app_utils::get_atom(nameNode);
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
          //Get ObjectRef (geometryObject) (only 1)
          case 23://ObjectRef
            if (numObjectRef == 0){
              ++numObjectRef;
              no_error = openGEX_ObjectRef(object_ref, substructure);
              if (!info_meshes_from_objectRef.contains(object_ref))
                info_meshes_from_objectRef[object_ref];
              else{
                //Think what will do here TODO
              }
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
          //Get MaterialRef
          case 17://MaterialRef
            mat_index = 0;
            no_error = openGEX_MaterialRef(ref_material, mat_index, substructure);
            if (!ref_materials.contains(ref_material))
              ref_materials[ref_material] = NULL;
            else{
              //Think what will do here TODO
            }
            ++num_mat_index;
            if (mat_index > capacity_materials){
              mat_index += 5;
              info_current_instance->ref_materials.resize(mat_index);
              for (unsigned int i = mat_index - 5 ; i < mat_index ; ++i){
                info_current_instance->ref_materials[i] = NULL;
              }
            }
            info_current_instance->ref_materials[mat_index] = ref_material;
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
            no_error = openGEX_Transform(current_ref, transformMatrixes, object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 33://Translation
            no_error = openGEX_Translate(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 25://Rotation
            no_error = openGEX_Rotate(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 26://Scale
            no_error = openGEX_Scale(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          //Get Animation
          case 0://Animation
            if (check_animation)
              no_error = openGEX_Animation(list_ref, substructure, node);
            break;
          //Get Nodes (children)
          case 4://BoneNode
            no_error = openGEX_BoneNode(substructure, node);
            break;
          case 7://CameraNode
            //IGNORE CAMERAS FOR NOW!!!! TO DO!
            break;
          case 10://GeometryNode
            no_error = openGEX_GeometryNode(substructure, node);
            break;
          case 14://LightNode
            //IGNORE LIGHTS FOR NOW!!!! TO DO!
            break;
          case 22://Nodes
            no_error = openGEX_Node(substructure, node);
            break;
          }
        }
        //Sum up after reading all substructures
        if (num_mat_index == 0){ //If it has no Material... add a default material
          info_current_instance->ref_materials[0] = "_DE_FA_UL_T";
        }
        if (numObjectRef != 1){
          printf("(((ERROR!! The GeometricNode structure has to have one ObjectRef!)))\n");
          no_error = false;
        } else{
          if (numNames == 0){ //it has no name, so get the structure name
            if (DEBUGOPENGEX) printf("As it has no name, assign the structure name \n");
            nameNode = name;
          }
          // We are working with transpose matrix in octet!!!
          node->access_nodeToParent().multMatrix(nodeToParent.transpose4x4());
          //Add the id for the animations
          node->set_sid(app_utils::get_atom(nameNode));
          info_meshes_from_objectRef[object_ref].push_back(info_current_instance);
        }
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a GeometryObject structure
      /// @param  structure This is the structure to be analized, it has to be GeometryNode.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_GeometryObject(openDDL_identifier_structure *structure){
        int tempID;
        bool no_error = true;
        bool values_specified[3] = { false, false, false }; //values => visible, shadow, motion_blur
        bool values_properties[3] = { false, false, false };
        //Creating matrix of transforms
        mat4t nodeToParent;
        nodeToParent.loadIdentity(); //and initialize it to identity!
        //Obtain the name of the structure
        char * name = structure->get_name();
        //Obtain the properties (may not have)
        int numProperties = structure->get_number_properties();
        for (int i = 0; i < numProperties; ++i){
          openDDL_properties *current_property;
          current_property = structure->get_property(i);
          tempID = identifiers_.get_value(current_property->identifierID);
          switch (tempID){
          case 57:
            //Property visible
            values_specified[0] = true;
            values_properties[0] = current_property->literal.value.bool_;
            break;
          case 52:
            //Property shadow
            values_specified[1] = true;
            values_properties[1] = current_property->literal.value.bool_;
            break;
          case 48:
            //Property motion_blur
            values_specified[2] = true;
            values_properties[2] = current_property->literal.value.bool_;
            break;
          default:
            printf("(((ERROR: Property %i non valid!)))\n", tempID);
            break;
          }
        }
        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        dynarray<int> lod(numSubstructures);
        //Check all the substructures (all of them has to be of mesh type)
        for (int i = 0; i < numSubstructures && no_error; ++i){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          if (tempID == 18){//Mesh
            lod[i] = 0;
            no_error = openGEX_Mesh(name, lod[i], substructure);
          }
          else{
            no_error = false;
            printf("(((ERROR: The structure GeometricObject can only have as substructure a Mesh)))\n");
          }
        }

        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a CameraNode structure
      /// @param  structure This is the structure to be analized, it has to be CameraNode.
      /// @param  father This is the father scene_node. By default  NULL
      /// @return True if everything went well, false if there was some problem
      ///   Note: This function will check the properties of the structure
      ///   And it will check for the referencies. It will prepare to build the node
      ///   assigning a pointer to a mesh that it will be created later
      ///   GeometryObject will contain the node to the mesh!
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_CameraNode(openDDL_identifier_structure *structure, scene_node *father = NULL){
        int tempID;
        bool no_error = true;
        ref<info_camera_instance> info_current_instance = new info_camera_instance;
        camera_instance * current_camera_instance;
        current_camera_instance = new camera_instance;
        //Creating new node
        scene_node *node;
        node = new scene_node();
        //Assign that to the current info
        //Obtain the name of the structure
        char * name = structure->get_name();
        node->set_sid(app_utils::get_atom(name));
        info_current_instance->node = node;
        //If it's not a Top-Level class, add it to his father
        if (father != NULL){
          father->add_child(node);
        }
        //Camera node has no properties
        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        //Some variables to check the quantity of some substructures
        int numNames = 0; //0 or 1
        int numObjectRef = 0; //It has to be 1!!
        //Creating matrix of transforms
        mat4t nodeToParent;
        nodeToParent.loadIdentity(); //and initialize it to identity!
        //This is to get some info from the substructures
        dynarray<mat4t> transformMatrixes;
        if (transformMatrixes.size() < 1)
          transformMatrixes.resize(1);
        dynarray<ref_transform> list_ref;
        float *values = NULL;
        int numValues;
        unsigned int mat_index;
        int num_mat_index = 0;
        char * nameNode = NULL;
        int sizeName = 0;
        bool object_only = false;
        char * object_ref = NULL;
        char * ref_material = NULL;

        //Check all the substructures
        for (int i = 0; i < numSubstructures; ++i){
          ref_transform current_ref;
          current_ref.ref = atom_;
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          switch (tempID){
            //Get Name (may not have)
          case 21://Name
            if (numNames == 0){
              ++numNames;
              no_error = openGEX_Name(nameNode, sizeName, substructure);
              info_current_instance->name = app_utils::get_atom(nameNode);
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
            //Get ObjectRef (geometryObject) (only 1)
          case 23://ObjectRef
            if (numObjectRef == 0){
              ++numObjectRef;
              no_error = openGEX_ObjectRef(object_ref, substructure);
              if (!info_cameras_from_objectRef.contains(object_ref))
                info_cameras_from_objectRef[object_ref];
              else{
                //Think what will do here TODO
              }
            }
            else{
              printf("(((ERROR: It has more than one Morph, it can only have one (or none)!!!)))\n");
            }
            break;
            //Get Transforms (may not have)
          case 32://Transform
            no_error = openGEX_Transform(current_ref, transformMatrixes, object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 33://Translation
            no_error = openGEX_Translate(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 25://Rotation
            no_error = openGEX_Rotate(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
          case 26://Scale
            no_error = openGEX_Scale(current_ref, transformMatrixes[0], object_only, substructure);
            nodeToParent.multMatrix(transformMatrixes[0]);
            list_ref.push_back(current_ref);
            break;
            //Get Animation
          case 0://Animation
            if (check_animation)
              no_error = openGEX_Animation(list_ref, substructure, node);
            break;
            //Get Nodes (children)
          case 4://BoneNode
            no_error = openGEX_BoneNode(substructure, node);
            break;
          case 7://CameraNode
            no_error = openGEX_CameraNode(substructure, node);
            break;
          case 10://GeometryNode
            no_error = openGEX_GeometryNode(substructure, node);
            break;
          case 14://LightNode
            //IGNORE LIGHTS FOR NOW!!!! TO DO!
            break;
          case 22://Nodes
            no_error = openGEX_Node(substructure, node);
            break;
          }
        }
        //Sum up after reading all substructures
        if (numObjectRef != 1){
          printf("(((ERROR!! The GeometricNode structure has to have one ObjectRef!)))\n");
          no_error = false;
        }
        else{
          if (numNames == 0){ //it has no name, so get the structure name
            if (DEBUGOPENGEX) printf("As it has no name, assign the structure name \n");
            nameNode = name;
          }
          // We are working with transpose matrix in octet!!!
          node->access_nodeToParent().multMatrix(nodeToParent.transpose4x4());
          //Add the id for the animations
          node->set_sid(app_utils::get_atom(nameNode));
          info_cameras_from_objectRef[object_ref].push_back(info_current_instance);
        }
        return no_error;
      }
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will obtain all the info from a CameraObject structure
      /// @param  structure This is the structure to be analized, it has to be CameraObject.
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_CameraObject(openDDL_identifier_structure *structure){
        int tempID;
        bool no_error = true;
        //Creating matrix of transforms
        mat4t nodeToParent;
        nodeToParent.loadIdentity(); //and initialize it to identity!
        //Obtain the name of the structure
        char * name = structure->get_name();
        //Ignore the properties
        //Check substructures
        int numSubstructures = structure->get_number_substructures();
        //Check all the substructures (all of them has to be of Param type)
        dynarray<float> param_value;
        param_value.resize(numSubstructures);
        dynarray<GEX_PARAM> param_type;
        param_type.resize(numSubstructures);
        for (int i = 0; i < numSubstructures && no_error; ++i){
          openDDL_identifier_structure *substructure = (openDDL_identifier_structure *)structure->get_substructure(i);
          tempID = substructure->get_identifierID();
          if (tempID == 24){//Param
            no_error = openGEX_Param(param_value[i], param_type[i], substructure);
          }
          else{
            no_error = false;
            printf("(((ERROR: The structure CameraObject can only have Param substructures!)))\n");
          }
        }
        //Process all information (creating a mesh!)

        return no_error;
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
            no_error = openGEX_GeometryNode(structure);
            break;
          case 11: //GeometryObject
            if (DEBUGOPENGEX) printf("GeometryObject\n");
            //Process GeometryObject structure
            no_error = openGEX_GeometryObject(structure);
            break;
          case 16: //Material
            if (DEBUGOPENGEX) printf("Material\n");
            //Process Material structure
            no_error = openGEX_Material(structure);
            break;
          case  4: //BoneNode
            if (DEBUGOPENGEX) printf("BoneNode\n");
            no_error = openGEX_BoneNode(structure);
            break;
          case 22: //Node
            if (DEBUGOPENGEX) printf("BoneNode\n");
            no_error = openGEX_Node(structure);
            break;
          default:
            printf("(((ERROR!!!!-> It's reading a structure not suposed to be Top Level. Are you sure?)))\n");
            no_error = false;
          }
        }
        return no_error;
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
          if (DEBUGSTRUCTURE) no_error = printf_identifier_structure((openDDL_identifier_structure*)structure); 
          no_error = openGEX_identifier_structure((openDDL_identifier_structure*)structure);
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
      ///   This function will only call to the initializers of openDDL process and openGEX process
      ////////////////////////////////////////////////////////////////////////////////
      openGEX_parser(){
        init_ddl();
        init_gex();
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This function will analize all the data obtained by the openDDL lexer process
      /// @param  new_dict This is a pointer to the dictionary of resources, this will be a return outcome of the openDDL lexer process
      /// @param  skin_skeleton This is just a boolean that determines if we want to obtain the skin_skeleton or not
      /// @param  animation This is just a boolean that determines if we want to obtain the animation or not
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool openGEX_data(resource_dict *new_dict, bool animation, bool skin_skeleton){
        dict = new_dict;
        check_skin_skeleton = skin_skeleton;
        check_animation = animation;
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
          no_error = openGEX_structure(topLevelStructure);
        }
        if (DEBUGSTRUCTURE) printf("\n");
        return no_error;
      }
    };
    }
  }
}

#endif
