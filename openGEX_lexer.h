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
      void init_gex(){
        gex_ident identList;
        //Load the identifiers of openGEX in identifeirs_ dictionary
        for (int i = gex_ident_list::id_Animation; i != gex_ident_list::ident_last; ++i)
          add_identifier(identList.ident_name(i).c_str(),i);
      }

      /// @brief  This function will lexer the next words, considering to be a "structureData"
      void process_structureData(int type){
        int arraySize;
        printf("\t----Is a Type n: %i!!----\n", type);
        //First step is remove whiteSpace and comments
        while (is_comment() || is_whiteSpace()){
          if (is_comment()) ignore_comment();
          else get_next_char();
        }
        //Then it will read the first character, to see if its a [, or {, or name
        //if name it is a only dataList, so call to process_dataList() and tell that function if has a name or not
        if (is_name()){
          printf("It's a name + data list!\n");
          process_name();
          get_next_char();
          //expect a { (if not, error)
          if (currentChar[0] != 0x7b) //7b = {
            ; //return error
          else{
            ;// process_data_list(); //expect a } (if not, error)
          }
        }
        else{
          printf("It's not a name\n");
          //get_next_char();
          switch (currentChar[0]){
          // {  it is a only dataList, so call to process_dataList() and tell that function if has a name or not
          case 0x7b: // 7b = {
            get_next_char();
            printf("It's a data list!\n");
            process_data_list(type); //expect a } (if not, error)
            break;
            // [ it will be a "array"
          case 0x5b: // 5b = [
            printf("It's an data array list!\n");
            //check integer-literal (for a data array list)
            arraySize = read_array_size();
            get_next_char();
            //it may receive a name (optional)
            if (is_name()){
              process_name();
              get_next_char();
            }
            //expect a { (if not, error)
            if (currentChar[0] != 0x7b) //7b = {
              ; //return error
            else{
              ;//call to process data array list

              //expect a } (if not, error)
              get_next_char();
              if (currentChar[0] != 0x7d) //7d = }
                ; //return error
              else{
                ;//call to process structure
              }
            }
            break;
          default:
            printf("Nothin! %x\n",currentChar[0]);
            break;
          }
        }
        //Else it's an error
      }

      /// @brief  This function will lexer the next words, considering to be a "structureIdentifier"
      void process_structureIdentifier(){
        printf("\t----Is a identifier!!----\n");
        //First step is remove whiteSpace and comments
        while (is_comment() || is_whiteSpace()){
          if (is_comment()) ignore_comment();
          else get_next_char();
        }
        //Then it will read the first character, to see if its a (, or name, or {
        //If its a name call to something to process name
        if (is_name()) process_name();
        //Later, check if it's ( and call something to check properties - telling the function which structure is this one
        if (currentChar[0] == 0x28) // 28 = (
          //call something to check properties
          //expect a ) (if not, error)
          ;
        //Later expect a {, if not return error, and check for a new structure inside this structure
        if (currentChar[0] != 0x7b) //7b = {
          ; //return error
        else{
          ;//call to process structure
          //Later expect a }, if not return error
          get_next_char();
          if (currentChar[0] != 0x7d) //7d = }
            ; //return error
          else{
            ;//call to process structure
          }
        }
        
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
          //printf("Structure Started!\n");
          word = read_word();
          printf("%s\n", word);
          int type = is_dataType(word); //check if it's a type and return it's index (if its negative it's not a type)
          if (type >= 0){
            process_structureData(types_.get_value(type)); //As it's a Data type, now it can be single data list or data array list!
          }
          else if (is_identifier(word)){
            process_structureIdentifier(); //As it's a Identifier type, now check name? properties? and then { structure(s)? }
          }
          else //if it's nothing of the above is an error
            ;//assert(0 && "It's not a proper structure");
          //printf("Structure ended!\n\n");
        }
        return true;
      }

    public:
      /// @brief Constructor of lexer
      openGEX_lexer(){
        init_ddl();
        init_gex();
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