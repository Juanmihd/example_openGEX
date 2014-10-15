////////////////////////////////////////////////////////////////////////////////
/// @file openDDL_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
////////////////////////////////////////////////////////////////////////////////

#include "openDDL_tokens.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
////////////////////////////////////////////////////////////////////////////////
namespace octet
{
  namespace loaders{
    class openDDL_lexer : ddl_token{
    protected:
      // Dictionary of identifiers of the openDDL language we are using
      dictionary<int> identifiers_;
      // Dictionary of types of openDDL
      dictionary<int> types_;
      // Dictionary of names
      dictionary<int> names_;
      // Dictionary of symbols
      dictionary<int> symbols_;
      // Dictionary of references
      dictionary<int> references_;
      // This are the current character and the next character after the token
      uint8_t * currentChar;
      uint8_t * tempChar;
      // This is the current buffer with the characters in UTF8
      dynarray<uint8_t> buffer;
      // The number of characters till the end of the file
      int bufferSize;
      // The size of the token
      int sizeRead;

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will add an identifiers to the dictionary
////////////////////////////////////////////////////////////////////////////////
      void add_identifier(const char *id, int tok){
        identifiers_[id] = tok;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will add an types to the dictionary
////////////////////////////////////////////////////////////////////////////////
      void add_type(const char *id, int tok){
        types_[id] = tok;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will add an symbols to the dictionary
////////////////////////////////////////////////////////////////////////////////
      void add_symbol(const char *id, int tok){
        symbols_[id] = tok;
      }

      // Some small functions to make easier the testing
////////////////////////////////////////////////////////////////////////////////
/// @brief  This function returns true if the pointer is at the end of the file
////////////////////////////////////////////////////////////////////////////////
      bool is_end_file(){
        return bufferSize <= 0;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will get a new char
      ////////////////////////////////////////////////////////////////////////////////
      void get_next_char(){
        --bufferSize;
        ++currentChar;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will get the previous char
////////////////////////////////////////////////////////////////////////////////
      void get_previous_char(){
        ++bufferSize;
        --currentChar;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief Jumps the char "pos" positions
////////////////////////////////////////////////////////////////////////////////
      void char_jump(int pos){
        bufferSize -= pos;
        currentChar += pos;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief This will check if the current char is equal the string
/// This will check the whole string, if it matches with the begining of currentChar
////////////////////////////////////////////////////////////////////////////////
      bool char_word_is(string word){
        sizeRead = 0;
        while ((sizeRead < word.size()) && (currentChar[sizeRead] == (uint8_t)word[sizeRead]))
          ++sizeRead;
        return sizeRead == word.size();
      }
      
////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will test if the current token is a whitespace (less than 0x20 character)
////////////////////////////////////////////////////////////////////////////////
      bool is_whiteSpace(){ // everything less or equal than 0x20 is a whitespace
        return currentChar[0] <= 0x20;
      }
      
////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will test if the current token is a whitespace (less than 0x20 character)
////////////////////////////////////////////////////////////////////////////////
      void remove_comments_whitespaces(){ // everything less or equal than 0x20 is a whitespace
        while (is_comment() || is_whiteSpace()){
          if (is_comment()) ignore_comment();
          else get_next_char();
        }
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will test if the current token is going to be a comment (// or /*)
////////////////////////////////////////////////////////////////////////////////
      bool is_comment(){ //0x2f = /  and  0x2A = *
        return currentChar[0] == 0x2F && (currentChar[1] == 0x2F || currentChar[1] == 0x2A);
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will test if the current token is a dataType (will return the index of the type
////////////////////////////////////////////////////////////////////////////////
      int is_dataType(string word){
        int index = types_.get_index(word.c_str());
        return index;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will test if the current token is a identifier
////////////////////////////////////////////////////////////////////////////////
      int is_identifier(string word){
        int index = identifiers_.get_index(word.c_str());
        return index;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will test if the current token is a name
////////////////////////////////////////////////////////////////////////////////
      bool is_name(){
        return (currentChar[0] == '%') || (currentChar[0] == '$');
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will test if the current character is a symbol { } [ ] ( ) , =
////////////////////////////////////////////////////////////////////////////////
      bool is_symbol(){
        string character ((char*)currentChar,1);
        return symbols_.contains(character.c_str());
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a bool-literal and return it's value (will check if there is any problem)
////////////////////////////////////////////////////////////////////////////////
      bool is_bool_literal(bool *value, string *word){

      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a int-literal
////////////////////////////////////////////////////////////////////////////////
      bool get_integer_literal(int *value, string *word){

      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a float-literal
////////////////////////////////////////////////////////////////////////////////
      bool get_float_literal(float *value, string *word){

      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a string-literal
////////////////////////////////////////////////////////////////////////////////
      bool get_string_literal(string *value, string *word){

      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a reference
////////////////////////////////////////////////////////////////////////////////
      bool get_reference(string *value, string *word){

      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a type
////////////////////////////////////////////////////////////////////////////////
      bool get_data_type(string *value, string *word){

      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This functions process the name, and add it to the application
////////////////////////////////////////////////////////////////////////////////
      void process_name(){
        string name = read_word();
        printf("It's the name %s !!\n", name);
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This functions process the properties, and add it to the application
////////////////////////////////////////////////////////////////////////////////
      bool process_properties(){
        printf("\tProperties!\n");
        get_next_char();
        remove_comments_whitespaces();
        while (currentChar[0] != 0x29){
          //printf("%x, ", currentChar[0]);
          get_next_char();
          if (is_end_file()){
            return false;
            printf("\n\nERROR!!!\n\n");
          }
        }
        get_next_char();
        //printf("\n End Properties!\n");
        return true;
      }
      
////////////////////////////////////////////////////////////////////////////////
/// @brief  This function has to process the datalist
///  This function will keep on checking data until it finds a }. 
/// @return true if it went ok and false if there was any problem (for instance not finding a })
////////////////////////////////////////////////////////////////////////////////
      bool process_data_list(int type){
        int ending;
        string *word = new string();
        ending = read_data_list_element(word);
        printf("Reading -> %d\n", ending);
        while (ending == 1){
          get_next_char();
          ending = read_data_list_element(word);
          printf("Reading -> %d\n", ending);
        }
        return ending >= 0;
      }
      
////////////////////////////////////////////////////////////////////////////////
/// @brief  This function has to process the dataarraylist
///  This function will keep on checking data until it finds a }. 
/// @return true if it went ok and false if there was any problem (for instance not finding a })
////////////////////////////////////////////////////////////////////////////////
      bool process_data_array(int type, int arraySize){
        //detect {
        printf("Checking1 %x\n", currentChar[0]);
        if (currentChar[0] != 0x7b) //7b = {
          return false;
        remove_comments_whitespaces();
        //read elements
        int itemsLeft = arraySize;
        while (itemsLeft > 0){
          printf("I have %i items left\n", itemsLeft);
          while (currentChar[0] != 0x2c || currentChar[0] != 0x7d){ // 2c = ,
            get_next_char();
            printf("Checking DENTRO %x\n", currentChar[0]);
          }
          get_next_char();

          --itemsLeft;
        }
        get_next_char();
        //detect }
        remove_comments_whitespaces();
        printf("Checking2 %x\n", currentChar[0]);
        if (currentChar[0] != 0x7d) //7d = }
          return false;
        remove_comments_whitespaces();
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function has to process the datalist
///  This function will keep on checking data until it finds a }. 
/// @return true if it went ok and false if there was any problem (for instance not finding a })
////////////////////////////////////////////////////////////////////////////////
      bool process_data_array_list(int type, int arraySize){
        bool no_error = false;
        while (no_error && currentChar[0] != 0x7d){
          printf("Checking %x\n", currentChar[0]);
          no_error = process_data_array(type, arraySize); //This will have to start with {, read arraySize elements, read }
        }
        //expect } (7d)
        get_next_char();
        if (currentChar[0] != 0x7d) no_error = false;
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This will lexer a comment with /* */or // and break line
////////////////////////////////////////////////////////////////////////////////
      void ignore_comment(){
        get_next_char();

        switch (currentChar[0]){
        case 0x2F: // 0x2f = /  as it comes from another / that means that it's a // comment
          while (currentChar[0] != 0x0D && currentChar[0] != 0x0A)
            get_next_char();
          break;
        case 0x2A: // 0x2a = * as it comes from another / that means that it's a /* comment
          get_next_char();
          while (currentChar[0] != 0x2A && currentChar[1] != 0x2f){ // until next characters are * and / 
            get_next_char();
            if (is_end_file()) assert(0 && "It's missing the */ of a comment");
          }
          get_next_char();
          break;
        }
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will read a data-list element and will return if error or more elements
/// @param word Pointer to the word, this is the word read
/// @return -1 if error
/// @return  0 if last element
/// @return  1 if more elements
////////////////////////////////////////////////////////////////////////////////
      int read_data_list_element(string *word){
        int sizeWord = 0, to_return;
        tempChar = currentChar;
        while (currentChar[0] != 0x2c && currentChar[0] != 0x7d && !is_whiteSpace() && !is_comment()){
          printf("%x, ", currentChar[0]);
          get_next_char();
          ++sizeWord;
        }

        remove_comments_whitespaces();

        switch (currentChar[0]){
        case 0x2c: //2c = ,
          to_return = 1;
          break;
        case 0x7d: //7d = }
          to_return = 0;
          break;
        default:
         to_return = -1;
          break;
        }
        word->set((char*)(tempChar), sizeWord);
        return to_return;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will read and return an integer
      ////////////////////////////////////////////////////////////////////////////////
      int read_array_size(){
        int sizeNumber = 0, number = 0, pow = 1;
        tempChar = currentChar;
        while (currentChar[0] != 0x5d){ // 5d = ]
          get_next_char();
          ++sizeNumber;
        }
        printf("\n");
        --sizeNumber;
        for (int i = 0; i <= sizeNumber; ++i){
          number += pow*((int)tempChar[sizeNumber-i]-48);
          pow *= 10;
        }
        return number;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief It will read and return a word
      /// It will start geting characters and return them as a word. It will stop when it finds the begining a comment or a whitespace
      ////////////////////////////////////////////////////////////////////////////////
      string read_word(){
        int sizeWord = 0;
        tempChar = currentChar;
        if (is_symbol())
          sizeWord = 1;
        else{
          while (!is_symbol() && !is_whiteSpace() && !is_comment()){
            get_next_char();
            ++sizeWord;
          }
          if (is_symbol()) get_previous_char();
          if (is_comment()){
            ignore_comment();
            get_next_char();
          }
        }
        string temp((char*)(tempChar), sizeWord);
        //printf("Last symbol-> %x finding %s\n", currentChar[0], temp);
        //printf("Finding => %s\n", temp);
        return temp;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will initialize the dictionaries of the lexer
      ////////////////////////////////////////////////////////////////////////////////
      void init_ddl(){
        // Adding the types to the list of types
        for (int i = first_type(); i < first_symbol(); ++i)
          add_type(token_name(i).c_str(),i);
        for (int i = first_symbol(); i <= last_symbol(); ++i)
          add_symbol(token_name(i).c_str(), i);
      }
    public:
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Constructor of lexer
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_lexer(){}

    };
  }
}