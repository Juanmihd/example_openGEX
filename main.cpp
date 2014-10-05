///
/// @file main.cpp
/// @author Juanmi Huertas Delgado 
/// @brief This is the main class for the example_openGEX 

///

#include "../../octet.h"

#include "example_openGEX.h"

/// Create a box with octet
int main(int argc, char **argv) {
  // set up the platform.
  octet::app::init_all(argc, argv);

  // our application.
  octet::example_openGEX app(argc, argv);
  app.init();

  // open windows
  octet::app::run_all_apps();
}


