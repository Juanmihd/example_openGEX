////////////////////////////////////////////////////////////////////////////////
/// @file openGEX_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
/// NOTE: In the compiler several functions will have the boolean "no_error"
///
////////////////////////////////////////////////////////////////////////////////

#include "openDDL_lexer.h"
#include "openGEX_identifiers.h"
////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
////////////////////////////////////////////////////////////////////////////////
namespace octet
{
  namespace loaders{
    class openGEX_lexer : openDDL_lexer{
      typedef gex_ident::gex_ident_enum gex_ident_list;

////////////////////////////////////////////////////////////////////////////////
/// @brief This will initialize some structures of the lexer (dictionary of identifiers of openGEX)
////////////////////////////////////////////////////////////////////////////////
      void init_gex(){
        gex_ident identList;
        //Load the identifiers of openGEX in identifeirs_ dictionary
        for (int i = gex_ident_list::id_Animation; i != gex_ident_list::ident_last; ++i)
          add_identifier(identList.ident_name(i).c_str(),i);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will lexer the next words, considering to be a "structureData"
      ////////////////////////////////////////////////////////////////////////////////
      bool process_structureData(int type){
        bool no_error = true;
        int arraySize;
        printf("\t----Is a Type n: %i!!----\n", type);
        printf("%x, ", currentChar[0]);
        //First step is remove whiteSpace and comments
        remove_comments_whitespaces();
        printf("%x\n", currentChar[0]);
        //Then it will read the first character, to see if its a [, or {, or name
        //if name it is a only dataList, so call to process_dataList() and tell that function if has a name or not
        if (currentChar[0] == 0x5b){ // 5b = [
          printf("It's a data array list!\n");
          //check integer-literal (for a data array list)
          get_next_char();
          arraySize = read_array_size();
          get_next_char();

          remove_comments_whitespaces();

          //it may receive a name (optional)
          if (is_name()){
            process_name();
            get_next_char();
          }

          remove_comments_whitespaces();

          //expect a { (if not, error)
          if (currentChar[0] == 0x7b) //7b = {
            no_error = process_data_array_list(type, arraySize);
          else{ //call to process data array list, it will check the }
            no_error = false; //return error
            printf("\n\nERROR: I don't find the data-array-list!\n\n");
          }
        } //ending the [integer-literal] (name) { data-array-list* } option
        
        //now check the other option (name) { data-list* }
        else{
          if (is_name()){ // check if there is a name, and process it
            printf("It's a name + data list!\n");
            process_name();
            get_next_char();
          }

          //After the optional name, it expects a {, and analize the data_list
          if (currentChar[0] == 0x7b){ // 7b = {
            printf("It's a data list!\n");
            no_error = process_data_list(type);  //expect a } (if not, error)
          }
          else{ //if there is no {, ITS AN ERROR!!!
            no_error = false;
            printf("\n\nERROR: I don't find the data-list!\n\n");
          }
        }

        return no_error;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will lexer the next words, considering to be a "structureIdentifier"
////////////////////////////////////////////////////////////////////////////////
      bool process_structureIdentifier(int type){
        bool no_error = true;
        printf("\t----Is the identifier n. %i!!----\n", type);

        //First step is remove whiteSpace and comments
        remove_comments_whitespaces();

        //Then it will read the first character, to see if its a (, or name, or {

        //If its a name call to something to process name
        if (is_name()) process_name();
        //Later, check if it's ( and call something to check properties - telling the function which structure is this one

        if (currentChar[0] == 0x28){ // 28 = (
          //call something to check properties          //expect a ) (if not, error)
          no_error = process_properties();
        }

        remove_comments_whitespaces();
        printf("%x\n", currentChar[0]);
        //Later expect a {, if not return error, and check for a new structure inside this structure
        if (currentChar[0] == 0x7b){ //7b = {
          printf("\tSubstructure!\n");
          get_next_char();
          remove_comments_whitespaces();
          no_error = process_structure();  //call to process structure
          //Later expect a }, if not return error
          get_next_char();
          remove_comments_whitespaces();
          if (currentChar[0] != 0x7d){ //7d = }
            no_error = false;
            printf("\n\nERROR no ending of the structure!!!\n\n");
          }
          get_next_char();
        }
        else{
          no_error = false;
          printf("\nERROR: No substructure!!!\n\n");
        }
        
        return no_error;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will process the currentChar to look for the next token and study it
/// openDDL is divided into structures
////////////////////////////////////////////////////////////////////////////////
      bool process_structure(){
        bool no_error = true;
        string word;

        // It's a real structure! But it can be IDENTIFIER or DATATYPE
        
        printf("Structure Started!\n");
        word = read_word();
        printf("Finding => %s\n", word);
        remove_comments_whitespaces();
        printf("%x <----\n", currentChar[0]);
        //check if it's a type and return it's index (if its negative it's not a type)
        int type = is_dataType(word); 
        if (type >= 0){ //As it's a Data type, now it can be single data list or data array list!
          process_structureData(types_.get_value(type)); 
        }

        else{
          //check if it's a identifier and return it's index (if its negative it's not a identifier)
          type = is_identifier(word);
          if (type >= 0){ //As it's a Identifier type, now check name? properties? and then { structure(s)? }
            process_structureIdentifier(identifiers_.get_value(type)); 
          }

          else //if it's nothing of the above is an error
            ;//assert(0 && "It's not a proper structure");
          printf("Structure ended!\n\n");
        }
        return no_error;
      }

    public:
////////////////////////////////////////////////////////////////////////////////
/// @brief Constructor of lexer
////////////////////////////////////////////////////////////////////////////////
      openGEX_lexer(){
        init_ddl();
        init_gex();
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief This will be the function that creates de process of the lexer receiving as parameter the array of characters
////////////////////////////////////////////////////////////////////////////////
      bool lexer_file(dynarray<uint8_t> file){
        bool no_error = true;
        sizeRead = 0;
        buffer = file;
        currentChar = &buffer[0];
        bufferSize = buffer.size();

        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file() && no_error){
          remove_comments_whitespaces();
          //Process token (in openDDL is a structure) when you find it (when it's not a whiteSpace)
          no_error = process_structure();
          //get new token
          get_next_char();
        }

        return no_error;
      }
    };
  }
}