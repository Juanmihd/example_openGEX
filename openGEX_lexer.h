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
      /// url file
      /// int8 pointer to the text

    public:
       openGEX_lexer();

       void loadFile();
    };
  }
}