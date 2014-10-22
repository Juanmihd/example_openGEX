//////////////////////////////////////////////////////////////////////////////////////////
///
/// @file openGEX_identifiers.h
/// @author Juanmi Huertas Delgado
/// @brief This will contain the identifiers of the structures and substructures and attributes
///

/// @brief This will be the list of identifiers of openGEX

#ifndef OPENGEX_IDENTIFIERS_INCLUDED
#define OPENGEX_IDENTIFIERS_INCLUDED

namespace octet{
  namespace loaders{
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    struct gex_ident_enum{
      enum ident_type
      {
        id_Animation, 
        id_Atten, 
        id_BoneCountArray, 
        id_BoneIndexArray, 
        id_BoneNode, 
        id_BoneRefArray, 
        id_BoneWeightArray, 
        id_CameraNode, 
        id_CameraObject, 
        id_Color, 
        id_GeometryNode, 
        id_GeometryObject, 
        id_IndexArray, 
        id_Key, 
        id_LightNode, 
        id_LightObject, 
        id_Material, 
        id_MaterialRef, 
        id_Mesh,
        id_Metric,
        id_Morph, 
        id_Name,
        id_Node, 
        id_ObjectRef, 
        id_Param,
        id_Rotation, 
        id_Scale,
        id_Skeleton,
        id_Skin, 
        id_Texture,
        id_Time,
        id_Track,
        id_Transform, 
        id_Translation, 
        id_Value, 
        id_VertexArray,
        prop_attrib,
        prop_begin,
        prop_clip,
        prop_curve,
        prop_end,
        prop_front,
        prop_index,
        prop_key,
        prop_kind,
        prop_lod,
        prop_material,
        prop_morph,
        prop_motion_blur,
        prop_object,
        prop_primitive,
        prop_restart,
        prop_shadow,
        prop_target,
        prop_texcoord,
        prop_two_sided,
        prop_type,
        prop_visible,
        ident_last,
      };
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief This will be the class of ddl tokens
    ////////////////////////////////////////////////////////////////////////////////
    class gex_ident : public gex_ident_enum
    {

    public:
      ////////////////////////////////////////////////////////////////////////////////
      /// @brief This function will return the token_name of the given token identifier
      ////////////////////////////////////////////////////////////////////////////////
      static const string ident_name(unsigned t){
        static const string ident_names[ident_last] = {
          "Animation",      // identificator 00
          "Atten",          // identificator 01
          "BoneCountArray", // identificator 02
          "BoneIndexArray", // identificator 03
          "BoneNode",       // identificator 04
          "BoneRefArray",   // identificator 05
          "BoneWeightArray",// identificator 06
          "CameraNode",     // identificator 07
          "CameraObject",   // identificator 08
          "Color",          // identificator 09
          "GeometryNode",   // identificator 10
          "GeometryObject", // identificator 11
          "IndexArray",     // identificator 12
          "Key",            // identificator 13
          "LightNode",      // identificator 14
          "LightObject",    // identificator 15
          "Material",       // identificator 16
          "MaterialRef",    // identificator 17
          "Mesh",           // identificator 18
          "Metric",         // identificator 19
          "Morph",          // identificator 20
          "Name",           // identificator 21
          "Node",           // identificator 22
          "ObjectRef",      // identificator 23
          "Param",          // identificator 24
          "Rotation",       // identificator 25
          "Scale",          // identificator 26
          "Skeleton",       // identificator 27
          "Skin",           // identificator 28
          "Texture",        // identificator 29
          "Time",           // identificator 30
          "Track",          // identificator 31
          "Transform",      // identificator 32
          "Translation",    // identificator 33
          "Value",          // identificator 34
          "VertexArray",    // identificator 35
          "attrib",         // identificator 36
          "begin",          // identificator 37
          "clip",           // identificator 38
          "curve",          // identificator 39
          "end",            // identificator 40
          "front",          // identificator 41
          "index",          // identificator 42
          "key",            // identificator 43
          "kind",           // identificator 44
          "lod",            // identificator 45
          "material",       // identificator 46
          "morph",          // identificator 47
          "motion_blur",    // identificator 48
          "object",         // identificator 49
          "primitive",      // identificator 50
          "restart",        // identificator 51
          "shadow",         // identificator 52
          "target",         // identificator 53
          "texcoord",       // identificator 54
          "two_sided",      // identificator 55
          "type",           // identificator 56
          "visible"         // identificator 57
        };
        // It will be an error if we try to call a token bigger than our current token list!
        assert((t < ident_last) && "Calling a token bigger than the token list!");
        return ident_names[(int)t];
      }
    };
  }
}


#endif