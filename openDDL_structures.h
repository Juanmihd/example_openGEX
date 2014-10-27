////////////////////////////////////////////////////////////////////////////////
/// @file openDDL_structures.h
/// @author Juanmi Huertas Delgado
/// @brief This file containts the data object for structures
///
////////////////////////////////////////////////////////////////////////////////

#ifndef OPENDDL_STRUCTURES_INCLUDED
#define OPENDDL_STRUCTURES_INCLUDED

////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openDDLstructure, it contains the classes and sublcasses to store all the info of the openDDL file
////////////////////////////////////////////////////////////////////////////////

namespace octet{
  namespace loaders{
    enum structureType { data_type_structure, identifier_structure, not_yet_type };

    class openDDLproperties{

    };

    class openDDLstructure{
    protected:
      structureType type;
      int mainID;
      int nameID; 

      void init(){
        mainID = 0;
        nameID = 0;
        type = not_yet_type;
      }
    public:
      openDDLstructure(){
        init();
      }
    };

    class openDDL_identifier_structure : public openDDLstructure{
    protected:
      dynarray<openDDLproperties> propertiesList;
      dynarray<openDDLstructure> substructureList;
    public:
      openDDL_identifier_structure(){
        type = identifier_structure;
      }
    };

    class openDDL_data_type_structure : public openDDLstructure{
    protected:
      dynarray<openDDLproperties> propertiesList;
      dynarray<openDDLstructure> substructureList;
    public:
      openDDL_data_type_structure(){
        type = data_type_structure;
      }
    };
  }
}

#endif