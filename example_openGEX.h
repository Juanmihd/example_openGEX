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
      printf("Size of meshes %i vertices and %i indices\n", num_vertices, num_indices);
      gl_resource::rwlock vlr(current_mesh->get_vertices());
      gl_resource::rwlock ilr(current_mesh->get_indices());
      //Printing vertices
      mesh::vertex *vtx = (mesh::vertex *)vlr.u8();
      for (int i = 0; i < num_vertices; ++i){
        openGEXLoader.lexer.print_vertex(vtx[i]);
      }
      //Printing indices
      uint32_t *itx = ilr.u32();
      int count = 3;
      printf("-->");
      for (int i = 0; i < num_indices; ++i){
        printf("%u, ", itx[i]);
        count--;
        if (count == 0){
          printf("\n->");
          count = 3;
        }
      }
    }

    void print_mat4t(mat4t matrix){
      printf("Matrix:\n");
      printf("%f, %f, %f, %f\n", matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
      printf("%f, %f, %f, %f\n", matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
      printf("%f, %f, %f, %f\n", matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
      printf("%f, %f, %f, %f\n", matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
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

      dynarray<resource*> meshes;
      dynarray<resource*> mesh_instances;
      //dict.find_all(meshes, atom_mesh);
      dict.find_all(mesh_instances, atom_mesh_instance);
      printf("I've found %i meshes!\n", mesh_instances.size());

      if (meshes.size()) {
        mesh_instance * current_instance = mesh_instances[0]->get_mesh_instance();
        print_mat4t(current_instance->get_node()->access_nodeToParent());
        app_scene->add_mesh_instance(current_instance);
        //material *mat = new material(vec4(1,0,0,1));
        //mesh *cube = meshes[0]->get_mesh();
        ////print_mesh(cube);
        //scene_node *node = new scene_node();
        //node->rotate(-90, vec3(1, 0, 0));
        //node->translate(vec3(0, 80, -150));
        //app_scene->add_child(node);
        //app_scene->add_mesh_instance(new mesh_instance(node, cube, mat));

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
        node->rotate(0.5, vec3(0, 0, 1));
      }
    }
  };
}
