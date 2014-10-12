/////////////////////////////////////////////////////////////////
///
/// @filename openDDL_tokens.h
/// @author Juanmi Huertas Delgado
/// @brief This are the openDDL tokens that will use, creating the structures to import them in a dictionary later
///

namespace octet{
  namespace loaders{
    /// @brief This will be the list of tokens used in ddl
    struct ddl_token_enum{
      enum token_type
      {
        tok_bool, 
        tok_int8, tok_int16, tok_int32, tok_int64, 
        tok_uint8, tok_uint16, tok_uint32, tok_uint64, 
        tok_float, tok_double, tok_string, tok_ref, tok_type,

        tok_true, tok_false,

        tok_lbrace, tok_rbrace, tok_lbracket, tok_rbracket, tok_lparen, tok_rparen, tok_comma, tok_equal,

        tok_last,
      };
    };


    /// @brief This will be the class of ddl tokens
    class ddl_token : public ddl_token_enum
    {

    public:
      /// @brief This function will return the token_name of the given token identifier
      static const string token_name(unsigned t){
        static const string token_names[tok_last] = {
          "bool", 
          "int8", "int16", "int32", "int64", 
          "unsigned_int8", "unsigned_int16", "unsigned_int32", "unsigned_int64", 
          "float", "double", "string", "ref", "type",
          "true", "false",
          "{", "}", "[", "]", "(", ")", ",", "=",
        };
        // It will be an error if we try to call a token bigger than our current token list!
        assert(((unsigned)t < tok_last) && "Calling a token bigger than the token list!");
        return token_names[(int)t];
      }

      /// @brief This will return the enum token_type bool, that it's the first type of the enum
      token_type first_type(){
        return tok_bool;
      }

      /// @brief This will return the enum token_type type, that it's the last type of the enum
      token_type last_type(){
        return tok_type;
      }

      /// @brief This will return the enum token_type left brace, that it's the first symbol of the enum
      token_type first_symbol(){
        return tok_lbrace;
      }

      /// @brief This will return the enum token_type equal, that it's the last symbol of the enum
      token_type last_symbol(){
        return tok_equal;
      }
    };
  }
}