////////////////////////////////////////////////////////////////////////////////
/// @file openDDL_structures.h
/// @author Juanmi Huertas Delgado
/// @brief This file containts the data object for structures
///     A list of the classes that this file containts is:
///   class openDDL_structure;
///   class openDDL_identifier_structure;
///   class openDDL_data_type_structure;
///   class openDDL_structure;
///   struct openDDL_data_list;
///   struct openDDL_properties;
///   struct openDDL_data_literal;
///     It contains also some enums (for structureType, and value_type_DDL)
////////////////////////////////////////////////////////////////////////////////

#ifndef OPENDDL_STRUCTURES_INCLUDED
#define OPENDDL_STRUCTURES_INCLUDED

namespace octet{
  namespace loaders{
    class openDDL_structure;
    class openDDL_identifier_structure;
    class openDDL_data_type_structure;
    class openDDL_structure;
    struct openDDL_data_list;
    struct openDDL_properties;
    struct openDDL_data_literal;
    
    enum structureType { NOT_YET_TYPE = -1, IDENTIFIER_TYPE = 0, DATA_TYPE_TYPE = 1 };
    enum value_type_DDL   { UINT = 0, INT = 1, BOOL = 2, FLOAT = 3, STRING = 4, REF = 5, TYPE = 6};

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief This is a handy function that converts from openDDL_token to this literal type
    /// @param  token This is the type in "token mode"
    /// @return The integer with the type in "literal mode"
    ////////////////////////////////////////////////////////////////////////////////
    int convert_type_token_to_DDL(int token){
      return (token == 0 ? 2 : (token < 5 ? 1 : (token < 9 ? 0 : (token < 11 ? 3 : token - 7))));
    }

////////////////////////////////////////////////////////////////////////////////
/// @brief This struct is represent a data_literal
////////////////////////////////////////////////////////////////////////////////
    struct openDDL_data_literal : public resource{
    public:
      value_type_DDL value_type;
      union my_literal {
        unsigned int u_integer_literal_;
        int integer_;
        bool bool_;
        float float_;
        char * string_;
        char * ref_;
        int type_;
      } value;
      int size_string_;
      bool global_ref_;
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief This struct is represent some properties in openDDL
////////////////////////////////////////////////////////////////////////////////
    struct openDDL_properties : public resource {
      int identifierID;
      openDDL_data_literal literal;
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief This struct is represent some data_list in openDDL
////////////////////////////////////////////////////////////////////////////////
    struct openDDL_data_list : public resource{
      value_type_DDL value_type;
      dynarray<openDDL_data_literal*> data_list;
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief This class represents structures in openDDL, it will be a parent class for the different types of structures
////////////////////////////////////////////////////////////////////////////////
    class openDDL_structure : public resource{
    protected:
      structureType type;
      int nameID;
      char * name;
      openDDL_identifier_structure * father_structure;
      //Local names and references
      dictionary<openDDL_structure *> names_;
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will initialize the structure with no name and no type
      ////////////////////////////////////////////////////////////////////////////////
      void init(){
        father_structure = NULL;
        name = NULL;
        nameID = -1;
        type = NOT_YET_TYPE;
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
      /// @brief This will get the name identificator of the class
      /// @return A char pointer to the name
      ////////////////////////////////////////////////////////////////////////////////
      char * get_name(){
        return name;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will set the name identificator of the class
      /// @param  n_nameID  The new identificator of the name
      ////////////////////////////////////////////////////////////////////////////////
      void set_nameID(int n_nameID){
        nameID = n_nameID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will set the name identificator of the class
      /// @param  n_name  The name as a char pointer
      ////////////////////////////////////////////////////////////////////////////////
      void set_name(char * n_name){
        name = n_name;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the type of the structure
      /// @return The type of the structure.  
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

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the father_structure of the structure
      /// @return The father_structure of the structure.  
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_identifier_structure * get_father_structure(){
        return father_structure;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will set the name father_structure of the class
      /// @param  n_father_structure  The new father_structure of the name
      ////////////////////////////////////////////////////////////////////////////////
      void set_father_structure(openDDL_identifier_structure * n_father_structure){
        father_structure = n_father_structure;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the index of the name
      /// @param  name  A char pointer that contains the name
      /// @return -1 if the name has not been initialized yet or the index of the name
      ////////////////////////////////////////////////////////////////////////////////
      int get_local_index(char *name){
        return names_.get_index(name);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the index of the name
      /// @param  name  A char pointer that contains the name
      /// @return -1 if the name has not been initialized yet or the index of the name
      ////////////////////////////////////////////////////////////////////////////////
      int set_local_name(char *name, openDDL_structure * structure){
        names_[name] = structure;
        return names_.get_index(name);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the structure being part of a local name (by index)
      /// @param  index The index to the local name willing to obtain
      /// @return The pointer to the structure with that name
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_structure * get_local_name(int index){
        return names_.get_value(index);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the structure being part of a local name (by name)
      /// @param  name  A char pointer that contains the name
      /// @return The pointer to the structure with that name
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_structure * get_local_name(char *name){
        return names_[name];
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will get the structure being part of a local name (by index)
      /// @param  index The index to the local name willing to obtain
      /// @return The pointer to the char of the name
      ////////////////////////////////////////////////////////////////////////////////
      const char * get_local_name_char(int index){
        return names_.get_key(index);
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
        type = IDENTIFIER_TYPE;
      }
    public:
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Default constructor, with no identificator, no name, but calling init () (type identifier)
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_identifier_structure(){
        init();
        identifierID = -1;
        nameID = -1;
        father_structure = NULL;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Constructor with parameteres, with identificator, name (by default 0), and calling init () (type identifier)
      /// @param  n_identifierID  This will represent the type of the identifier
      /// @param  n_nameID  This is an optional parameter (by default -1) that represent the name of the structure
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_identifier_structure(int n_identifierID, openDDL_identifier_structure * father, int n_nameID = -1){
        init();
        identifierID = n_identifierID;
        nameID = n_nameID;
        father_structure = father;
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

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will add a new property to the structure
      /// @param  new_property  The new property to add
      ////////////////////////////////////////////////////////////////////////////////
      void add_property(openDDL_properties * new_property){
        propertiesList.push_back(new_property);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will return the size of the properties list in the structure
      /// @return Int with the ammount of properties
      ////////////////////////////////////////////////////////////////////////////////
      int get_number_properties(){
        return propertiesList.size();
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will return a given property by an index
      /// @param  index The index of the property to get
      /// @return A pointer to the given property
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_properties *get_property(int index){
        return propertiesList[index];
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will add a new structure to the structure
      /// @param  new_structure  The new structure to add
      ////////////////////////////////////////////////////////////////////////////////
      void add_structure(openDDL_structure *new_structure){
        substructureList.push_back(new_structure);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will return the size of the substructures list in the structure
      /// @return Int with the ammount of substructures
      ////////////////////////////////////////////////////////////////////////////////
      int get_number_substructures(){
        return substructureList.size();
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will return a given substructure by an index
      /// @param  index The index of the substructure to get
      /// @return A pointer to the given substructure
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_structure *get_substructure(int index){
        return substructureList[index];
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
        type = DATA_TYPE_TYPE;
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
        father_structure = NULL;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Constructor with parameteres, with data_type, name (by default 0), and calling init () (type identifier)
      /// @param  n_typeID  This will represent the type of the data_type
      /// @param  n_integer_literal It will receive also a integer that represent the number of elements (if data_list_array) or -1 if it's only a data_list
      /// @param  n_nameID  This is an optional parameter (by default -1) that represent the name of the structure
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_data_type_structure(int n_typeID, int n_integer_literal, openDDL_identifier_structure * father, int n_nameID = -1){
        init();
        typeID = n_typeID;
        integer_literal = n_integer_literal;
        nameID = n_nameID;
        father_structure = father;
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

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will add a new data_list to the structure
      /// @param  new_data_list  The new data_list to add
      ////////////////////////////////////////////////////////////////////////////////
      void add_data_list(openDDL_data_list * new_data_list){
        data_list_array.push_back(new_data_list);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This obtain the number of data_lists (or arrays) in the structure
      /// @returns The ammount of data_lists inside the structure
      ////////////////////////////////////////////////////////////////////////////////
      int get_number_lists(){
        return data_list_array.size();
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This get a data_list by a given index
      /// @param  index  The index of the data_list requested
      /// @returns A pointer to the asked data_list
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_data_list *get_data_list(int index){
        return data_list_array[index];
      }
    };
  }
}

#endif