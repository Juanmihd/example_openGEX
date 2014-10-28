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

#ifndef OPENDDL_LEXER_INCLUDED
#define OPENDDL_LEXER_INCLUDED
#include "openDDL_tokens.h"
#include "openDDL_structures.h"

namespace octet
{
  namespace loaders{

////////////////////////////////////////////////////////////////////////////////
/// @brief This class is the openGEX lexer, it will read the array of characters and get tokes
////////////////////////////////////////////////////////////////////////////////
    class openDDL_lexer : ddl_token{
      enum { MIN_RESERVING_DATA_LIST = 10, debuggingDDL = 0, debuggingDDLMore = 0, debugging = 0, debuggingMore = 0 };
    protected:
      // Dictionary of identifiers of the openDDL language we are using
      dictionary<int> identifiers_;
      // Dictionary of types of openDDL
      dictionary<int> types_;
      // Dictionary of names
      dictionary<openDDL_structure *> names_;
      // Dictionary of symbols
      dictionary<int> symbols_;
      // This are the current character and the next character after the token
      uint8_t * currentChar;
      uint8_t * tempChar;
      // This is the current buffer with the characters in UTF8
      dynarray<uint8_t> buffer;
      // The number of characters till the end of the file
      int bufferSize;
      // The size of the token
      int sizeRead;
      // Just to check nesting
      int nesting;
      // This will be the openDDL file being (just a series of structures)
      dynarray<openDDL_structure *> openDDL_file; 
      openDDL_data_list * current_data_list;
      openDDL_structure * current_structure;
      openDDL_data_literal* current_literal;

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
        if(!is_end_file())
          while ((is_whiteSpace() || is_comment()) && !is_end_file()){
            if (is_whiteSpace()) get_next_char(); 
            else ignore_comment();
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
        string character((char*)currentChar, 1);
        return symbols_.contains(character.c_str());
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a bool-literal and return it's value (will check if there is any problem)
      /// @param  value   it returns the value of the boolean
      /// @param  word    this is a pointer to the beginning of the word
      /// @param  size    this is the size of the word readed
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_bool_literal(bool &value, char *word, int size){
        if (size == 4){ //true.size() == 4
          if (word[0] == 't' && word[1] == 'r' && word[2] == 'u' && word[4] == 'e')
            value = true;
          else
            return false;
        }
        else if (size == 5){ //false.size() == 5
          if (word[0] == 'f' && word[1] == 'a' && word[2] == 'l' && word[4] == 's' && word[5] == 'e')
            value = false;
          else
            return false;
        }
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a int-literal
      /// @param  value   it returns the value of the integer
      /// @param  word    this is a pointer to the beginning of the word
      /// @param  size    this is the size of the word readed
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_integer_literal(int &value, char *word, int size){
        value = 0;
        int pow = 10;
        int initial_i = 0;
        int pos_negative = 1;

        //It may have - or +
        if (*word == 0x2b){ //2b = +
          initial_i = 1; //ignore the first character
          pos_negative = 1; //is positive number
        }
        else if (*word == 0x2d){ //2d = -
          initial_i = 1; //ignore the first character
          pos_negative = -1;
        }


        //It may be a binary-literal, hex-literal, int-literal or char-literal
        if (word[initial_i] == 0x30){ //30 = 0
          if (word[initial_i + 1] == 0x42 || word[initial_i + 1] == 0x62){ //42 = B, 62 = b, that meaning, it's a binary number
            pow = 2;
            initial_i += 2;
            //fill this with the reader of binaries
            for (int i = 0; i < size; ++i){
              if (word[i] == 48 || word[i] == 49)
                value = value*pow + (word[i] - 48);
              else
                return false; //ERROR!
            }
          }

          else if (word[initial_i + 1] == 0x58 || word[initial_i + 1] == 0x78){ //58 = X, 78 = x, that meaning, it's an hex number
            pow = 16;
            initial_i += 2;
            //fill this with the reader of exadecimals
            for (int i = 0; i < size; ++i){
              if (word[i] >= 48 || word[i] <= 57)
                value = value*pow + (word[i] - 48);
              else if (word[i] >= 65 || word[i] <= 70)
                value = value*pow + (word[i] - 55);
              else if (word[i] >= 97 || word[i] <= 102)
                value = value*pow + (word[i] - 87);
              else
                return false; //ERROR!
            }
          }

        }
        else if (*word == 0x27) { //27 = '   This will be need to be fixed, this is for character literal
          for (int i = initial_i; i < size; ++i){
            value = value*pow + (word[i] - 48);
            //this has to check also for escape-char
          }

        }
        else {
          for (int i = initial_i; i < size; ++i){
            if (word[i] >= 48 || word[i] <= 57)
              value = value*pow + (word[i] - 48);
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
      /// @param  word    this is a pointer to the beginning of the word
      /// @param  size    this is the size of the word readed
      /// @return   True if everything went right, and false if something went wrong
      ///   ToDo: Replace String for char* 
      ///   ToDo: Check this line: "value = value * 16 + ( ( *src - ( *src < 'A' ? '0' : 'A'-10 ) ) & 15 );"
      ////////////////////////////////////////////////////////////////////////////////
      bool get_float_literal(float &value, char *word, int size){
        int decimal = 1; //This will be use to check how many decimals
        int initial_i = 0; //This will be used to "jump" the firs character if there is a sign (+, -)
        int pos_negative = 1; //This will be used to determine if it's positive or negative
        int pow = 10; //This will be use to always track which kind of power we are aplying
        int exponential = 1; //This will be use to check the exponential
        int exp_pos_neg = 1; //This will be use to check the sign of the exponential
        value = 0;

        //It may have - or +
        if (word[0] == 0x2b){ //2b = +
          initial_i = 1; //ignore the first character
          pos_negative = 1; //is positive number
        }
        else if (word[0] == 0x2d){ //2d = -
          initial_i = 1; //ignore the first character
          pos_negative = -1;
        }

        //It may be a binary-literal, hex-literal or float-literal (starting with 0 or .) or float-literal starting with any number
        if (word[initial_i] == 0x30){ //30 = 0
          //------CHECKING BINARY NUMBER
          if (word[initial_i + 1] == 0x42 || word[initial_i + 1] == 0x62){ //42 = B, 62 = b, that meaning, it's a binary number
            pow = 2;
            initial_i += 2;
            //fill this with the reader of binaries
            for (int i = initial_i; i < size; ++i){
              if (word[i] == 48 || word[i] == 49)  //48 in decimal is the symbol 0, 49 in decimal is the symbol 1
                value = value * 2 + (word[i] - 48);
              else
                return false; //ERROR!
            }
          }

          //------CHECKING HEXADECIMAL NUMBER
          else if (word[initial_i + 1] == 0x58 || word[initial_i + 1] == 0x78){ //58 = X, 78 = x, that meaning, it's an hex number
            union tupla { int i; float f; } u;
            pow = 16;
            initial_i += 2;
            u.i = 0;
            //fill this with the reader of exadecimals
            for (int i = initial_i; i < size; ++i){
              if (word[i] >= 0x30 && word[i] <= 0x39)
                u.i = u.i * 16 + (word[i] - 0x30);
              else if (word[i] >= 0x41 && word[i] <= 0x5A)
                u.i = u.i * 16 + (word[i] - (0x41 - 10));
              else if (word[i] >= 0x61 && word[i] <= 0x7A)
                u.i = u.i * 16 + (word[i] - (0x61 - 10));
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
          for (i = initial_i; i < size && decimal == -1 && exponential == 1; ++i){
            if (word[i] == 0x2e) //2e = .
              decimal = 1;
            else if (word[i] == 0x45 || word[i] == 0x65) //45 = E, 65 = e
              exponential = 0;
            else if (word[i] >= 48 && word[i] <= 57)
              value = value * 10 + (word[i] - 48);
            else{
              printf("It's not a correct digit!\n");
              return false; //ERROR!
            }
          }
          //------CHECKING right part of the DECIMAL NUMBER
          //this part will read decimals (right part of dot)
          if (decimal != -1)
            for (; i < size && exponential == 1; ++i){
            if (word[i] == 0x45 || word[i] == 0x65) //45 = E, 65 = e
              exponential = 0;
            else{
              decimal *= 10;
              if (word[i] >= 48 && word[i] <= 57)
                value = value * 10 + (word[i] - 48);
              else{
                printf("It's not a correct digit!\n");
                return false; //ERROR!
              }
            }
            }
          //------CHECKING exponential part of the DECIMAL NUMBER
          //this part will understand exponentials (if there is any)
          if (exponential == 0){
            if (word[i] == 0x2b) //2b = +
              ++i;
            else if (word[i] == 0x2d){ //2d = -
              ++i;
              exp_pos_neg = -1;
            }
            for (; i < size; ++i){
              if (word[i] >= 48 && word[i] <= 57)
                exponential = exponential * 10 + (word[i] - 48);
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
      /// @brief  This function check if the character is a hexadecimal digit
      /// @param  c   Character to be tested
      /// @return   True if it's an hexadecimal, false if it's not 8
      ///   Note: Decimals are a subset of hexadecimal!
      ////////////////////////////////////////////////////////////////////////////////
      bool is_hex_digit(char c){
        return (c >= 0x30) && (c <= 0x66) && !(c > 0x39 && c < 0x41) && !(c > 0x47 && c < 0x61);
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a string-literal
      /// @param  value   it returns the value of the string
      /// @param  word    this is a pointer to the beginning of the word
      /// @param  size    this is the size of the word readed
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_string_literal(string &value, char *word, int size){
        if (debuggingDDL) printf("Reading the string: ");
        char caracter;
        //first of all check if it's a correct string
        if (*word != 0x22 || word[size - 1] != 0x22){
          return false; // ERROR!!!
          printf("Error with the string! \n");
        }

        const int i_limit = size - 1;
        int new_size = 0;
        dynarray<char> new_word(i_limit - 1);
        ++word;
        for (int i = 1; i < i_limit; ++i, ++word){
          caracter = *word;
          if (caracter == 0x5c){ //5c = '\'
            if (debuggingDDL) printf("Escape char\n");
            ++i;
            if (i >= i_limit){
              printf("There is an error with the string\n");
              return false;
            }
            ++word;
            switch (*word){
            case 0x22: //22 = " means double quote 0x22
            case 0x27: //27 = ' means Single quote 0x27
            case 0x3F: //3f = ? means question mark 0x3f
            case 0x5C: //5c = \ means blackslash 0x5c
              caracter = *word;
              break;
            case 0x61: //61 = a means bell 0x07
              caracter = 0x07;
              break;
            case 0x62: //62 = b means backspace 0x08
              caracter = 0x08;
              break;
            case 0x66: //66 = f means formfeed 0x0c
              caracter = 0x0c;
              break;
            case 0x6E: //6e = n means newline 0x0a
              caracter = 0x0a;
              break;
            case 0x72: //72 = r means carriage return 0x0d
              caracter = 0x0d;
              break;
            case 0x74: //74 = t means horizontal tab 0x09
              caracter = 0x09;
              break;
            case 0x76: //76 = v means vertical tab 0x0b
              caracter = 0x0b;
              break;
            case 0x78: //78 = x means is a especial escape char
              i += 2;
              if (is_hex_digit(word[1]) && is_hex_digit(word[2])){
                caracter = (word[1] - (word[1]<0x39 ? '0' : (word[1]<0x60 ? 'A' : 'a'))) * 16 + (word[2] - (word[2]<0x39 ? '0' : (word[2]<0x60 ? 'A' : 'a')));
              }
              else{
                printf("Error with the escpace char, it's not a hexadecimal!\n");
              }
              break;
            default:
              printf("Error with the escape char!\n");
              return false;
              break;
            }

          }
          //and then, once the escape char was tested (if it was a escapeChar) and if not the same caracter
          new_word.data()[new_size] = caracter;
          ++new_size;
        }
        value = string(new_word.data(), new_size);

        if (debuggingDDL) printf("%s\n", value);
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a string-literal
      /// @param  new_word   this will be the pointer to the new word in a char*
      /// @param  new_size   this will be the new size of the char*
      /// @param  word    this is a pointer to the beginning of the word
      /// @param  size    this is the size of the word readed
      /// @return   True if everything went right, and false if something went wrong
      ///   The pointer new_word has to be previously initialized in memory with "size" 
      ///   reserved in memory. 
      ////////////////////////////////////////////////////////////////////////////////
      bool get_string_literal(char * new_word, int &new_size, char *word, int size){
        if (debuggingDDL) printf("Reading the string: ");
        char caracter;
        //first of all check if it's a correct string
        if (*word != 0x22 || word[size - 1] != 0x22){
          return false; // ERROR!!!
          printf("Error with the string! \n");
        }

        const int i_limit = size - 1;
        new_size = 0;
        ++word;
        for (int i = 1; i < i_limit; ++i, ++word){
          caracter = *word;
          if (caracter == 0x5c){ //5c = '\'
            if (debuggingDDL) printf("Escape char\n");
            ++i;
            if (i >= i_limit){
              printf("There is an error with the string\n");
              return false;
            }
            ++word;
            switch (*word){
            case 0x22: //22 = " means double quote 0x22
            case 0x27: //27 = ' means Single quote 0x27
            case 0x3F: //3f = ? means question mark 0x3f
            case 0x5C: //5c = \ means blackslash 0x5c
              caracter = *word;
              break;
            case 0x61: //61 = a means bell 0x07
              caracter = 0x07;
              break;
            case 0x62: //62 = b means backspace 0x08
              caracter = 0x08;
              break;
            case 0x66: //66 = f means formfeed 0x0c
              caracter = 0x0c;
              break;
            case 0x6E: //6e = n means newline 0x0a
              caracter = 0x0a;
              break;
            case 0x72: //72 = r means carriage return 0x0d
              caracter = 0x0d;
              break;
            case 0x74: //74 = t means horizontal tab 0x09
              caracter = 0x09;
              break;
            case 0x76: //76 = v means vertical tab 0x0b
              caracter = 0x0b;
              break;
            case 0x78: //78 = x means is a especial escape char
              i += 2;
              if (is_hex_digit(word[1]) && is_hex_digit(word[2])){
                caracter = (word[1] - (word[1]<0x39 ? '0' : (word[1]<0x60 ? 'A' : 'a'))) * 16 + (word[2] - (word[2]<0x39 ? '0' : (word[2]<0x60 ? 'A' : 'a')));
              }
              else{
                printf("Error with the escpace char, it's not a hexadecimal!\n");
              }
              break;
            default:
              printf("Error with the escape char!\n");
              return false;
              break;
            }

          }
          //and then, once the escape char was tested (if it was a escapeChar) and if not the same caracter
          new_word[new_size] = caracter;
          ++new_size;
        }

        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a char-literal
      /// @param  value   it returns the value of the char
      /// @param  word    this is a pointer to the beginning of the word
      /// @param  size    this is the size of the word readed
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_char_literal(char &value, char *word, int size){
        if (debuggingDDL) printf("Reading the string: ");
        //first of all check if it's a correct char literal
        if (size < 3 || size > 5){
          printf("The size of the char literal is not correct!\n");
          return false;
        }
        if (*word != 0x27 || word[size - 1] != 0x27){
          return false; // ERROR!!!
          printf("Error with the char! \n");
        }
        ++word;

        if (*word == 0x5c){ //5c = '\' (it's a escape char)
          if (debuggingDDL) printf("Escape char\n");
          ++word;
          if (*word == 0x78 && is_hex_digit(word[1]) && is_hex_digit(word[2])){ //78 = x , so is a escape-char with 2 hex-digit
            value = *word; // get the value of the character
          }
          else{
            switch (*word){
            case 0x22: //22 = " means double quote 0x22
            case 0x27: //27 = ' means Single quote 0x27
            case 0x3F: //3f = ? means question mark 0x3f
            case 0x5C: //5c = \ means blackslash 0x5c
              value = *word;
              break;
            case 0x61: //61 = a means bell 0x07
              value = 0x07;
              break;
            case 0x62: //62 = b means backspace 0x08
              value = 0x08;
              break;
            case 0x66: //66 = f means formfeed 0x0c
              value = 0x0c;
              break;
            case 0x6E: //6e = n means newline 0x0a
              value = 0x0a;
              break;
            case 0x72: //72 = r means carriage return 0x0d
              value = 0x0d;
              break;
            case 0x74: //74 = t means horizontal tab 0x09
              value = 0x09;
              break;
            case 0x76: //76 = v means vertical tab 0x0b
              value = 0x0b;
              break;
            case 0x78: //78 = x means is a especial escape char
              if (is_hex_digit(word[1]) && is_hex_digit(word[2])){
                value = (word[1] - (word[1]<0x39 ? '0' : (word[1]<0x60 ? 'A' : 'a'))) * 16 + (word[2] - (word[2]<0x39 ? '0' : (word[2]<0x60 ? 'A' : 'a')));
              }
              else{
                printf("Error with the escpace char, it's not a hexadecimal!\n");
              }
              break;
            default:
              printf("Error with the escape char!\n");
              return false;
              break;
            }
          }
        }
        else{ //If it's not a escape char, just copy the char
          value = *word;
        }


        if (debuggingDDL) printf("%c\n", value);
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a reference
      /// @param  value   it returns the value of the reference
      /// @param  word    this is a pointer to the beginning of the word
      /// @param  size    this is the size of the word readed
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_value_reference(int &ref, char *word, int size){
        if (debuggingDDL) printf("Reading the reference: ");
        
        // A reference can be the identificator or name 'null'
        if (size == 4){ //null size is 4
          if (word[0] == 'n' && word[1] == 'u' && word[2] == 'l' && word[3] == 'l'){
            ref = -1;
          }
        }
        // If it's not the null value...
        else{
          // it can be also a name
          if (*word == 0x44){ // 44 = $, that means it's a global name
            //first step is to look if the global name exists
            //if exist, add the same pointer
            //if it does not exist, pointer = NULL
          }
          else if (*word == 0x45){ // 45 = %, that means it's a local name
            //first step is to look if the local name exists
            //if exist, add the same pointer
            //if it does not exist, pointer = NULL
          }
          // followed optionally for some identifiers
          string new_word;
          while (*currentChar != 0x7d && *currentChar != 0x2c){ //While the current char is not the } or , there must be more references to this reference
            if (*currentChar == 0x25){
              get_next_char();
              remove_comments_whitespaces();
              new_word = read_word();
              if (debugging) printf(" %s ", new_word);
              get_next_char();
              remove_comments_whitespaces();
            }
            else{
              if (debugging) printf(" %c ", *currentChar);
              remove_comments_whitespaces(); 
            }
          }
        }

        if (debuggingDDL) printf("Reference with id: %i\n", ref);
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will check if it's a type
      /// @param  value   it returns the value of the type
      /// @param  word    this is a pointer to the beginning of the word
      /// @param  size    this is the size of the word readed
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool get_value_data_type(int &type, char *word, int size){
        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This functions process the name, and add it to the application
      /// @param  structure   This is a pointer to the structure that will have this name
      /// @return   The ID of the name
      ////////////////////////////////////////////////////////////////////////////////
      int process_name(openDDL_structure * father_structure){
        char *name;
        int size_name = read_word_size();
        name = new char[size_name];
        for (int i = 0; i < size_name; ++i){
          name[i] = tempChar[i];
        }
        if (debugging) printf("It's the name %s<!!\n", name);
        
        int nameID;
        if (*name == 0x24){ //It's a global name
          nameID = names_.get_index(name);
          if (nameID < 0){
            names_[name] = current_structure;
            current_structure->set_nameID(names_.get_index(name));
          }
          else{
            printf("This global name already exists!\n");
            return -1;
          }
        }
        else if (*name == 0x25){ //It's a local name
          nameID = father_structure->get_index(name);
          if (nameID < 0){
            father_structure->add_name(name,current_structure);
            current_structure->set_nameID(father_structure->get_index(name));
          }
          else{
            printf("This global name already exists!\n");
            return -1;
          }
        }

        if (debugging) printf( (nameID < 0) ? "And it does not exist!\n" : "And it exists!\n" );
        return nameID;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This functions process one single property
      /// @param  new_property  it's a pointer to a openDDL_properties, it will be use to store the content of the property
      /// @param  type_known    This is an int with the data_type expected. If it's not known it will be -1   
      /// @return   True if everything went right, and false if something went wrong
      ///     This function is able to detect the type of the data that the property has inside, anyway there is
      ///     the possibility to add that information as "type_known". If it has the correct value that the property
      ///     is expecting it will store properly the property. If it's not correct, it will print an error reading the file
      ////////////////////////////////////////////////////////////////////////////////
      bool process_single_property(openDDL_properties * new_property, int type_known = -1){
        string word;

        //Now it has to find a identifier
        word = read_word();
        int type = is_identifier(word);
        new_property->identifierID = type;
        //Now check if it's a correct property
        if (type < 0){
          printf("\n\nERROR: It's not a proper property!!!\n");
          return false;
        }
        remove_comments_whitespaces();

        //Now it has to find an =
        if (*currentChar != 0x3D){
          printf("\nERROR: It was expecting a '=' and find %c instead.\n", *currentChar);
          return false;
        }
        get_next_char();
        remove_comments_whitespaces();

        //Not it has to find a literal, that might be (bool, int, float, string, ref or type)
        int size;
        read_data_property(size);

        // first check if the type is known or unkonw. 
        if (type_known = -1){ //If it's unkown try to identify it
          // Check if it's a string
          if (*tempChar == 0x22){
            //Get ready the data to store the size
            char * new_string = new char[size];
            int new_size;
            // Obtain the string from the property
            get_string_literal(new_string, new_size, (char*) tempChar, size);
            // Set new property with the new value as string!
            new_property->literal.value_type = value_type_DDL::STRING;
            new_property->literal.value.string_ = new_string;
            new_property->literal.size_string_ = new_size;
          }
          //Check if it's a data_type
          else{
            int type = 1;
            if (type >= 0){
              // Set new property with the new value as data_type
              new_property->literal.value_type = value_type_DDL::TYPE;
              new_property->literal.value.type_ = type;
            }
          //Check if it's a reference
            else{
              type = 1;
              if (type >= 0){
                // Set new property with the new value as reference
                new_property->literal.value_type = value_type_DDL::REF;
                new_property->literal.value.ref_ = type;
              }
              //Check if it's a bool
              else{
                bool bool_value;
                if (get_bool_literal(bool_value, (char*)tempChar, size)){
                  // Set new property with the new value as bool!
                  new_property->literal.value_type = value_type_DDL::BOOL;
                  new_property->literal.value.bool_ = bool_value;
                }
              }
            }
          }
          //Check if it's a integer or float
        }
        // This means that the type is known!!
        else{
          char * new_string;
          //process accordinglt to the type
          new_property->literal.value_type = (value_type_DDL)type_known;
          switch (type_known){
          case value_type_DDL::UINT:
            break;
          case value_type_DDL::INT:
            break;
          case value_type_DDL::BOOL:
            bool bool_value;
            get_bool_literal(bool_value, (char*)tempChar, size);
            new_property->literal.value.bool_ = bool_value;
            break;
          case value_type_DDL::FLOAT:
            break;
          case value_type_DDL::STRING:
            //Get ready the data to store the size
            new_string = new char[size];
            int new_size;
            // Obtain the string from the property
            get_string_literal(new_string, new_size, (char*)tempChar, size);
            // Set new property with the new value as string!
            new_property->literal.value.string_ = new_string;
            new_property->literal.size_string_ = new_size;
            break;
          case value_type_DDL::REF:
            break;
          case value_type_DDL::TYPE:
            break;
          }
        }



        if (debugging) printf("\n\tCurrent character after the word %s!! %c\n\n", string((char*)tempChar, size), *currentChar);

        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This functions process the properties, and add it to the application
      /// @param  structure   This is a pointer to the structure where to include the properties readed in this function
      /// @return   True if everything went right, and false if something went wrong
      ////////////////////////////////////////////////////////////////////////////////
      bool process_properties(openDDL_identifier_structure * structure){
        bool no_error = true;
        if(debugging) printf("Reading properties!\n");
        get_next_char();
        remove_comments_whitespaces();

        //process the first element
        openDDL_properties * new_property = new openDDL_properties();
        current_structure = structure;
        no_error = process_single_property(new_property);
        structure->add_property(new_property);

        //it will have to expect more properties as long as it's not a )
        while (*currentChar != 0x29 && no_error){ // 0x29 = )
          if (debuggingDDL) printf("More properties!\n");
          //before going on, check that it's a proper list of properties, that's so, it has to have a ,
          if (*currentChar != 0x2C){ // 0x2C = ,   
            printf("\n\nERROR!! It was expecting a ',' and it found a %c instead.\n", *currentChar);
            return false;
          }
          else{
            get_next_char();
            remove_comments_whitespaces();
          }
          //now, keep on processing properties
          new_property = new openDDL_properties();
          current_structure = structure;
          no_error = process_single_property(new_property);
          structure->add_property(new_property);
        }
        get_next_char();
        return no_error;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This functions process the data list, taking into account the type of data expected
      /// @param  type  represent the type of element that it's expecting
      /// @param  size  The size of the word to read (starting with tempChar)
      ////////////////////////////////////////////////////////////////////////////////
      void process_data_list_element(int type, int size){
        bool no_error = true;
        //string word((char*)tempChar, size);
        //Check the type, and call to the appropriate processor function
        switch (type){
        case token_type::tok_bool:
        {
          bool valueBool;
          no_error = get_bool_literal(valueBool, (char*)tempChar, size);
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
          no_error = get_integer_literal(current_literal->value.integer_, (char*)tempChar, size);
          break;
        }
        case token_type::tok_float:
        case token_type::tok_double:
        {
          no_error = get_float_literal(current_literal->value.float_, (char*)tempChar, size);
          break;
        }
        case token_type::tok_string:
        {
          string valueString;
          no_error = get_string_literal(valueString, (char*)tempChar, size);
          break;
        }
        case token_type::tok_ref:
        {
          no_error = get_value_reference(current_literal->value.ref_, (char*)tempChar, size);
          break;
        }
        case token_type::tok_type:
        {
          no_error = get_value_data_type(current_literal->value.type_, (char*)tempChar, size);
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
        int ending, size;

        remove_comments_whitespaces();
        if (*currentChar == 0x7d){ //7d = },  if the next character is }, that means that it's empty!!!
          return true;
        }
        
        // Initializing the new data_list
        current_data_list = new openDDL_data_list;
        //This powerfull line  converts from the numeration given in openDDL_tokes for types to the one given in value_type_DDL
        current_data_list->value_type = (value_type_DDL)((type>8) ? (type - 6) : ((type>4) ? 0 : (type == 0) ? 2 : 1));
        // Reserve some space prior to start
        current_data_list->data_list.reserve(MIN_RESERVING_DATA_LIST);

        //Read the first element, process it and add it to data_list
        current_literal = new openDDL_data_literal;
        ending = read_data_list_element(size);
        process_data_list_element(type, size);
        current_literal->value_type = current_data_list->value_type;
        current_data_list->data_list.push_back(current_literal);

        //If there are more elements...
        while (ending == 1){ //keep on reading while there are more elements
          get_next_char();
          remove_comments_whitespaces();
          //Read next element, process it, and add it to data_list
          current_literal = new openDDL_data_literal();
          ending = read_data_list_element(size);
          process_data_list_element(type, size);
          current_literal->value_type = current_data_list->value_type;
          current_data_list->data_list.push_back(current_literal);
        }
        if (debuggingDDL) printf("\n");

        ((openDDL_data_type_structure *)current_structure)->add_data_list(current_data_list);

        return true;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function has to process the dataarraylist
      /// @param  type  This is the type of the elements that it's reading in the array
      /// @param  arraySize   This is the size of the array (it will be used to make faster the parser process)
      /// @return true if it went ok and false if there was any problem (for instance not finding a })
      ///  This function will keep on checking data until it finds a }. 
      ////////////////////////////////////////////////////////////////////////////////
      bool process_data_array(int type, int arraySize){
        //detect {
        if (*currentChar == 0x2c){ //2c = ,
          get_next_char();
          remove_comments_whitespaces();
        }
        if (*currentChar != 0x7b){ //7b = {
          printf("Problem reading the begining of the data array!!! \n");
          return false;
        }
        //read elements
        int wordSize;
        int ending;
        int itemsLeft = arraySize;
        while (itemsLeft > 0){
          get_next_char();
          remove_comments_whitespaces();
          current_literal = new openDDL_data_literal();
          ending = read_data_list_element(wordSize);
          process_data_list_element(type, wordSize);
          current_literal->value_type = current_data_list->value_type;
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
        value_type_DDL translated_type = (value_type_DDL)((type>8) ? (type - 6) : ((type>4) ? 0 : (type == 0) ? 2 : 1));;
        if (*currentChar != 0x7b){ //7b = {
          printf("Problem reading the begining of the data array list!!! \n");
          return false;
        }
        get_next_char();
        remove_comments_whitespaces();

        while (no_error && *currentChar != 0x7d){
          current_data_list = new openDDL_data_list();
          current_data_list->value_type = translated_type;
          no_error = process_data_array(type, arraySize); //This will have to start with {, read arraySize elements, read }
          get_next_char();
          remove_comments_whitespaces();
          ((openDDL_data_type_structure *)current_structure)->add_data_list(current_data_list);
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
      /// @param  size  This function read the size of the next word and returns it also as a parameter 
      /// @return -1 if error
      /// @return  0 if last element
      /// @return  1 if more elements
      ///     The starting position of the word is tempChar!
      ////////////////////////////////////////////////////////////////////////////////
      int read_data_list_element(int &sizeWord, bool charString = false){
        int to_return = -2;
        sizeWord = 0;
        tempChar = currentChar;
        //If the element to read is a char literal or a string literal, we cannot ignore all whitespaces, so we will
        // have to use a different way to stop it with whitespaces
        if (*currentChar == 0x27 || *currentChar == 0x22){ //27 = ' and 22 = "
          while (*currentChar != 0x2c && *currentChar != 0x7d && !(*currentChar != 0x20 && is_whiteSpace()) && !is_comment()){
            if (debuggingDDLMore) printf("%c, ", *currentChar);
            get_next_char();
            ++sizeWord;
          }
        }
        else{
          while (*currentChar != 0x2c && *currentChar != 0x7d && !is_whiteSpace() && !is_comment()){
            if (debuggingDDLMore) printf("%x, ", currentChar[0]);
            get_next_char();
            ++sizeWord;
          }
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

        return to_return;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will read a data-list element and will return if error or more elements
      /// @param word Pointer to the word, this is the word read
      /// @return -1 if error
      /// @return  0 if last element
      /// @return  1 if more elements
      ////////////////////////////////////////////////////////////////////////////////
      int read_data_property(int &size){
        size = 0;
        tempChar = currentChar;
        while (*currentChar != 0x2C && *currentChar != 0x29 && !is_whiteSpace() && !is_comment()){
          if (debuggingDDLMore) printf("%x, ", *currentChar);
          ++size;
          get_next_char();
        }

        remove_comments_whitespaces();
        if (*currentChar == 0x2C)
          return 1;
        else if (*currentChar == 0x29)
          return 0;
        return -1;
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
        if (debugging) printf("Last symbol-> %x finding %s\n", currentChar[0], temp);
        if (debugging) printf("Finding => %s ", temp);
        return temp;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief It will read and return the size of the next word
      /// @return Integer with the<size of the word
      ///     NOTICE: - This will let "currentChar" at the begining of the next token
      ///             - To use the word, it has to work with tempChar and the size that this
      ///             function returns as an integer
      ////////////////////////////////////////////////////////////////////////////////
      int read_word_size(){
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
        return sizeWord;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will lexer the next words, considering to be a "structureData"
      /// @param  It will receive the type of the data that it has been read
      /// @return True if everthing went well, false if there was some error
      ///     This function will create the structure to be stored in the file!
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_data_type_structure * process_structureData(int type, openDDL_identifier_structure * father){
        bool no_error = true;
        int arraySize;
        if (debugging) printf("\t----TYPE n: %i!!----\n", type);
        //First step is remove whiteSpace and comments
        remove_comments_whitespaces();
        if (debuggingMore) printf("%x\n", currentChar[0]);

        openDDL_data_type_structure * data_type_structure;
        //Then it will read the first character, to see if its a [, or {, or name
        //if name it is a only dataList, so call to process_dataList() and tell that function if has a name or not
        if (*currentChar == 0x5b){ // 5b = [
          if (debugging) printf("It's a data array list!\n");
          //check integer-literal (for a data array list)
          get_next_char();
          arraySize = read_array_size();
          if (debuggingMore) printf("The size is %i\n", arraySize);
          get_next_char();
          remove_comments_whitespaces();

          //it may receive a name (optional)
          int nameID = -1;
          if (is_name()){
            nameID = process_name(father);
            get_next_char();
          }

          // Get ready to create a new structure of data_type
          data_type_structure = new openDDL_data_type_structure(type, arraySize, father, nameID);

          remove_comments_whitespaces();

          //expect a { (if not, error)
          current_structure = data_type_structure;
          if (*currentChar == 0x7b) //7b = {
            no_error = process_data_array_list(type, arraySize);
          else{ //call to process data array list, it will check the }
            no_error = false; //return error
            printf("\n\nERROR: I don't find the data-array-list!\n\n");
          }
        } //ending the [integer-literal] (name) { data-array-list* } option

        //now check the other option (name) { data-list* }
        else{
          int nameID = -1;
          if (is_name()){ // check if there is a name, and process it
            if (debugging) printf("It's a name + data list!\n");
            nameID = process_name(father);
            get_next_char();
          }

          // Get ready to create a new structure of data_type (it has an array size of -1, because it's not array_size
          data_type_structure = new openDDL_data_type_structure(type, -1, father, nameID);

          //After the optional name, it expects a {, and analize the data_list
          if (*currentChar == 0x7b){ // 7b = {
            if (debugging) printf("It's a data list!\n");
            get_next_char();
            remove_comments_whitespaces();
            current_structure = data_type_structure;
            no_error = process_data_list(type);  //expect a } (if not, error)
            if (!no_error) printf("---SOMETHING WENT WRONG WITH DATA LIST\n");
          }
          else{ //if there is no {, ITS AN ERROR!!!
            no_error = false;
            printf("\n\nERROR: I don't find the data-list!\n\n");
          }
        }
        if (debugging) printf("Expect a } ... %c\n", *currentChar);
        
        if (no_error)
          return data_type_structure;
        else
          return NULL;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will lexer the next words, considering to be a "structureIdentifier"
      /// @param  type  It will receive the type of identifier that has been read
      /// @param  father  This is a pointer to the father structure
      /// @return True if everything went well, false if there was some error
      ///     This function will create the structure to be stored in the file!
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_identifier_structure * process_structureIdentifier(int type, openDDL_identifier_structure * father){
        bool no_error = true;
        if (debugging) printf("\t----IDENTIFIER n. %i!!----\n", type);

        //First step is remove whiteSpace and comments
        remove_comments_whitespaces();

        //Then it will read the first character, to see if its a (, or name, or {
        int nameID = -1;
        //If its a name call to something to process name
        if (is_name()){
          nameID = process_name(father);
        }
        //Later, check if it's ( and call something to check properties - telling the function which structure is this one

        //Get ready the new structure
        openDDL_identifier_structure * identifier_structure = new openDDL_identifier_structure(type, father, nameID);

        remove_comments_whitespaces();
        if (*currentChar == 0x28){ // 28 = (
          //call something to check properties          //expect a ) (if not, error)
          no_error = process_properties(identifier_structure);
        }

        remove_comments_whitespaces();
        //Later expect a {, if not return error, and check for a new structure inside this structure
        if (*currentChar == 0x7b){ //7b = {
          get_next_char();
          remove_comments_whitespaces();

          while (*currentChar != 0x7d){ //7d = } (keep on looking for new substructures while it does not find }
            no_error = process_structure(identifier_structure);  //call to process structure
            //Later expect a }, if not return error
            remove_comments_whitespaces();
          }
        }
        else{
          no_error = false;
          printf("\nERROR: No substructure!!!\n\n");
        }
        if (debugging) printf("Expect a } ... %c\n", *currentChar);

        if (no_error)
          return identifier_structure;
        else
          return NULL;
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This function will process the currentChar to look for the next token and study it
      /// @param  father  Is a pointer to the father structure. If there is no father, it's set to NULL
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool process_structure(openDDL_identifier_structure * father = NULL){
        bool no_error = true;
        if (is_end_file()) return true; //If we arrived to the end of the file, let's finish this!
        string word;
        ++nesting;
        if (debugging) printf("\n-----------%x\t%c\n", *currentChar, *currentChar);
        openDDL_structure * processing_structure = NULL;
        // It's a real structure! But it can be IDENTIFIER or DATATYPE
        //remove_comments_whitespaces();
        word = read_word();
        if (debugging) printf("Finding => %s\n", word);
        remove_comments_whitespaces();
        if (debuggingMore) printf("%x <----\n", currentChar[0]);

        //check if it's a type and return it's index (if its negative it's not a type)
        int type = is_dataType(word);
        if (type >= 0){ //As it's a Data type, now it can be single data list or data array list!
          processing_structure = process_structureData(types_.get_value(type),father);
        }

        else{
          //check if it's a identifier and return it's index (if its negative it's not a identifier)
          type = is_identifier(word);
          if (type >= 0){ //As it's a Identifier type, now check name? properties? and then { structure(s)? }
            processing_structure = process_structureIdentifier(identifiers_.get_value(type), father);
          }

          else{ //if it's nothing of the above is an error
            printf("ERRROR!!! There is no real structure.\n");//assert(0 && "It's not a proper structure");
          }
        }
        if (debugging) printf("Expect a } ... %c\n", *currentChar);
        get_next_char();
        --nesting;
        if (processing_structure == NULL)
          no_error = false;
        else{
          if (father == NULL) //Doesn't have a father, then it's a global structure
            openDDL_file.push_back(processing_structure);
          else //It has a father, so add it to the father
            father->add_structure(processing_structure);
          no_error = true;
        }
        return no_error;
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
        nesting = 0;
      }
    public:
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief Constructor of lexer
      ////////////////////////////////////////////////////////////////////////////////
      openDDL_lexer(){}

      ////////////////////////////////////////////////////////////////////////////////
      /// @brief  This will be the function that creates de process of the lexer receiving as parameter the array of characters
      /// @param  It will receive a dynarray of uint8, it will represente the content of the file
      /// @return True if everything went well, false if there was some problem
      ////////////////////////////////////////////////////////////////////////////////
      bool lexer_file(dynarray<uint8_t> file){
        bool no_error = true;
        int completeBufferSize;
        sizeRead = 0;
        buffer = file;
        currentChar = &buffer[0];
        bufferSize = buffer.size();
        completeBufferSize = bufferSize;
        printf("Starting lexer to openDDL process:\n");
        // It's starting to process all the array of characters starting with the first
        // Will do this until the end of the file
        while (!is_end_file() && no_error){
          printf("%4.2f %%\n", 100*((1.0f*(completeBufferSize-bufferSize))/(1.0f*completeBufferSize)));
          remove_comments_whitespaces();
          if (!is_end_file()){
            //Process token (in openDDL is a structure) when you find it
            no_error = process_structure();
            //get new token
            if(debugging) printf("-----------%x\n", *currentChar);
          }
        }
        printf("Finished process.\n");

        return no_error;
      }
    };
  }
}

#endif