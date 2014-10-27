////////////////////////////////////////////////////////////////////////////////
/// @file openDDL_structures.h
/// @author Juanmi Huertas Delgado
/// @brief This file containts the data object for structures
///
////////////////////////////////////////////////////////////////////////////////

#ifndef OPENDDL_STRUCTURES_INCLUDED
#define OPENDDL_STRUCTURES_INCLUDED

namespace octet{
  namespace loaders{
    enum structureType { not_yet_type = -1, identifier_structure = 0, data_type_structure = 1};

////////////////////////////////////////////////////////////////////////////////
/// @brief This struct is represent a data_literal
////////////////////////////////////////////////////////////////////////////////
    struct openDDL_data_literal : public resource{
    public:
      enum { INT, BOOL, FLOAT, STRING, REF, TYPE };
      union {
        int integer_literal;
        bool bool_literal;
        float float_literal;
        int reference_literal;
        int data_type_literal;
        char * string_literal;
      };
      int size_string_literal;
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief This struct is represent some properties in openDDL
////////////////////////////////////////////////////////////////////////////////
    struct openDDL_properties : public resource {
      int identifierID;
      union {
        int integer_literal;
        bool bool_literal;
        float float_literal;
        int reference_literal;
        int data_type_literal;
      };
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief This struct is represent some data_list in openDDL
////////////////////////////////////////////////////////////////////////////////
    struct openDDL_data_list : public resource{
      dynarray<openDDL_data_literal> data_list;
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief This class represents structures in openDDL, it will be a parent class for the different types of structures
////////////////////////////////////////////////////////////////////////////////
    class openDDL_structure : public resource{
    protected:
      structureType type;
      int nameID; 
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will initialize the structure with no name and no type
      ////////////////////////////////////////////////////////////////////////////////
      void init(){
        nameID = -1;
        type = not_yet_type;
      }
    public:

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This is the default constructor, will call init() (no type and no name)
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_structure(){
        init();
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the name identificator of the class
      /// @return The identificator of the name. If it has no name it will return -1
      ////////////////////////////////////////////////////////////////////////////////
      int get_nameID(){
        return nameID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will set the name identificator of the class
      /// @param  n_nameID  The new identificator of the name
      ////////////////////////////////////////////////////////////////////////////////
      void set_nameID(int n_nameID){
        nameID = n_nameID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the type of the structure
      /// @return The type of the structure. Being 
      ////////////////////////////////////////////////////////////////////////////////
      structureType get_type_structure(){
        return type;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will set the name identificator of the class
      /// @param  n_nameID  The new identificator of the name
      ////////////////////////////////////////////////////////////////////////////////
      void set_type_structure(structureType n_type){
        type = n_type;
      }
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief This class represents structures with identifier, it may have properties and it will have substructures
////////////////////////////////////////////////////////////////////////////////
    class openDDL_identifier_structure : public openDDL_structure{
    protected:
      int identifierID;
      dynarray<openDDL_properties*> propertiesList;
      dynarray<openDDL_structure*> substructureList;

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will initialize the structure with type "identifier"
      ////////////////////////////////////////////////////////////////////////////////
      void init(){
        type = identifier_structure;
      }
    public:

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Default constructor, with no identificator, no name, but calling init () (type identifier)
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_identifier_structure(){
        init();
        identifierID = -1;
        nameID = -1;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Constructor with parameteres, with identificator, name (by default 0), and calling init () (type identifier)
      /// @param  n_identifierID  This will represent the type of the identifier
      /// @param  n_nameID  This is an optional parameter (by default -1) that represent the name of the structure
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_identifier_structure(int n_identifierID, int n_nameID = -1){
        init();
        identifierID = n_identifierID;
        nameID = n_nameID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the identifier of the structure
      /// @return The identificator of the identifier. If it has no identifier it will return -1
      ////////////////////////////////////////////////////////////////////////////////
      int get_identifierID(){
        return identifierID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will set the identifier of the structure
      /// @param  n_nameID  The new identificator of the identifier
      ////////////////////////////////////////////////////////////////////////////////
      void set_identifierID(int n_identifierID){
        identifierID = n_identifierID;
      }
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief This class represents structures of data type, it may be data_list or data_list_array
////////////////////////////////////////////////////////////////////////////////
    class openDDL_data_type_structure : public openDDL_structure{
    protected:
      int typeID;
      int integer_literal;
      dynarray<ref<openDDL_data_list>> data_list_array;
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will initialize the structure with type "data_ñtype"
      ////////////////////////////////////////////////////////////////////////////////
      void init(){
        type = data_type_structure;
      }
    public:

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Default constructor, with no data_type, no name, but calling init () (type identifier)
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_data_type_structure(){
        init();
        typeID = -1;
        nameID = -1;
        integer_literal = -1;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Constructor with parameteres, with data_type, name (by default 0), and calling init () (type identifier)
      /// @param  n_typeID  This will represent the type of the data_type
      /// @param  n_integer_literal It will receive also a integer that represent the number of elements (if data_list_array) or -1 if it's only a data_list
      /// @param  n_nameID  This is an optional parameter (by default -1) that represent the name of the structure
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_data_type_structure(int n_typeID, int n_integer_literal, int n_nameID = -1){
        init();
        typeID = n_typeID;
        integer_literal = n_integer_literal;
        nameID = n_nameID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the data type of the structure
      /// @return The identificator of the data type. If it has no data type it will return -1
      ////////////////////////////////////////////////////////////////////////////////
      int get_typeID(){
        return typeID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will set the data type of the structure
      /// @param  n_nameID  The new identificator of the data type
      ////////////////////////////////////////////////////////////////////////////////
      void set_typeID(int n_typeID){
        typeID = n_typeID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the integer_literal of the structure
      /// @return The identificator of the integer_literal. If it has no integer_literal it will return -1
      ////////////////////////////////////////////////////////////////////////////////
      int get_integer_literal(){
        return integer_literal;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will set the integer_literal of the structure
      /// @param  n_nameID  The new identificator of the integer_literal
      ////////////////////////////////////////////////////////////////////////////////
      void set_integer_literal(int n_integer_literal){
        integer_literal = n_integer_literal;
      }
    };
  }
}

#endif