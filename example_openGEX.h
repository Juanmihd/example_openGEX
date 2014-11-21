///
/// @file example_openGEX.h
/// @author Juanmi Huertas Delgado
/// @brief This class will do a simple example of a openGEX file imported
/// This class is make from the example_duck from octet

namespace octet {
  /// Scene containing openGEX object with octet.
  class example_openGEX : public app {
    // scene for drawing duck
    ref<visual_scene> app_scene;
    openGEX_loader::openGEX_loader openGEXLoader;
    int num_meshes;
    bool rotate;
    //Camera stuff!
    mouse_ball camera;
  public:
    /// this is called when we construct the class before everything is initialised.
    example_openGEX(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene = new visual_scene();
      rotate= false;
      resource_dict dict;

      if (!openGEXLoader.load_file("assets/openGEX/zombie.txt")){
        printf("Error loading or reading the openGEX file!!\n");
      }

      //This is to call the process. The true and true means that it's taking into account animations (first true) and skin_skeleton (second true)
      else if (!openGEXLoader.process_resources(&dict, false, false)){
        printf("Error processing the openGEX file (openGEX_parser).\n");
      }

      dynarray<resource*> mesh_instances;
      dynarray<resource*> animations;
      dynarray<resource*> materials;
      dict.find_all(mesh_instances, atom_mesh_instance);
      printf("I've found %i mesh instances!\n", mesh_instances.size());
      dict.find_all(animations, atom_animation_instance);
      printf("I've found %i animations!\n", animations.size());
      dict.find_all(materials, atom_material);
      printf("I've found %i materials!\n", materials.size());


      num_meshes = mesh_instances.size();
      int num_animations = animations.size();
      if (num_meshes) {
        for (unsigned int i = 0; i < num_meshes; ++i){
          mesh_instance * current_instance = mesh_instances[i]->get_mesh_instance();
          //print_mat4t(current_instance->get_node()->get_nodeToParent());
          printf("%s?\n", app_utils::get_atom_name(current_instance->get_node()->get_sid()));
          app_scene->add_child(current_instance->get_node());
          app_scene->add_mesh_instance(current_instance);
        }
        
        for (int i = 0; i < num_animations; ++i){
          animation_instance * current_animation = animations[i]->get_animation_instance();
          app_scene->add_animation_instance(current_animation);
        }
        
        scene_node *light_node = new scene_node();
        light *_light = new light();
        _light->set_attenuation(1, 0, -1);
        light_node->rotate(-45, vec3(1, 0, 0));
        light_node->translate(vec3(0, 0, 100));
        app_scene->add_light_instance(new light_instance(light_node, _light));
      }
      app_scene->create_default_camera_and_lights();
      
      app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 0));

      camera.init(this, 1, 100);
    }

    //This function is called to get the keyboard strokes
    void keyboard(){
      if (is_key_down('S')){
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, -2.5, 0);
      }
      else if (is_key_down('W')){
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 2.5, 0);
      }
      if (is_key_down('A')){
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(-2.5, 0, 0);
      }
      else if (is_key_down('D')){
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(2.5, 0, 0);
      }
      if (is_key_down('Q')){
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, -2.5);
      }
      else if (is_key_down('E')){
        app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().translate(0, 0, 2.5);
      }
      else if (is_key_down('R')){
          rotate = true;
      }
      else{
        rotate = false;
      }
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {

      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy, vec4(1.0f, 1.0f, 1.0f, 1.0f));

      //Move the camera with the mouse
      camera.update(app_scene->get_camera_instance(0)->get_node()->access_nodeToParent());
      
      // update matrices. assume 30 fps.
      app_scene->update(1.0f / 30);

      // draw the scene
      app_scene->render((float)vx / vy);
      
      // Updating the screen with the keyboard
      keyboard();
      if (rotate)
        for (int i = 0; i < num_meshes; ++i){
          scene_node *node = app_scene->get_mesh_instance(i)->get_node();
            node->rotate(1, vec3(1, 0, 0));
            node->rotate(1, vec3(0, 1, 0));
        }
    }
  };
}