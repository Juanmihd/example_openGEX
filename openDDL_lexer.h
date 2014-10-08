////////////////////////////////////////////////////////////////////////////////
/// @file openDDL_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////

#include "openDDL_tokens.h"

/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
namespace octet
{
  namespace loaders{
    class openDDL_lexer : ddl_token{
    protected:
      // Dictionary of identifiers of openGEX
      dictionary<int> identifiers_;
      // Dictionary of names
      dictionary<int> names_;
      // Dictionary of references
      dictionary<int> references_;
      // This are the current character and the next character after the token
      uint8_t * currentChar;
      uint8_t * tempChar;
      // This is the current buffer with the characters in UTF8
      dynarray<uint8_t> buffer;
      // The number of characters till the end of the file
      int bufferSize;
      // The size of the token
      int sizeRead;

      // Some small functions to make easier the testing
      /// @brief  This function returns true if the pointer is at the end of the file
      bool is_end_file(){
        return bufferSize == 0;
      }

      /// @brief  This function will get a new token position
      void get_next_char(){
        ++currentChar;
        --bufferSize;
      }

      /// @brief Jumps the char "pos" positions
      inline void char_jump(int pos){
        currentChar += pos;
        assert(((bufferSize -= pos) < 0) && "The end of the file arrived before it was expected.");

      }

      /// @brief This will check if the current char is equal the string
      /// This will check the whole string, if it matches with the begining of currentChar
      bool char_word_is(string word){
        sizeRead = 0;
        while ((sizeRead < word.size()) && (currentChar[sizeRead] == (uint8_t)word[sizeRead]))
          ++sizeRead;
        return sizeRead == word.size();
      }

      /// @brief  This function will test if the current token is a whitespace (less than 0x20 character)
      bool is_whiteSpace(){ // everything less or equal than 0x20 is a whitespace
        return currentChar[0] <= 0x20;
      }

      /// @brief  This function will test if the current token is going to be a comment (// or /*)
      bool is_comment(){ //0x2f = /  and  0x2A = *
        return currentChar[0] == 0x2F && (currentChar[1] == 0x2f || currentChar[1] == 0x2A);
      }

      /// @brief  This function will test if the current token is a dataType
      bool is_dataType(){
        return true;
      }

      /// @brief  This function will test if the current token is a identifier
      bool is_identifier(){
        return true;
      }

      /// @brief  This will lexer a comment with /* */or // and break line
      void ignore_comment(){
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
            assert(!is_end_file() && "Some code is missing here! Text finished before */");
            get_next_char();
            assert(!is_end_file() && "Some code is missing here! Text finished before */");
          }
        }
      }

      /// @brief  This function will process the currentChar to look for the next token and study it
      /// openDDL is divided into structures
      void process_structure(){
        //printf("%x ", currentChar[0]);
        // first thing is to check if it's a comment
        if (is_comment()) ignore_comment(); //if it's a comment analyze it (that means, ignore it)
        else if (is_dataType()){
         // process_structureData();
        }
        else if (is_identifier()){
          // process_structureData();
        }
        else //if it's nothing of the above is an error
          assert(0 && "It's not a proper structure");
      }

    public:
      /// @brief Constructor of lexer
      openDDL_lexer(){}

    };
  }
}