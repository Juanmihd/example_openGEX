////////////////////////////////////////////////////////////////////////////////
/// @file openDDL_lexer.h
/// @author Juanmi Huertas Delgado
/// @brief This is the lexer to load the files of OpenGEX
///
///   NOTE: Some of the functions will have some "no_error" to track if there is an error.
///         likewise when in the coments there is a two digit number = something, that stands
///         for the hexadecimal representation of that symbol in UTF8
///         if there is nothing regarding that number, that mean that it's an hexadecimal
///         in case of doubt just check the number "before" the comment
////////////////////////////////////////////////////////////////////////////////

#include "openDDL_tokens.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
////////////////////////////////////////////////////////////////////////////////
namespace octet
{
  namespace loaders{
    class openDDL_lexer : ddl_token{
      enum { debuggingDDL = 1, debuggingDDLMore = 0 };
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
      /// @param  id  , stands for the name of the identificator 
      /// @param  tok , stands for the token of the identificator
      ////////////////////////////////////////////////////////////////////////////////
      void add_identifier(const char *id, int tok){
        identifiers_[id] = tok;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will add an types to the dictionary
      /// @param  id  , stands for the name of the type
      /// @param  tok , stands forthe token of the type
      ////////////////////////////////////////////////////////////////////////////////
      void add_type(const char *id, int tok){
        types_[id] = tok;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will add an symbols to the dictionary
      /// @param  id  , stands for the name of the symbol to add (the character)
      /// @param  tok , stands for the token of the symbol
      ////////////////////////////////////////////////////////////////////////////////
      void add_symbol(const char *id, int tok){
        symbols_[id] = tok;
      }

      // Some small functions to make easier the lexer process
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function checks if the currentChar is in at the end of the file (or has passed it)
      /// @return This function returns true if the pointer is at the end of the file
      ////////////////////////////////////////////////////////////////////////////////
      bool is_end_file(){
        return bufferSize <= 0;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will get a new char
      ///   To do so, it will decrease the number of elements, and it will get a new char
      ///   this function does not check if it's the end of the file! (It should?)
      ////////////////////////////////////////////////////////////////////////////////
      void get_next_char(){
        --bufferSize;
        ++currentChar;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will get the previous char
      ///   To do so, it will increase the number of elements, and it will get the previous char
      ////////////////////////////////////////////////////////////////////////////////
      void get_previous_char(){
        ++bufferSize;
        --currentChar;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Jumps the char "pos" positions
      /// @param  pos , stands for the number of characters that need to jump
      ///     This function will be useful when "jumpin" some characters that have already been readed
      ///     Be aware that it does not check if it has passed the end of the file!!!!
      ////////////////////////////////////////////////////////////////////////////////
      void char_jump(int pos){
        bufferSize -= pos;
        currentChar += pos;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This will check if the current char is equal the string
      /// @param  word  , This is the word that has to be checked
      /// @returns  True if the word /word is equal to the first characters from the current character
      ///     ToDO: Is this function useful? Check if it has been used later
      ///  (deprecated)
      ////////////////////////////////////////////////////////////////////////////////
      bool char_word_is(string word){
        sizeRead = 0;
        while ((sizeRead < word.size()) && (currentChar[sizeRead] == (uint8_t)word[sizeRead]))
          ++sizeRead;
        return sizeRead == word.size();
      }
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will test if the current token is a whitespace (less than 0x20 character)
      /// @return True if the current character is a whiteSpace (any value less than 0x20)
      ////////////////////////////////////////////////////////////////////////////////
      bool is_whiteSpace(){ // everything less or equal than 0x20 is a whitespace
        return *currentChar <= 0x20;
      }
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function ignore any comment or whitespace
      ///     ToDO: This is used really often, check if it is really efficient!
      ////////////////////////////////////////////////////////////////////////////////
      void remove_comments_whitespaces(){ // everything less or equal than 0x20 is a whitespace
        while (is_comment() || is_whiteSpace()){
          if (is_comment()) ignore_comment();
          else get_next_char();
        }
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will test if the current token is going to be a comment (// or /*)
      /// @returns This function returns true if the currentChar is the begining of a comment
      ////////////////////////////////////////////////////////////////////////////////
      bool is_comment(){ //0x2f = /  and  0x2A = *
        return *currentChar == 0x2F && (currentChar[1] == 0x2F || currentChar[1] == 0x2A);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This will lexer a comment with /* */or // and break line
      ///     ToDo: This function is used A LOT. Check if it's really good, and make it better!
      ////////////////////////////////////////////////////////////////////////////////
      void ignore_comment(){
        //It comes from reading the / so, get next char
        get_next_char();

        switch (*currentChar){
        case 0x2F: // 0x2f = /  as it comes from another / that means that it's a // comment
          if (debuggingDDLMore) printf("(//comment)");
          get_next_char();
          while (*currentChar != 0x0D && *currentChar != 0x0A){
            if (debuggingDDLMore) printf("%c", *currentChar);
            get_next_char();
          }
          if (debuggingDDLMore) printf("_%c", *currentChar);
          break;
        case 0x2A: // 0x2a = * as it comes from another / that means that it's a /* comment
          if (debuggingDDLMore) printf("(/**/comment)");
          get_next_char();
          while (*currentChar != 0x2A && currentChar[1] != 0x2f){ // until next characters are * and / 
            if (debuggingDDLMore) printf("%c", *currentChar);
            get_next_char();
            if (is_end_file()) assert(0 && "It's missing the */ of a comment");
          }
          if (debuggingDDLMore) printf("_%c", *currentChar);
          get_next_char();
          if (debuggingDDLMore) printf("_%c", *currentChar);
          get_next_char();
          break;
        }
        while (is_whiteSpace()) get_next_char();
        if (debuggingDDLMore) printf("\n%x\n", *currentChar);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will test if the current token is a dataType (will return the index of the type
      /// @param  word    It's the word to check if it's a data type
      /// @return   it returns the value of the type readed
      ////////////////////////////////////////////////////////////////////////////////
      int is_dataType(string word){
        int index = types_.get_index(word.c_str());
        return index;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will test if the current token is a identifier
      /// @param  word    It's the word to check if it's a identifier type
      /// @return   True if it's a identifier, and false if it's not a identifier
      ////////////////////////////////////////////////////////////////////////////////
      int is_identifier(string word){
        int index = identifiers_.get_index(word.c_str());
        return index;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will test if the current token is a identifier (version with *char)
      /// @param  charWord    It's the word to check if it's a identifier type
      /// @param  word_size   It's the size of the word to check
      /// @return   True if it's a identifier, and false if it's not a identifier
      ////////////////////////////////////////////////////////////////////////////////
      int is_identifier(char *charWord, int word_size){
        /*charWord = new char[word_size];
        for (int i = 0; i < word_size; ++i)
          charWord[i] = tempChar[i];*/
        int index = identifiers_.get_index(charWord);
        //delete charWord;
        return index;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will test if the current token is a name
      /// @return   True if it's a name, and false if it's not a name
      ////////////////////////////////////////////////////////////////////////////////
      bool is_name(){
        return (*currentChar == '%') || (*currentChar == '$');
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will test if the current character is a symbol { } [ ] ( ) , =
      /// @return   True if it's a symbol, and false if it's not a symbol
      ////////////////////////////////////////////////////////////////////////////////
      bool is_symbol(){
        string character ((char*)currentChar,1);
        return symbols_.contains(character.c_str());
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a bool-literal and return it's value (will check if there is any problem)
      /// @param  value   it returns the value of the boolean
      /// @param  word    this is the word to lexer into a value
      /// @return   True if everything went right, and false if something went wrong
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
      /// @param  value   it returns the value of the integer
      /// @param  word    this is the word to lexer into a value
      /// @return   True if everything went right, and false if something went wrong
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
      /// @param  value   it returns the value of the float
      /// @param  word    this is the word to lexer into a value
      /// @return   True if everything went right, and false if something went wrong
      ///   ToDo: Replace String for char* 
      ///   ToDo: Check this line: "value = value * 16 + ( ( *src - ( *src < 'A' ? '0' : 'A'-10 ) ) & 15 );"
      ////////////////////////////////////////////////////////////////////////////////
      bool get_float_literal(float &value, string *word){
        const char *src = word->c_str(); //This will be used to check each letter of the "word"
        int decimal = 1; //This will be use to check how many decimals
        int initial_i = 0; //This will be used to "jump" the firs character if there is a sign (+, -)
        int pos_negative = 1; //This will be used to determine if it's positive or negative
        int pow = 10; //This will be use to always track which kind of power we are aplying
        int exponential = 1; //This will be use to check the exponential
        int exp_pos_neg = 1; //This will be use to check the sign of the exponential
        value = 0;

        //It may have - or +
        if (src[0] == 0x2b){ //2b = +
          initial_i = 1; //ignore the first character
          pos_negative = 1; //is positive number
        }
        else if (src[0] == 0x2d){ //2d = -
          initial_i = 1; //ignore the first character
          pos_negative = -1;
        }

        resource_dict a;

        //It may be a binary-literal, hex-literal or float-literal (starting with 0 or .) or float-literal starting with any number
        if (src[initial_i] == 0x30){ //30 = 0
          //------CHECKING BINARY NUMBER
          if (src[initial_i + 1] == 0x42 || src[initial_i + 1] == 0x62){ //42 = B, 62 = b, that meaning, it's a binary number
            pow = 2;
            initial_i += 2;
            //fill this with the reader of binaries
            for (int i = initial_i; i < word->size(); ++i){
              if (src[i] == 48 || src[i] == 49)  //48 in decimal is the symbol 0, 49 in decimal is the symbol 1
                value = value*2 + (src[i] - 48);
              else
                return false; //ERROR!
            }
          }

          //------CHECKING HEXADECIMAL NUMBER
          else if (src[initial_i + 1] == 0x58 || src[initial_i + 1] == 0x78){ //58 = X, 78 = x, that meaning, it's an hex number
            union tupla { int i; float f; } u;
            pow = 16;
            initial_i += 2;
            u.i = 0;
            //fill this with the reader of exadecimals
            for (int i = initial_i; i < word->size(); ++i){
              if (src[i] >= 0x30 && src[i] <= 0x39)
                u.i = u.i * 16 + (src[i] - 0x30);
              else if (src[i] >= 0x41 && src[i] <= 0x5A)
                u.i = u.i * 16 + (src[i] - (0x41 - 10));
              else if (src[i] >= 0x61 && src[i] <= 0x7A)
                u.i = u.i * 16 + (src[i] - (0x61 - 10));
              else
                return false; //ERROR!
            }
            value = u.f;
            if (debuggingDDLMore) printf("===> %f, %8x <==== Is this right?\n", u.f, u.i);
          }
        }

        //------CHECKING DECIMAL NUMBER
        if (pow == 10){
          int i;
          // It can be a decimal, so we will use decimal as a way to test if it's decimal or not
          decimal = -1;

          //this will read the left part of the dot 
          for (i = initial_i; i < word->size() && decimal == -1 && exponential == 1; ++i){
            if (src[i] == 0x2e) //2e = .
              decimal = 1;
            else if (src[i] == 0x45 || src[i] == 0x65) //45 = E, 65 = e
              exponential = 0;
            else if (src[i] >= 48 && src[i] <= 57)
              value = value*10 + (src[i] - 48);
            else{
              printf("It's not a correct digit!\n");
              return false; //ERROR!
            }
          }
          //------CHECKING right part of the DECIMAL NUMBER
          //this part will read decimals (right part of dot)
          if (decimal != -1)
            for (; i < word->size() && exponential == 1; ++i){
              if (src[i] == 0x45 || src[i] == 0x65) //45 = E, 65 = e
                exponential = 0;
              else{
                decimal *= 10; 
                if (src[i] >= 48 && src[i] <= 57)
                  value = value*10 + (src[i] - 48);
                else{
                  printf("It's not a correct digit!\n");
                  return false; //ERROR!
                }
              }
            }
          //------CHECKING exponential part of the DECIMAL NUMBER
          //this part will understand exponentials (if there is any)
          if (exponential == 0){
            if (src[i] == 0x2b) //2b = +
              ++i;
            else if (src[i] == 0x2d){ //2d = -
              ++i;
              exp_pos_neg = -1;
            }
            for (; i < word->size(); ++i){
              if (src[i] >= 48 && src[i] <= 57)
                exponential = exponential * 10 + (src[i] - 48);
              else{
                printf("It's not a correct digit!\n");
                return false; //ERROR!
              }
            }
          }

          //If decimal arrived here as -1, it's not decimal, so make decimal == 1; if it's different to -1, its a decimal, so keep decimal value
          decimal = decimal == -1 ? 1 : decimal;
        }

        //Now construct the number knowing value, pos_negative, decimal, pow, exponential!
        value = value*pos_negative / decimal;
        if (debuggingDDL) printf("Number-> %f ", value);
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a string-literal
      /// @param  value   it returns the value of the string
      /// @param  word    this is the word to lexer into a value
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_string_literal(string &value, string *word){
        if (debuggingDDL) printf("Reading the string: ");
        char caracter;
        //first of all check if it's a correct string
        if (word->c_str()[0] != 0x22 || word->c_str()[word->size() - 1] != 0x22)
          return false; // ERROR!!!
        
        const int i_limit = word->size() - 1;
        dynarray<char> new_word(i_limit);
        for (int i = 1; i < i_limit; ++i){
          caracter = word->c_str()[i];
          if (caracter == 0x5c){ //5c = '\'
            if (debuggingDDL) printf("Escape char\n");
            //Here it would be a function that decode the escape char. I'll do it later
          }
          else{ //If it's not a escape char, it's text
            new_word.data()[i] = word->c_str()[i];
            if (debuggingDDL) printf("%c", new_word.data()[i]);
          }
        }
        if (debuggingDDL) printf("\n");
        value = string(new_word.data());
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a reference
      /// @param  value   it returns the value of the reference
      /// @param  word    this is the word to lexer into a value
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_value_reference(string &value, string *word){


        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a type
      /// @param  value   it returns the value of the type
      /// @param  word    this is the word to lexer into a value
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_value_data_type(string &value, string *word){
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This functions process the name, and add it to the application
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      void process_name(){
        string name = read_word();
        if (debuggingDDL) printf("It's the name %s !!\n", name);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This functions process the properties, and add it to the application
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool process_properties(){
        int word_size;
        if (debuggingDDL) printf("\tProperties!\n");
        get_next_char();
        remove_comments_whitespaces();
        //Now it has to find a identifier
        word_size = read_word_size();
        //Now it has to find an =

        //Not it has to find a literal, that might be (bool, int, float, string, ref or type)

        //Not it has to find a }
        while (*currentChar != 0x29){
          if (debuggingDDL) printf("%x, ", currentChar[0]);
          get_next_char();
          if (is_end_file()){
            return false;
            printf("\n\nERROR!!!\n\n");
          }
        }
        get_next_char();
        if (debuggingDDL) printf("\n End Properties!\n");
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This functions process the data list, taking into account the type of data expected
      /// @param  type  represent the type of element that it's expecting
      /// @param  word  pointer to the word readed
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
      /// @param  type  This is the type of the data list elements
      /// @return true if it went ok and false if there was any problem (for instance not finding a })
      ///   This function will keep on checking data until it finds a }. 
      ////////////////////////////////////////////////////////////////////////////////
      bool process_data_list(int type){
        int ending;
        string word;
        ending = read_data_list_element(&word);
        process_data_list_element(type, &word);
        while (ending == 1){
          get_next_char();
          ending = read_data_list_element(&word);
          process_data_list_element(type, &word);
        }
        if (debuggingDDL) printf("\n");
        return ending >= 0;
      }
      
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function has to process the dataarraylist
      /// @param  type  This is the type of the elements that it's reading in the array
      /// @param  arraySize   This is the size of the array (it will be used to make faster the parser process)
      /// @return true if it went ok and false if there was any problem (for instance not finding a })
      ///  This function will keep on checking data until it finds a }. 
      ////////////////////////////////////////////////////////////////////////////////
      int process_data_array(int type, int arraySize){
        //detect {
        if (*currentChar != 0x7b){ //7b = {
          printf("Problem reading the begining of the data array!!! \n");
          return false;
        }
        //read elements
        string word;
        int ending;
        int itemsLeft = arraySize;
        while (itemsLeft > 0){
          get_next_char();
          remove_comments_whitespaces();
          ending = read_data_list_element(&word);
          process_data_list_element(type, &word);
          --itemsLeft;
        }
        //detect }
        if (debuggingDDL) printf("_%x\n", currentChar[0]);
        if (*currentChar != 0x7d){ //7d = }
          return false;
          printf("I Don't find the } inside process_data_array\n");
        }
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function has to process the datalist
      /// @param  type  This is the type of the elements that it's reading in the array
      /// @param  arraySize   This is the size of the array (it will be used to make faster the parser process)
      /// @return true if it went ok and false if there was any problem (for instance not finding a })
      ///  This function will keep on checking data until it finds a }. 
      ////////////////////////////////////////////////////////////////////////////////
      bool process_data_array_list(int type, int arraySize){
        bool no_error = true;
        if (*currentChar != 0x7b){ //7b = {
          printf("Problem reading the begining of the data array!!! \n");
          return false;
        }
        get_next_char();
        remove_comments_whitespaces();
        while (no_error && *currentChar != 0x7d){
          no_error = process_data_array(type, arraySize); //This will have to start with {, read arraySize elements, read }
          get_next_char();
          remove_comments_whitespaces();
          if (debuggingDDL) printf("After data array...%x\n", currentChar[0]);
        }
        //expect } (7d)

        if (*currentChar != 0x7d){
          no_error = false;
          printf("----ERROR WITH DATA ARRAY LIST\n");
        }

        return no_error;
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
        while (*currentChar != 0x2c && *currentChar != 0x7d && !is_whiteSpace() && !is_comment()){
          if (debuggingDDLMore) printf("%x, ", currentChar[0]);
          get_next_char();
          ++sizeWord;
        }

        remove_comments_whitespaces();

        switch (*currentChar){
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
      /// @brief  This function will check the size of the array
      /// @return Returns the size of the array
      ////////////////////////////////////////////////////////////////////////////////
      int read_array_size(){
        int sizeNumber = 0, number = 0, pow = 1;
        tempChar = currentChar;
        while (*currentChar != 0x5d){ // 5d = ]
          get_next_char();
          ++sizeNumber;
        }
        if (debuggingDDLMore) printf("\n");
        --sizeNumber;
        for (int i = 0; i <= sizeNumber; ++i){
          number += pow*((int)tempChar[sizeNumber-i]-48);
          pow *= 10;
        }
        return number;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief It will read and return a word
      /// @return it will return the word readed
      /// It will start geting characters and return them as a word. It will stop when it finds the begining a comment or a whitespace
      ///       ToDO: ACTUALLY, I don't need STRINGS, it could be enough for me just use the char*, and the size of the word. That's enough to 
      ///             work with the words!!!!!!!!!!!!!!!!
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
        if (debuggingDDLMore) printf("Last symbol-> %x finding %s\n", currentChar[0], temp);
        if (debuggingDDL) printf("Finding => %s ", temp);
        return temp;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief It will read and return the size of the next word
      /// @return Integer with the<size of the word
      ///     NOTICE: This will let "currentChar" at the begining of the next token
      ///             To use the word, it has to work with tempChar and the size that this
      ///             function returns as an integer
      ////////////////////////////////////////////////////////////////////////////////
      int read_word_size(){
        int sizeWord = 0;
        tempChar = currentChar;
        if (is_symbol())
          sizeWord = 1;
        else{
          while (!is_symbol() && !is_whiteSpace() && !is_comment()){
            
            ++sizeWord;
          }
          //if (is_symbol()) get_previous_char();
          if (is_comment()){
            ignore_comment();
            get_next_char();
          }
        }
        return sizeWord;
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
