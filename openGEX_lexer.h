////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////

#include "openDLL_tokens.h"

/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
namespace octet
{
  namespace loaders{
    class openGEX_lexer : ddl_token{
      // Dictionary of identifiers
      dictionary<int> identifiers_;

      // This is the current buffer with the characters in UTF8
      dynarray<uint8_t> buffer;
      // This are the current character and the next character after the token
      uint8_t * currentChar;
      uint8_t * tempChar;
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

      /*
      /// @brief  This will lexer the word "Metric" and will decide the next lexer to analize
      void lex_Metric(){
        // check if it's distance, angle, time or up
        switch (currentChar[0]){
        case 0x64: // 0x64 = d  this is suppose to start a distance Metric structure
          if (char_word_is("distance\") {")){
            printf("distance!\n");
            char_jump(sizeRead);
            lex_dataType();
            // There must be something in here to read the { or other simbols
            lex_literal();
          }
          break;
        case 0x61: // 0x61 = a this is suppose to start a angle Metric structure
          if (char_word_is("angle\") {")){
            printf("angle!\n");
            char_jump(sizeRead);
            lex_dataType();
            // There must be something in here to read the { or other simbols
            lex_literal();
          }
          break;
        case 0x74: // 0x74= t this is suppose to start a time Metric structure
          if (char_word_is("time\") {")){
            printf("time!\n");
            char_jump(sizeRead);
            lex_dataType();
            // There must be something in here to read the { or other simbols
            lex_literal();
          }
          break;
        case 0x75: // 0x75 = u this is suppose to start a up Metric structure
          if (char_word_is("up\") {")){
            printf("up!\n");
            char_jump(sizeRead);
            lex_dataType();
            // There must be something in here to read the { or other simbols
            lex_literal();
          }
          break;
        default: // ERROR!!!!!
          printf(">>>¡¡¡ERROR!!!<<<");
          break;
        }
      }
      */

      /// @brief  This function will test if the current token is going to be a comment (// or /*)
      bool is_comment(){ //0x2f = /  and  0x2A = *
        return currentChar[0] == 0x2F && (currentChar[1] == 0x2f || currentChar[1] == 0x2A); 
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

      /// @brief  This function will test if the current token is a dataType
      bool is_dataType(){

      }

      /// @brief  This function will test if the current token is a identifier
      bool is_identifier(){

      }

      /// @brief  This function will process the currentChar to look for the next token and study it
      /// openDDL is divided into structures
      void process_structure(){
        //printf("%x ", currentChar[0]);
        // first thing is to check if it's a comment
        if (is_comment()) 
          lex_comment(); //if it's a comment analyze it
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