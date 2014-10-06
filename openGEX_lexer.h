////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////


/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
namespace octet
{
  namespace loaders{
    class openGEX_lexer {
      // This is the current buffer with the characters in UTF8
      dynarray<uint8_t> buffer;
      // This are the current character and the last character to test if it's the end of the file
      uint8_t * currentChar;
      uint8_t * lastChar;
      ///This function returns true if the pointer is at the end of the file
      bool is_end_file(){
        return currentChar == lastChar;
      }

      /// This function will process the currentChar to look for the next token and study it
      void process_token(){
        //THIS IS JUST A TEST FOR NOW. I just want to be sure that this is working
        printf("%s ", currentChar);
      }

    public:
      openGEX_lexer(){}

      ///This will be the function that creates de process of the lexer receiving as parameter the array of characters
      void lexer_file(){
        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file()){ //It will test if it's the last token
          //Process token
          process_token();
          //get new token
          ++currentChar;
        }
      }
    };
  }
}