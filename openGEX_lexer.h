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
      uint8_t * tempChar;
      // The number of characters till the end of the file
      int bufferSize;
      // The size of the token
      int tokenSize;

      /// @brief  This function returns true if the pointer is at the end of the file
      bool is_end_file(){
        return bufferSize == 0;
      }

      /// @brief  This function will get a new token position
      void get_next_char(){
        ++currentChar;
        --bufferSize;
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

      /// @brief This will check the current position of char, to see if it's a Metric structure and it 
      /// it will return true if the next chars are "etric (key ="" (it has to come from a 'M')
      bool is_Metric(){  // currentChar[0] is already M
        return (currentChar[1]  == 0x65 &&   // e
                currentChar[2]  == 0x74 &&   // t
                currentChar[3]  == 0x72 &&   // r
                currentChar[4]  == 0x69 &&   // i
                currentChar[5]  == 0x63 &&   // c
                currentChar[6]  == 0x20 &&   // SP
                currentChar[7]  == 0x28 &&   // (
                currentChar[8]  == 0x6B &&   // k
                currentChar[9]  == 0x65 &&   // e
                currentChar[10] == 0x79 &&   // y
                currentChar[11] == 0x20 &&   // SP
                currentChar[12] == 0x3D &&   // =
                currentChar[13] == 0x20 &&   // SP
                currentChar[14] == 0x22);    // "
      }

      /// @brief  This will lexer the word "Metric" and will decide the next lexer to analize
      void lex_Metric(){
        //get_next_char();
        printf("Works!\n");
      }

      /// @brief  This function will process the currentChar to look for the next token and study it
      void process_token(){
        //THIS IS JUST A TEST FOR NOW. I just want to be sure that this is working
        //printf("%x < %x", currentChar[0], 0x20);
        //printf("%x ", currentChar[0]);
        //printf((currentChar[0] < 0x20) ? "true" : "false");
        if (currentChar[0] > 0x20){ // less than 0x20 it's a whitespace so, ignore it
          switch (currentChar[0]){
          case 0x2F: // 0x2f = /  this is suppose to start a comment
            lex_comment();
            break;
          case 0x4D: // 0x4d = M  this might be for a Metric or other thing
            if (is_Metric()){
              lex_Metric();  //if is Metric, keep on, if not, it's only an 'M' so next case
              break;
            }
          default:
            //printf("%x", currentChar[0]);
            break;
          }
        }
      }

    public:
      /// @brief Constructor of lexer
      openGEX_lexer(){}

      /// @brief This will be the function that creates de process of the lexer receiving as parameter the array of characters
      void lexer_file(dynarray<uint8_t> file){
        tokenSize = 0;
        buffer = file;
        currentChar = &buffer[0];
        bufferSize = buffer.size();
        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file()){
          //Process token
          process_token();
          //get new token
          get_next_char();
        }
      }
    };
  }
}