////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_loader.h
/// @author Juanmi Huertas Delgado
/// @brief This is the loader to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////


/// @brief This class is the openGEX loader, it will open the file and call the lexer and so
namespace octet
{
  namespace openGEX{
    class openGEX_loader {
      /// url file
      /// int8 pointer to the text

    public:
      openGEX_loader();

      void loadFile();
    };
  }
}