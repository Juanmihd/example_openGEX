////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////

#include "openDDL_lexer.h"
#include "openGEX_identifiers.h"

/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
namespace octet
{
  namespace loaders{
    class openGEX_lexer : openDDL_lexer{
      typedef gex_ident::gex_ident_enum gex_ident_list;

      /// @brief This will initialize some structures of the lexer (dictionary of identifiers of openGEX)
      void init(){
        gex_ident identList;
        //Load the identifiers of openGEX in identifeirs_ dictionary
        for (int i = gex_ident_list::id_Animation; i != gex_ident_list::ident_last; ++i)
          add_identifier(identList.ident_name(i).c_str(),i);
      }

    public:
      /// @brief Constructor of lexer
      openGEX_lexer(){
        init();
      }

      /// @brief  This function will process the currentChar to look for the next token and study it
      /// openDDL is divided into structures
      bool process_structure(){
        string word;
        //printf("%x ", currentChar[0]);
        // first thing is to check if it's a comment
        if (is_comment()) ignore_comment(); //if it's a comment analyze it (that means, ignore it)
        else if (is_whiteSpace()); // check if it's a whitespace, do nothing (that's why there is a ;)
        else{ // It's a real structure! But it can be IDENTIFIER or DATATYPE
          printf("Structure Started!\n");
          word = read_word();
          printf("%s\n", word);
          //read word
          if (is_dataType()){
            // process_structureData(); //As it's a Data type, now it can be single data list or data array list!
          }
          else if (is_identifier(word)){
            // process_structureData(); //As it's a Identifier type, now check name? properties? and then { structure(s)? }
            printf("\t----Is a identifier!!----\n");
          }
          else //if it's nothing of the above is an error
            ;//assert(0 && "It's not a proper structure");
          printf("Structure ended!\n\n");
        }
        return true;
      }

      /// @brief This will be the function that creates de process of the lexer receiving as parameter the array of characters
      bool lexer_file(dynarray<uint8_t> file){
        sizeRead = 0;
        buffer = file;
        currentChar = &buffer[0];
        bufferSize = buffer.size();
        bool keep_on = true;
        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file() && keep_on){
          //Process token (in openDDL is a structure) when you find it (when it's not a whiteSpace)
          keep_on = process_structure();
          //get new token
          get_next_char();
        }

        return true;
      }
    };
  }
}