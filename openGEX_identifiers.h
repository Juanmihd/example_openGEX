//////////////////////////////////////////////////////////////////////////////////////////
///
/// @file openGEX_identifiers.h
/// @author Juanmi Huertas Delgado
/// @brief This will contain the identifiers of the structures and substructures and attributes
///

/// @brief This will be the list of identifiers of openGEX

namespace octet{
  namespace loaders{
    struct gex_ident_enum{
      enum ident_type
      {
        id_Animation, id_Atten, 
        id_BoneCountArray, id_BoneIndexArray, id_BoneNode, id_BoneRefArray, id_BoneWeightArray, 
        id_CameraNode, id_CameraObject, id_Color, 
        id_GeometryNode, id_GeometryObject, 
        id_IndexArray, 
        id_Key, 
        id_LightNode, id_LightObject, 
        id_Material, id_MaterialRef, id_Mesh, id_Metric, id_Morph, 
        id_Name, id_Node, 
        id_ObjectRef, 
        id_Param, id_Rotation, 
        id_Scale, id_Skeleton, id_Skin, 
        id_Texture, id_Time, id_Track, id_Transform, id_Translation, 
        id_Value, id_VertexArray,
        
        ident_last,
      };
    };

    /// @brief This will be the class of ddl tokens
    class gex_ident : public gex_ident_enum
    {

    public:
      /// @brief This function will return the token_name of the given token identifier
      static const string ident_name(unsigned t){
        static const string ident_names[ident_last] = {
          "Animation",
          "Atten",
          "BoneCountArray",
          "BoneIndexArray",
          "BoneNode",
          "BoneRefArray",
          "BoneWeightArray",
          "CameraNode",
          "CameraObject",
          "Color",
          "GeometryNode",
          "GeometryObject",
          "IndexArray",
          "Key",
          "LightNode",
          "LightObject",
          "Material",
          "MaterialRef",
          "Mesh",
          "Metric",
          "Morph",
          "Name",
          "Node",
          "ObjectRef",
          "Param",
          "Rotation",
          "Scale",
          "Skeleton",
          "Skin",
          "Texture",
          "Time Track",
          "Transform",
          "Translation",
          "Value",
          "VertexArray",
        };
        // It will be an error if we try to call a token bigger than our current token list!
        assert((t < ident_last) && "Calling a token bigger than the token list!");
        return ident_names[(int)t];
      }
    };
  }
}

/*
Animation
Atten
BoneCountArray
BoneIndexArray
BoneNode
BoneRefArray
BoneWeightArray
CameraNode
CameraObject
Color
GeometryNode
GeometryObject
IndexArray
Key
LightNode
LightObject
Material
MaterialRef
Mesh
Metric
Morph
Name
Node
ObjectRef
Param
Rotation
Scale
Skeleton
Skin
Texture
Time
Track
Transform
Translation
Value
VertexArray
*/