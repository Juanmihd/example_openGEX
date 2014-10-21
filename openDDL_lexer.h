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
      bool get_bool_literal(bool &value, string *word){
        if (word->size() == 4){ //true.size() == 4
          if (word->find("true"))
            value = true;
          else
            return false;
        }
        else if (word->size() == 5){ //false.size() == 5
          if (word->find("false"))
            value = false;
          else
            return false;
        }
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a int-literal
////////////////////////////////////////////////////////////////////////////////
      bool get_integer_literal(int &value, string *word){
        value = 0;
        int pow = 10;
        int initial_i = 0;
        int pos_negative = 1;

        //It may have - or +
        if (word->c_str()[0] == 0x2b){ //2b = +
          initial_i = 1; //ignore the first character
          pos_negative = 1; //is positive number
        }
        else if (word->c_str()[0] == 0x2d){ //2d = -
          initial_i = 1; //ignore the first character
          pos_negative = -1;
        }


        //It may be a binary-literal, hex-literal or float-literal (starting with 0 or .) or float-literal starting with any number
        if (word->c_str()[initial_i] == 0x30){ //30 = 0
          if (word->c_str()[initial_i + 1] == 0x42 || word->c_str()[initial_i + 1] == 0x62){ //42 = B, 62 = b, that meaning, it's a binary number
            pow = 2;
            initial_i += 2;
            //fill this with the reader of binaries
            for (int i = 0; i < word->size(); ++i){
              if (word->c_str()[i] == 48 || word->c_str()[i] == 49)
                value = value*pow + (word->c_str()[i] - 48);
              else
                return false; //ERROR!
            }
          }

          else if (word->c_str()[initial_i + 1] == 0x58 || word->c_str()[initial_i + 1] == 0x78){ //58 = X, 78 = x, that meaning, it's an hex number
            pow = 16;
            initial_i += 2;
            //fill this with the reader of exadecimals
            for (int i = 0; i < word->size(); ++i){
              if (word->c_str()[i] >= 48 || word->c_str()[i] <= 57)
                value = value*pow + (word->c_str()[i] - 48);
              else if (word->c_str()[i] >= 65 || word->c_str()[i] <= 70)
                value = value*pow + (word->c_str()[i] - 55);
              else if (word->c_str()[i] >= 97 || word->c_str()[i] <= 102)
                value = value*pow + (word->c_str()[i] - 87);
              else
                return false; //ERROR!
            }
          }

        } else if (word->c_str()[0] == 0x27) { //27 = '   This will be need to be fixed, this is for character literal
          for (int i = initial_i; i < word->size(); ++i){
            value = value*pow + (word->c_str()[i] - 48);
            //this has to check also for escape-char
          }

        } else {
          for (int i = initial_i; i < word->size(); ++i){
            if (word->c_str()[i] >= 48 || word->c_str()[i] <= 57)
              value = value*pow + (word->c_str()[i] - 48);
            else
              return false; //ERROR!
          }
        }
        value *= pos_negative;
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check (and read) if it's a float-literal
////////////////////////////////////////////////////////////////////////////////
      bool get_float_literal(float &value, string *word){
        int decimal = 1;
        int initial_i = 0;
        int pos_negative = 1;
        int pow = 10;
        int exponential = 1;
        int exp_pos_neg = 1;
        value = 0;

        //It may have - or +
        if (word->c_str()[0] == 0x2b){ //2b = +
          initial_i = 1; //ignore the first character
          pos_negative = 1; //is positive number
        }
        else if (word->c_str()[0] == 0x2d){ //2d = -
          initial_i = 1; //ignore the first character
          pos_negative = -1;
        }

        //It may be a binary-literal, hex-literal or float-literal (starting with 0 or .) or float-literal starting with any number
        if (word->c_str()[initial_i] == 0x30){ //30 = 0
          if (word->c_str()[initial_i + 1] == 0x42 || word->c_str()[initial_i + 1] == 0x62){ //42 = B, 62 = b, that meaning, it's a binary number
            pow = 2;
            initial_i += 2;
            //fill this with the reader of binaries
            for (int i = 0; i < word->size(); ++i){
              if (word->c_str()[i] == 48 || word->c_str()[i] == 49)
                value = value*pow + (word->c_str()[i] - 48);
              else
                return false; //ERROR!
            }
          }
          else if (word->c_str()[initial_i + 1] == 0x58 || word->c_str()[initial_i + 1] == 0x78){ //58 = X, 78 = x, that meaning, it's an hex number
            pow = 16;
            initial_i += 2;
            //fill this with the reader of exadecimals
            for (int i = 0; i < word->size(); ++i){
              if (word->c_str()[i] >= 48 || word->c_str()[i] <= 57)
                value = value*pow + (word->c_str()[i] - 48);
              else if (word->c_str()[i] >= 65 || word->c_str()[i] <= 70)
                  value = value*pow + (word->c_str()[i] - 55);
              else if (word->c_str()[i] >= 97 || word->c_str()[i] <= 102)
                value = value*pow + (word->c_str()[i] - 87);
              else
                return false; //ERROR!
            }
          }
        }
        
        if (pow == 10){
          int i;
          // It can be a decimal, so we will use decimal as a way to test if it's decimal or not
          decimal = -1;

          //this will read the left part of the dot 
          for (i = initial_i; i < word->size() && decimal == -1 && exponential == 1; ++i){
            if (word->c_str()[i] == 0x2e) //2e = .
              decimal = 1;
            else if (word->c_str()[i] == 0x45 || word->c_str()[i] == 0x65) //45 = E, 65 = e
              exponential = 0;
            else if (word->c_str()[i] >= 48 || word->c_str()[i] <= 57)
              value = value*pow + (word->c_str()[i] - 48);
            else
              return false; //ERROR!
          }

          //this part will read decimals (right part of dot)
          if (decimal != -1)
            for (; i < word->size() && exponential == 1; ++i){
              if (word->c_str()[i] == 0x45 || word->c_str()[i] == 0x65) //45 = E, 65 = e
                exponential = 0;
              else{
                decimal *= 10; 
                if (word->c_str()[i] >= 48 || word->c_str()[i] <= 57)
                  value = value*pow + (word->c_str()[i] - 48);
                else
                  return false; //ERROR!
              }
            }

          //this part will understand exponentials (if there is any)
          if (exponential == 0){
            if (word->c_str()[i] == 0x2b) //2b = +
              ++i;
            else if (word->c_str()[i] == 0x2d){ //2d = -
              ++i;
              exp_pos_neg = -1;
            }
            for (; i < word->size(); ++i){
              if (word->c_str()[i] >= 48 || word->c_str()[i] <= 57)
                exponential = exponential * 10 + (word->c_str()[i] - 48);
              else
                return false; //ERROR!
            }
          }

          //If decimal arrived here as -1, it's not decimal, so make decimal == 1; if it's different to -1, its a decimal, so keep decimal value
          decimal = decimal == -1 ? 1 : decimal;
        }

        //Now construct the number knowing value, pos_negative, decimal, pow, exponential!
        value = value*pos_negative / decimal;
        //printf("Obtaining number -> %f\n", value);
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a string-literal
////////////////////////////////////////////////////////////////////////////////
      bool get_string_literal(string &value, string *word){
        printf("Reading the string: ");
        char caracter;
        //first of all check if it's a correct string
        if (word->c_str()[0] != 0x22 || word->c_str()[word->size() - 1] != 0x22)
          return false; // ERROR!!!
        
        const int i_limit = word->size() - 1;
        dynarray<char> new_word(i_limit);
        for (int i = 1; i < i_limit; ++i){
          caracter = word->c_str()[i];
          if (caracter == 0x5c){ //5c = '\'
            //printf("Escape char\n");
            //Here it would be a function that decode the escape char. I'll do it later
          }
          else{ //If it's not a escape char, it's text
            new_word.data()[i] = word->c_str()[i];
            printf("%c", new_word.data()[i]);
          }
        }
        printf("\n");
        value = string(new_word.data());
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a reference
////////////////////////////////////////////////////////////////////////////////
      bool get_value_reference(string &value, string *word){
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function will check if it's a type
////////////////////////////////////////////////////////////////////////////////
      bool get_value_data_type(string &value, string *word){
        return true;
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
        //printf("\tProperties!\n");
        get_next_char();
        remove_comments_whitespaces();
        while (currentChar[0] != 0x29){
          //printf("%x, ", currentChar[0]);
          get_next_char();
          if (is_end_file()){
            return false;
            //printf("\n\nERROR!!!\n\n");
          }
        }
        get_next_char();
        //printf("\n End Properties!\n");
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This functions process the data list, taking into account the type of data expected
////////////////////////////////////////////////////////////////////////////////
      void process_data_list_element(int type, string *word){
        bool no_error = true;
        //Check the type, and call to the appropriate processor function
        switch (type){
        case token_type::tok_bool:
        {
          bool valueBool;
          no_error = get_bool_literal(valueBool, word);
          break;
        }
        case token_type::tok_int8:
        case token_type::tok_int16:
        case token_type::tok_int32:
        case token_type::tok_int64:
        case token_type::tok_uint8:
        case token_type::tok_uint16:
        case token_type::tok_uint32:
        case token_type::tok_uint64:
        {
          int valueInt;
          no_error = get_integer_literal(valueInt, word);
          break;
        }
        case token_type::tok_float:
        case token_type::tok_double:
        {
          float valueFloat;
          no_error = get_float_literal(valueFloat, word);
          break;
        }
        case token_type::tok_string:
        {
          string valueString;
          no_error = get_string_literal(valueString, word);
          break;
        }
        case token_type::tok_ref:
        {
          string valueRef;
          no_error = get_value_reference(valueRef, word);
          break;
        }
        case token_type::tok_type:
        {
          string valueType;
          no_error = get_value_data_type(valueType, word);
          break;
        }
        default:
          break;
        };
      }
      
////////////////////////////////////////////////////////////////////////////////
/// @brief  This function has to process the datalist
///  This function will keep on checking data until it finds a }. 
/// @return true if it went ok and false if there was any problem (for instance not finding a })
////////////////////////////////////////////////////////////////////////////////
      bool process_data_list(int type){
        int ending;
        string *word = new string();
        //printf("{ ");
        ending = read_data_list_element(word);
        process_data_list_element(type, word);
        while (ending == 1){
          //printf(", ");
          get_next_char();
          ending = read_data_list_element(word);
          process_data_list_element(type, word);
        }
        //printf(" }\n");
        return ending >= 0;
      }
      
////////////////////////////////////////////////////////////////////////////////
/// @brief  This function has to process the dataarraylist
///  This function will keep on checking data until it finds a }. 
/// @return true if it went ok and false if there was any problem (for instance not finding a })
////////////////////////////////////////////////////////////////////////////////
      bool process_data_array(int type, int arraySize){
        //detect {
        if (currentChar[0] != 0x7b){ //7b = {
          printf("Problem reading the begining of the data array!!! \n");
          return false;
        }
        else
          printf("Reading new array list!%x\n", currentChar[0]);
        //read elements
        int itemsLeft = arraySize;
        while (itemsLeft > 0){
          get_next_char();
          remove_comments_whitespaces();
          printf("Start reading with the thing...%x\n", currentChar[0]);
          while (currentChar[0] != 0x2c && currentChar[0] != 0x7d){ // 2c = ,
            get_next_char();
          }
          printf("Stop reading because it find...%x\n", currentChar[0]);
          --itemsLeft;
        }
        //detect }
        if (currentChar[0] != 0x7d){ //7d = }
          return false;
          printf("I Don't find the } inside process_data_array\n");
        }
        return true;
      }

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function has to process the datalist
///  This function will keep on checking data until it finds a }. 
/// @return true if it went ok and false if there was any problem (for instance not finding a })
////////////////////////////////////////////////////////////////////////////////
      bool process_data_array_list(int type, int arraySize){
        bool no_error = true;
        if (currentChar[0] != 0x7b){ //7b = {
          printf("Problem reading the begining of the data array!!! \n");
          return false;
        }
        get_next_char();
        remove_comments_whitespaces();
        while (no_error && currentChar[0] != 0x7d){
          no_error = process_data_array(type, arraySize); //This will have to start with {, read arraySize elements, read }
          get_next_char();
          remove_comments_whitespaces();
          printf("After data array...%x\n", currentChar[0]);
        }
        //expect } (7d)

        if (currentChar[0] != 0x7d){
          no_error = false;
          printf("----ERROR WITH DATA ARRAY LIST\n");
        }

        return no_error;
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
         // printf("%x, ", currentChar[0]);
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
        //printf("\n");
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
          //if (is_symbol()) get_previous_char();
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