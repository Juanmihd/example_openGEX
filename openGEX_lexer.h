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
      int bufferSize;
      ///This function returns true if the pointer is at the end of the file
      bool is_end_file(){
        return bufferSize == 0;
      }

      /// This function will get a new token position
      void get_next_token(){
        ++currentChar;
        --bufferSize;
      }


      /// This function will process the currentChar to look for the next token and study it
      void process_token(){
        //THIS IS JUST A TEST FOR NOW. I just want to be sure that this is working
        printf("%x ", currentChar[0]);
      }

    public:
      openGEX_lexer(){}

      ///This will be the function that creates de process of the lexer receiving as parameter the array of characters
      void lexer_file(dynarray<uint8_t> file){
        buffer = file;
        currentChar = &buffer[0];
        bufferSize = buffer.size();
        printf("The size of the buffer is %i\n", buffer.size());
        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file()){ //It will test if it's the last token
          //Process token
          process_token();
          //get new token
          get_next_token();
        }
      }
    };
  }
}