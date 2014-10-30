////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_structures.h
/// @author Juanmi Huertas Delgado
/// @brief This file containts the data object for structures in openGEX
////////////////////////////////////////////////////////////////////////////////


#ifndef OPENGEX_STRUCTURES_INCLUDED
#define OPENGEX_STRUCTURES_INCLUDED

namespace octet{
  namespace loaders{

    struct openGEX_property{
      int propertyID;
      openDDL_data_literal default;
    };

    struct openGEX_structure{
      bool identifier;
      int structureID;
      int min;
      int max;
    };
  }
}

#endif