example_openGEX
===============

Project for the Msc in Computer Games & Entertainment: openGEX improter for Octet framework

Requirements:
-------------
  You need to install Octet Framework. http://sourceforge.net/projects/octetframework/
  Give a look to the example to get to know how to use the importer of openGEX.
note: 
This version of the importer has been tested with the Octet Framework version of Nov 8th.
  
Deploying the importer:
-----------------------
  1) Place the files contained in the folder "loaders" to the folder "octet\src\loaders".
  2) You need to apply the patch "patch_openGEX_0.1.txt" to octet.
 
How to use the importer:
------------------------
  Give a look to the example example_openGEX, to know how to use the importer.
  - To use the example_openGEX, place the visual studio project located in the folder
  example_openGEX into the folder "octet\src\examples".
  - To access to the openGEX files to try those examples, place such files in the 
  folder "octet\src\assets".
  
Current features:
-----------------
This importer is able to load files in the openGEX format to octet:
  - Loading mesh, materials, nodes, skeletons, animations...
  - Loading mesh_instances to octet, with the materials and nodes attached.
  - The patch provides also an easy way to obtain the resources as a dictionary
  being able to locate the mesh, materials, mesh_instances... by name.
  
  
ToDo features:
--------------
  - Loading camera and lights to the scene.
  - Develop a skin shader to octet to be able to visualize the skinned models.
  - Test properly the loading of skinned and skeletons with animations.
  
More info:
----------
  openGEX         http://opengex.org/
  octet           http://sourceforge.net/projects/octetframework/
  Juanmihd        http://uk.linkedin.com/in/juanmihuertas
