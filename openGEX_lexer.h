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
    };
  }
}

#endif