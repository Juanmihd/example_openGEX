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
    openGEX_loader openGEXLoader;
  public:
    /// this is called when we construct the class before everything is initialised.
    example_openGEX(int argc, char **argv) : app(argc, argv) {
    }

    void print_mesh(mesh * current_mesh){
      int num_vertices = current_mesh->get_num_vertices();
      int num_indices = current_mesh->get_num_indices();
      for (int i = 0; i < num_vertices; ++i){

      }
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene = new visual_scene();

      resource_dict dict;

      if (!openGEXLoader.loadFile("assets/openGEX/cubeGEX_2.txt")){
        printf("It did not work!!");
      }

      if (!openGEXLoader.process_resources(dict)){
        printf("It did not work!!");
      }

      printf("Size of dict is: %i\n", dict.get_size());

      dynarray<resource*> meshes;
      dict.find_all(meshes, atom_mesh);
      if (meshes.size()) {
        material *mat = new material(new image("assets/duckCM.gif"));
        mesh *cube = meshes[0]->get_mesh();
        printf("Size of meshes %i vertices and %i indices\n", cube->get_num_indices(), cube->get_num_vertices());
        scene_node *node = new scene_node();
        node->translate(vec3(-150, -150, 0));
        app_scene->add_child(node);
        app_scene->add_mesh_instance(new mesh_instance(node, cube, mat));

        scene_node *light_node = new scene_node();
        light *_light = new light();
        _light->set_attenuation(1, 0, -1);
        light_node->rotate(-45, vec3(1, 0, 0));
        light_node->translate(vec3(0, 0, 100));
        app_scene->add_light_instance(new light_instance(light_node, _light));
      }
      app_scene->create_default_camera_and_lights();
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f / 30);

      // draw the scene
      app_scene->render((float)vx / vy);

      // tumble the duck  (there is only one mesh instance)
      mesh_instance *mi = app_scene->get_mesh_instance(0);
      if (mi) {
        scene_node *node = mi->get_node();
        node->rotate(1, vec3(1, 0, 0));
        node->rotate(1, vec3(0, 1, 0));
      }
    }
  };
}
