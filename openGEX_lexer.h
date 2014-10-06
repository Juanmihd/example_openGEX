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
      // This are the current character and the next character after the token
      uint8_t * currentChar;
      uint8_t * nextChar;
      // The number of characters till the end of the file
      int bufferSize;
      // The size of the token
      int tokenSize;

      ///This function returns true if the pointer is at the end of the file
      bool is_end_file(){
        return bufferSize == 0;
      }

      /// This function will get a new token position
      void get_next_char(){
        ++currentChar;
        --bufferSize;
      }

      /// This will lexer a comment with /* */or // and break line
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

      /// This function will process the currentChar to look for the next token and study it
      void process_token(){
        //THIS IS JUST A TEST FOR NOW. I just want to be sure that this is working
        printf("%x ", currentChar[0]);
        switch (currentChar[0]){
        case 0x2F: // 0x2f = /  this is suppose to start a comment
          lex_comment();
          break;
        default:
          printf("ECCO!\n");
        }
      }

    public:
      openGEX_lexer(){}

      ///This will be the function that creates de process of the lexer receiving as parameter the array of characters
      void lexer_file(dynarray<uint8_t> file){
        tokenSize = 0;
        buffer = file;
        nextChar = currentChar = &buffer[0];
        bufferSize = buffer.size();
        printf("The size of the buffer is %i\n", buffer.size());
        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file()){ //It will test if it's the last token
          //Process token
          process_token();
          //get new token
          get_next_char();
        }
      }
    };
  }
}