////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////

#include "openDDL_tokens.h"

/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
namespace octet
{
  namespace loaders{
    class openGEX_lexer : ddl_token{
      // Dictionary of identifiers
      dictionary<int> identifiers_;

      // This is the current buffer with the characters in UTF8
      dynarray<uint8_t> buffer;
      // The number of characters till the end of the file
      int bufferSize;
      // The size of the token
      int sizeRead;


      // Some small functions to make easier the testing
            /// @brief  This function returns true if the pointer is at the end of the file
      inline bool is_end_file(){
        return bufferSize == 0;
      }
            /// @brief  This function will get a new token position
      inline void get_next_char(){
        ++currentChar;
        --bufferSize;
      }
            /// @brief Jumps the char "pos" positions
      inline void char_jump(int pos){
        currentChar += pos;
        bufferSize -= pos;
      }
            /// @brief This will check if the current char is equal the string
            /// This will check the whole string, if it matches with the begining of currentChar
      inline bool char_word_is(string word){
        sizeRead = 0;
        while ((sizeRead < word.size()) && (currentChar[sizeRead] == (uint8_t)word[sizeRead]))
          ++sizeRead;
        return sizeRead == word.size();
      }

      /// @brief  This will lexer a comment with /* */or // and break line
      void lex_comment(){
        get_next_char();
        switch (currentChar[0]){
        case 0x2F: // 0x2f = /  as it comes from another / that means that it's a // comment
          while (currentChar[0] != 0x0D)
            get_next_char();
          break;
        case 0x2A: // 0x2a = * as it comes from another / that means that it's a /* comment
          get_next_char();
          while (currentChar[0] != 0x2A && currentChar[1] != 0x2f){ // until next characters are * and / 
            get_next_char();
            get_next_char();
          }
        }
      }

      /// @brief  This function will process the currentChar to look for the next token and study it
      /// openDDL is divided into structures
      void process_structure(){
        //printf("%x ", currentChar[0]);
        // first thing is to check if it's a comment
        if (is_comment(currentChar)) 
          lex_comment(); //if it's a comment analyze it (that means, ignore it)
        else if (is_dataType()){

        }
        else if (is_identifier()){

        }else //if it's nothing of the above is an error
          printf("ERROR!");
      }

    public:
      /// @brief Constructor of lexer
      openGEX_lexer(){}

      /// @brief This will be the function that creates de process of the lexer receiving as parameter the array of characters
      bool lexer_file(dynarray<uint8_t> file){
        sizeRead = 0;
        buffer = file;
        currentChar = &buffer[0];
        bufferSize = buffer.size();
        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file()){
          //Process token (in openDDL is a structure) when you find it (when it's not a whiteSpace)
          if(!is_whiteSpace(currentChar[0])) process_structure();
          //get new token
          get_next_char();
        }

        return true;
      }
    };
  }
}