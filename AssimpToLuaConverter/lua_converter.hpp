#ifndef LUA_CONVERTER_H
#define LUA_CONVERTER_H

#include <ostream>

#include "assimp/scene.h"

namespace AssimpToLua {

void convert(std::ostream& os, const std::string& str);
void convert(std::ostream& os, const aiScene* scene);
void convert(std::ostream& os, const aiNode* node);
void convert(std::ostream& os, const aiMesh* mesh);
void convert(std::ostream& os, const aiFace* face);
void convert(std::ostream& os, const aiAnimMesh* animMesh);
void convert(std::ostream& os, const aiAABB* aabb);
void convert(std::ostream& os, const aiMaterial* mat);
void convert(std::ostream& os, const aiMaterialProperty* prop);
void convert(std::ostream& os, const aiTexture* texture);
void convert(std::ostream& os, const aiAnimation* anim);
void convert(std::ostream& os, const aiNodeAnim* anim);
void convert(std::ostream& os, const aiMeshAnim* anim);
void convert(std::ostream& os, const aiMeshMorphAnim* anim);
void convert(std::ostream& os, const aiSkeleton* skely);
void convert(std::ostream& os, const aiSkeletonBone* bone);
void convert(std::ostream& os, const aiBone* bone);
void convert(std::ostream& os, const aiLight* light);
void convert(std::ostream& os, const aiCamera* camera);
void convert(std::ostream& os, const aiMetadata* metadata);
void convert(std::ostream& os, const aiMetadataEntry* entry);
void convert(std::ostream& os, const aiMatrix4x4* mat4);
void convert(std::ostream& os, const aiMatrix3x3* mat3);
void convert(std::ostream& os, const aiVector3D* vec3);
void convert(std::ostream& os, const aiVector2D* vec2);
void convert(std::ostream& os, const aiQuaternion* quat);
void convert(std::ostream& os, const aiColor4D* col4);
void convert(std::ostream& os, const aiColor3D* col3);

}  // namespace AssimpToLua

#endif