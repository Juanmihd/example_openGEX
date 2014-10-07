/////////////////////////////////////////////////////////////////
///
/// @filename openDLL_tokens.h
/// @author Juanmi Huertas Delgado
/// @brief This are the openDDL tokens that will use, creating the structures to import them in a dictionary later
///

namespace octet{
  namespace loaders{
    /// @brief This will be the list of tokens used in ddl
    struct ddl_token_enum{
      enum token_type
      {
        tok_bool, tok_int8, tok_int16, tok_int32, tok_int64, tok_uint8, tok_uint16, tok_uint32, tok_uint64, tok_float, tok_double, tok_string, tok_ref, tok_type,

        tok_true, tok_false,

        tok_lbrace, tok_rbrace, tok_lbracket, tok_rbracket, tok_lparen, tok_rparen, tok_comma, tok_equal,

        tok_last,
      };
    };

    /// @brief THis will be the class of ddl tokens
    class ddl_token : public ddl_token_enum
    {
    public:
      /// @brief This function will return the token_name of the given token identifier
      static const char *token_name(token_type t){
        static const char token_names[tok_last][4] = {
          "b", "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64", "f", "d", "s", "r", "ty",
          "tr", "fa",
          "{", "}", "[", "]", "(", ")", ",", "=",
        };
        // It will be an error if we try to call a token biger than our current token list!
        assert((unsigned)t < tok_last);
        return token_names[(int)t];
      }

      /// @brief This will return the enum token_type left brace, that it's the first symbol of the enum
      token_type first_symbol(){
        return tok_lbrace;
      }

      /// @brief This will return the enum token_type equal, that it's the last symbol of the enum
      token_type last_symbol(){
        return tok_equal;
      }

      /// @brief  This function will test if the current token is a whitespace (less than 0x20 character)
       bool is_whiteSpace(unsigned char character){ // everything less or equal than 0x20 is a whitespace
        return character <= 0x20;
      }

      /// @brief  This function will test if the current token is going to be a comment (// or /*)
       bool is_comment(unsigned char * character){ //0x2f = /  and  0x2A = *
         return character[0] == 0x2F && (character[1] == 0x2f || character[1] == 0x2A);
       }

    };
  }
}