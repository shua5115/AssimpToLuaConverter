#include "lua_converter.hpp"

#include <iomanip>
#include <unordered_map>
#include <vector>

using namespace std;

static const char *NL = "\n";

static inline const char *b2str(bool b) {
    return b ? "true" : "false";
}

namespace AssimpToLua {

void convert(std::ostream &os, const string &str) {
    os << quoted(str);
}

void convert(std::ostream &os, const aiScene *scene) {
    os << "{" << NL;

    os << "name=";
    convert(os, scene->mName.C_Str());
    os << ';' << NL;

    // flags
    unsigned int flags = scene->mFlags;
    os << "flags={" << NL;
    os << "incomplete=" << b2str((flags & AI_SCENE_FLAGS_INCOMPLETE) != 0) << ';' << NL;
    os << "validated=" << b2str((flags & AI_SCENE_FLAGS_VALIDATED) != 0) << ';' << NL;
    os << "warning=" << b2str((flags & AI_SCENE_FLAGS_VALIDATION_WARNING) != 0) << ';' << NL;
    os << "nonverbose=" << b2str((flags & AI_SCENE_FLAGS_NON_VERBOSE_FORMAT) != 0) << ';' << NL;
    os << "terrain=" << b2str((flags & AI_SCENE_FLAGS_TERRAIN) != 0) << ';' << NL;
    os << "allow_shared=" << b2str((flags & AI_SCENE_FLAGS_ALLOW_SHARED) != 0) << ';' << NL;
    os << "};" << NL;
    os.flush();

    // Nodes

    // first, store the tree of nodes in a vector by performing a breadth-first iteration
    size_t nodestack_i = 0;
    vector<const aiNode *> nodelist;  // used as stack
    unordered_map<const aiNode *, size_t> node_indices;

    nodelist.push_back(scene->mRootNode);
    while (nodestack_i < nodelist.size()) {
        const aiNode *current = nodelist[nodestack_i];
        node_indices[current] = nodestack_i;
        for (unsigned int i = 0; i < current->mNumChildren; i++) {
            nodelist.push_back(current->mChildren[i]);
        }
        nodestack_i += 1;
    }
    
    os << "nodes={";
    for (const aiNode *node : nodelist) {
        os << '{' << NL;
        os << "name=";
        convert(os, node->mName.C_Str());
        os << ';' << NL;
        os << "transform=";
        convert(os, &node->mTransformation);
        os << ';' << NL;
        if (node->mMetaData != nullptr) {
            os << "metadata=";
            convert(os, node->mMetaData);
            os << ';' << NL;
        }
        os << "meshes={";
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            unsigned int idx = node->mMeshes[i];
            os << idx << ", ";
        }
        os << "};" << NL;
        if (node->mParent != nullptr) {
            os << "parent=" << node_indices.at(node->mParent) << ';' << NL;
        }
        os << "children={";
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            const aiNode *child = node->mChildren[i];
            os << node_indices.at(child) << ", ";
        }
        os << "};" << NL;
        os << "}," << NL;  // end node
        // os.flush();
    }
    os << "};" << NL;

    // meshes
    os << "meshes={" << NL;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[i];
        convert(os, mesh);
        os << ',' << NL;
    }
    os << "};" << NL;
    os.flush();
    // material
    os << "materials={" << NL;
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        auto mat = scene->mMaterials[i];
        convert(os, mat);
        os << ',' << NL;
    }
    os << "};" << NL;
    os.flush();
    // textures
    os << "textures={" << NL;
    for (unsigned int i = 0; i < scene->mNumTextures; i++) {
        convert(os, scene->mTextures[i]);
        os << ',' << NL;
    }
    os << "};" << NL;
    os.flush();
    // skeletons
    os << "skeletons={" << NL;
    for (unsigned int i = 0; i < scene->mNumSkeletons; i++) {
        convert(os, scene->mSkeletons[i]);
        os << ';' << NL;
    }
    os << "};" << NL;
    os.flush();
    // animation
    os << "animations={" << NL;
    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
        convert(os, scene->mAnimations[i]);
        os << ';' << NL;
    }
    os << "};" << NL;
    os.flush();
    // lights

    // cameras

    // metadata
    os << "metadata=";
    convert(os, scene->mMetaData);
    os << ';' << NL;

    os << '}';
    os.flush();
}

void convert(std::ostream &os, const aiMesh *mesh) {
    os << '{' << NL;
    os << "name=";
    convert(os, mesh->mName.C_Str());
    os << ";" << NL;

    os << "aabb=";
    convert(os, &mesh->mAABB);
    os << ";" << NL;

    os << "primitives={" << NL;
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_POINT) != 0) {
        os << "point=true;" << NL;
    }
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_LINE) != 0) {
        os << "line=true;" << NL;
    }
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) != 0) {
        os << "triangle=true;" << NL;
    }
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_POLYGON) != 0) {
        os << "polygon=true;" << NL;
    }
    if ((mesh->mPrimitiveTypes & aiPrimitiveType_NGONEncodingFlag) != 0) {
        os << "ngon_encoded=true;" << NL;
    }
    os << "};" << NL;

    if (mesh->mNumAnimMeshes > 0) {
        os << "anim_meshes={" << NL;
        for (unsigned int i = 0; i < mesh->mNumAnimMeshes; i++) {
            convert(os, mesh->mAnimMeshes[i]);
            os << ',' << NL;
        }
        os << "};" << NL;
    }

    if (mesh->mNumBones > 0) {
        os << "bones={" << NL;
        for (unsigned int i = 0; i < mesh->mNumBones; i++) {
            convert(os, mesh->mBones[i]);
            os << ',' << NL;
        }
        os << "};" << NL;
    }

    if (mesh->mColors != nullptr) {
        os << "color_channels={" << NL;
        for (unsigned int i = 0; i < mesh->GetNumColorChannels(); i++) {
            if (mesh->HasVertexColors(i)) {
                os << '{' << NL;
                auto col_array = mesh->mColors[i];
                for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                    convert(os, col_array + j);
                    os << ',' << NL;
                }
                os << "}," << NL;
            } else {
                os << "false," << NL;
            }
        }
        os << "};" << NL;
    }

    os << "faces={" << NL;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        convert(os, &mesh->mFaces[i]);
        os << ',' << NL;
    }
    os << "};" << NL;

    os << "material_index=" << mesh->mMaterialIndex << ';' << NL;
    os << "morph_method=" << mesh->mMethod << ';' << NL;

    if (mesh->mNormals != nullptr) {
        os << "normals={" << NL;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            convert(os, &mesh->mNormals[i]);
            os << ',' << NL;
        }
        os << "};" << NL;
    }

    if (mesh->mTangents != nullptr) {
        os << "tangents={" << NL;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            convert(os, &mesh->mTangents[i]);
            os << ',' << NL;
        }
        os << "};" << NL;
    }

    if (mesh->mBitangents != nullptr) {
        os << "bitangents={" << NL;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            convert(os, &mesh->mBitangents[i]);
            os << ',' << NL;
        }
        os << "};" << NL;
    }

    if (mesh->mTextureCoords != nullptr) {
        os << "texture_coords={" << NL;
        for (unsigned int i = 0; i < mesh->GetNumUVChannels(); i++) {
            if (mesh->HasTextureCoords(i)) {
                os << '{' << NL;
                os << "name=";
                convert(os, mesh->mTextureCoordsNames[i]->C_Str());
                os << ';' << NL;
                auto uv_array = mesh->mTextureCoords[i];
                for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                    convert(os, uv_array + j);
                    os << ',' << NL;
                }
                os << "}," << NL;
            } else {
                os << "false," << NL;
            }
        }
        os << "};" << NL;
    }

    os << "vertices={" << NL;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        convert(os, &mesh->mVertices[i]);
        os << ',' << NL;
    }
    os << "};" << NL;

    os << '}';
}

void convert(std::ostream &os, const aiFace *face) {
    os << '{';
    for (unsigned int i = 0; i < face->mNumIndices; i++) {
        os << face->mIndices[i] << ", ";
    }
    os << '}';
}

void convert(std::ostream &os, const aiAnimMesh *mesh) {
    os << '{' << NL;
    os << "name=";
    convert(os, mesh->mName.C_Str());
    os << ";" << NL;

    if (mesh->mColors != nullptr) {
        os << "color_channels={" << NL;
        for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; i++) {
            if (mesh->HasVertexColors(i)) {
                os << '{' << NL;
                auto col_array = mesh->mColors[i];
                for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                    convert(os, col_array + j);
                    os << ',' << NL;
                }
                os << "}," << NL;
            } else {
                os << "false," << NL;
            }
        }
        os << "};" << NL;
    }

    if (mesh->mNormals != nullptr) {
        os << "normals={" << NL;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            convert(os, &mesh->mNormals[i]);
            os << ',' << NL;
        }
        os << "};" << NL;
    }

    if (mesh->mTangents != nullptr) {
        os << "tangents={" << NL;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            convert(os, &mesh->mTangents[i]);
            os << ',' << NL;
        }
        os << "};" << NL;
    }

    if (mesh->mBitangents != nullptr) {
        os << "bitangents={" << NL;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            convert(os, &mesh->mBitangents[i]);
            os << ',' << NL;
        }
        os << "};" << NL;
    }

    if (mesh->mTextureCoords != nullptr) {
        os << "texture_coords={" << NL;
        for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++) {
            if (mesh->HasTextureCoords(i)) {
                os << '{' << NL;
                auto uv_array = mesh->mTextureCoords[i];
                for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
                    convert(os, uv_array + j);
                    os << ',' << NL;
                }
                os << "}," << NL;
            } else {
                os << "false," << NL;
            }
        }
        os << "};" << NL;
    }
    if (mesh->mVertices != nullptr) {
        os << "vertices={" << NL;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            convert(os, &mesh->mVertices[i]);
            os << ',' << NL;
        }
    }
    os << "};" << NL;

    os << '}';
}

void convert(std::ostream &os, const aiAABB *aabb) {
    os << '{' << NL;
    os << "min={" << aabb->mMin.x << ", " << aabb->mMin.y << ", " << aabb->mMin.z << "};" << NL;
    os << "max={" << aabb->mMax.x << ", " << aabb->mMax.y << ", " << aabb->mMax.z << "};" << NL;
    os << '}';
}

void convert(std::ostream &os, const aiNode *node) {
    os << '{' << NL;
    os << "name=";
    convert(os, node->mName.C_Str());
    os << ';' << NL;
    os << "transform=";
    convert(os, &node->mTransformation);
    os << ';' << NL;
    os << "metadata=";
    convert(os, node->mMetaData);
    os << ';' << NL;
    os << "meshes={";
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        unsigned int idx = node->mMeshes[i];
        os << idx << ", ";
    }
    os << "};" << NL;
    os << '}';
}

void convert(std::ostream &os, const aiMaterial *mat) {
    os << '{' << NL;
    // mapping from material property key to index in material properties array
    for (unsigned int i = 0; i < mat->mNumProperties; i++) {
        os << '[';
        convert(os, mat->mProperties[i]->mKey.C_Str());
        os << "]=" << i << ';' << NL;
    }
    for (unsigned int i = 0; i < mat->mNumProperties; i++) {
        convert(os, mat->mProperties[i]);
        os << ',' << NL;
    }
    os << '}';
}

void convert(std::ostream &os, const aiMaterialProperty *prop) {
    os << '{' << NL;

    os << "name=";
    convert(os, prop->mKey.C_Str());
    os << ';' << NL;

    os << "index=" << prop->mIndex << ';' << NL;

    os << "type=";
    switch (prop->mType) {
        case aiPropertyTypeInfo::aiPTI_Float:
        case aiPropertyTypeInfo::aiPTI_Double:
        case aiPropertyTypeInfo::aiPTI_Integer:
            convert(os, "number");
            break;
        case aiPropertyTypeInfo::aiPTI_String:
            convert(os, "string");
            break;
        default:
            convert(os, "raw");
            break;
    }
    os << ';' << NL;

    os << "texture_type=";
    switch (prop->mSemantic) {
        case aiTextureType_DIFFUSE:
            convert(os, "diffuse");
            break;
        case aiTextureType_SPECULAR:
            convert(os, "specular");
            break;
        case aiTextureType_AMBIENT:
            convert(os, "ambient");
            break;
        case aiTextureType_EMISSIVE:
            convert(os, "emissive");
            break;
        case aiTextureType_HEIGHT:
            convert(os, "height");
            break;
        case aiTextureType_NORMALS:
            convert(os, "normals");
            break;
        case aiTextureType_SHININESS:
            convert(os, "shininess");
            break;
        case aiTextureType_OPACITY:
            convert(os, "opacity");
            break;
        case aiTextureType_DISPLACEMENT:
            convert(os, "displacement");
            break;
        case aiTextureType_LIGHTMAP:
            convert(os, "lightmap");
            break;
        case aiTextureType_REFLECTION:
            convert(os, "reflection");
            break;
        case aiTextureType_BASE_COLOR:
            convert(os, "base_color");
            break;
        case aiTextureType_NORMAL_CAMERA:
            convert(os, "normal_camera");
            break;
        case aiTextureType_EMISSION_COLOR:
            convert(os, "emission_color");
            break;
        case aiTextureType_METALNESS:
            convert(os, "metalness");
            break;
        case aiTextureType_DIFFUSE_ROUGHNESS:
            convert(os, "diffuse_roughness");
            break;
        case aiTextureType_AMBIENT_OCCLUSION:
            convert(os, "ambient_occlusion");
            break;
        case aiTextureType_SHEEN:
            convert(os, "sheen");
            break;
        case aiTextureType_CLEARCOAT:
            convert(os, "clearcoat");
            break;
        case aiTextureType_TRANSMISSION:
            convert(os, "transmission");
            break;
        case aiTextureType_UNKNOWN:
            convert(os, "unknown");
            break;
        default:
            convert(os, "none");
            break;
    }
    os << ';' << NL;

    if (prop->mDataLength > 0) {
        os << "data_length=" << prop->mDataLength << ';' << NL;
        os << "data=[========[";
        os.write(prop->mData, prop->mDataLength);
        os << "]========];" << NL;
    }

    os << '}';
}

void convert(std::ostream &os, const aiTexture *texture) {
    os << '{' << NL;
    os << "filename=";
    convert(os, texture->mFilename.C_Str());
    os << ';' << NL;
    if (texture->mHeight > 0) {
        os << "format=";
        convert(os, "rgba8");
        os << ';' << NL;
        os << "width=" << texture->mWidth << ';' << NL;
        os << "height=" << texture->mHeight << ';' << NL;
        const unsigned int size = texture->mWidth * texture->mHeight;
        os << "data=[========[";
        for (unsigned int i = 0; i < size; i++) {
            auto texel = texture->pcData[i];
            const unsigned char pixel_rgba[4] = {texel.r, texel.g, texel.b, texel.a};
            os.write((const char *)pixel_rgba, 4);  // send unsigned char as signed char to preserve bits
        }
        os << "]========];" << NL;
    } else {
        os << "format=";
        convert(os, texture->achFormatHint);
        os << ';' << NL;
        os << "data=[========[";
        os.write((const char *)texture->pcData, texture->mWidth);
        os << "]========];" << NL;
    }
    os << '}';
}

void convert(std::ostream &os, const aiAnimation *anim) {
    os << '{' << NL;
    os << "name=";
    convert(os, anim->mName.C_Str());
    os << ';' << NL;
    os << "duration=" << anim->mDuration << ';' << NL;
    os << "fps=" << anim->mTicksPerSecond << ';' << NL;
    os << "node_anims={";
    for (unsigned int i = 0; i < anim->mNumChannels; i++) {
        auto c = anim->mChannels[i];
        convert(os, c);
        os << ',' << NL;
    }
    os << "};" << NL;
    os << "mesh_anims={";
    for (unsigned int i = 0; i < anim->mNumMeshChannels; i++) {
        auto c = anim->mMeshChannels[i];
        convert(os, c);
        os << ',' << NL;
    }
    os << "};" << NL;
    os << "morph_mesh_anims={";
    for (unsigned int i = 0; i < anim->mNumMorphMeshChannels; i++) {
        auto c = anim->mMorphMeshChannels[i];
        convert(os, c);
        os << ',' << NL;
    }
    os << "};" << NL;
    os << '}';
}

void convert(std::ostream &os, const aiAnimBehaviour behavior) {
    switch (behavior) {
        case aiAnimBehaviour_CONSTANT:
            convert(os, "constant");
            break;
        case aiAnimBehaviour_LINEAR:
            convert(os, "linear");
            break;
        case aiAnimBehaviour_REPEAT:
            convert(os, "repeat");
            break;
        default:
            convert(os, "default");
            break;
    }
}

void convert(std::ostream &os, const aiNodeAnim *anim) {
    os << '{' << NL;
    os << "node_name=";
    convert(os, anim->mNodeName.C_Str());
    os << ';' << NL;
    os << "pre_state=";
    convert(os, anim->mPreState);
    os << ';' << NL;
    os << "post_state=";
    convert(os, anim->mPostState);
    os << ';' << NL;
    os << "position_times={" << NL;
    for (unsigned int i = 0; i < anim->mNumPositionKeys; i++) {
        auto key = anim->mPositionKeys[i];
        os << key.mTime << ',' << NL;
    }
    os << "};" << NL;
    os << "position_keys={" << NL;
    for (unsigned int i = 0; i < anim->mNumPositionKeys; i++) {
        auto key = anim->mPositionKeys[i];
        convert(os, &key.mValue);
        os << ',' << NL;
    }
    os << "};" << NL;
    os << "rotation_times={" << NL;
    for (unsigned int i = 0; i < anim->mNumRotationKeys; i++) {
        auto key = anim->mRotationKeys[i];
        os << key.mTime << ',' << NL;
    }
    os << "};" << NL;
    os << "rotation_keys={" << NL;
    for (unsigned int i = 0; i < anim->mNumRotationKeys; i++) {
        auto key = anim->mRotationKeys[i];
        convert(os, &key.mValue);
        os << ',' << NL;
    }
    os << "};" << NL;
    os << "scale_times={" << NL;
    for (unsigned int i = 0; i < anim->mNumScalingKeys; i++) {
        auto key = anim->mScalingKeys[i];
        os << key.mTime << ',' << NL;
    }
    os << "};" << NL;
    os << "scale_keys={" << NL;
    for (unsigned int i = 0; i < anim->mNumScalingKeys; i++) {
        auto key = anim->mScalingKeys[i];
        convert(os, &key.mValue);
        os << ',' << NL;
    }
    os << "};" << NL;
    os << '}';
}

void convert(std::ostream &os, const aiMeshAnim *anim) {
    os << '{' << NL;
    os << "mesh_name=";
    convert(os, anim->mName.C_Str());
    os << ';' << NL;
    os << "times={" << NL;
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        auto key = anim->mKeys[i];
        os << key.mTime << ',' << NL;
    }
    os << "};" << NL;
    os << "keys={" << NL;
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        auto key = anim->mKeys[i];
        os << key.mValue << ',' << NL;
    }
    os << "};" << NL;
    os << '}';
}

void convert(std::ostream &os, const aiMeshMorphAnim *anim) {
    os << '{' << NL;
    os << "mesh_name=";
    convert(os, anim->mName.C_Str());
    os << ';' << NL;
    os << "times={" << NL;
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        auto key = anim->mKeys[i];
        os << key.mTime << ',' << NL;
    }
    os << "};" << NL;
    os << "value_keys={" << NL;
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        auto key = anim->mKeys[i];
        os << '{';
        for (unsigned int j = 0; j < key.mNumValuesAndWeights; j++) {
            os << key.mValues[j] << ", ";
        }
        os << "}," << NL;
    }
    os << "};" << NL;
    os << "weight_keys={" << NL;
    for (unsigned int i = 0; i < anim->mNumKeys; i++) {
        auto key = anim->mKeys[i];
        os << '{';
        for (unsigned int j = 0; j < key.mNumValuesAndWeights; j++) {
            os << key.mWeights[j] << ", ";
        }
        os << "}," << NL;
    }
    os << "};" << NL;
    os << '}';
}

void convert(std::ostream &os, const aiSkeleton *skely) {
    os << '{' << NL;
    os << "name=";
    convert(os, skely->mName.C_Str());
    os << ';' << NL;
    os << "bones={" << NL;
    for (unsigned int i = 0; i < skely->mNumBones; i++) {
        convert(os, skely->mBones[i]);
        os << ',' << NL;
    }
    os << "};" << NL;
    os << '}';
}

void convert(std::ostream &os, const aiSkeletonBone *bone) {
    os << '{' << NL;
    os << "parent=" << bone->mParent << ';' << NL;
    os << "matrix=";
    convert(os, &bone->mLocalMatrix);
    os << ';' << NL;
    os << "offset=";
    convert(os, &bone->mOffsetMatrix);
    os << ';' << NL;
    os << "vertex_ids={";
    for (unsigned int i = 0; i < bone->mNumnWeights; i++) {
        auto w = bone->mWeights[i];
        os << w.mVertexId << ", ";
    }
    os << "};" << NL;
    os << "weights={";
    for (unsigned int i = 0; i < bone->mNumnWeights; i++) {
        auto w = bone->mWeights[i];
        os << w.mWeight << ", ";
    }
    os << "};" << NL;
    // TODO reference node and mesh stuff
    // bone->mArmature
    // bone->mNode
    // bone->mMeshId;
    os << '}';
}

void convert(std::ostream &os, const aiBone *bone) {
    os << '{' << NL;
    os << "name=";
    convert(os, bone->mName.C_Str());
    os << ';' << NL;
    os << "offset=";
    convert(os, &bone->mOffsetMatrix);
    os << ';' << NL;
    os << "weight_idxs={";
    for (unsigned int i = 0; i < bone->mNumWeights; i++) {
        os << bone->mWeights[i].mVertexId << ", ";
    }
    os << "};" << NL;
    os << "weights={";
    for (unsigned int i = 0; i < bone->mNumWeights; i++) {
        os << bone->mWeights[i].mWeight << ", ";
    }
    os << "};" << NL;
    // TODO: reference node index
    // bone->mArmature;
    // bone->mNode;
    os << '}';
}

void convert(std::ostream &os, const aiLight *light) {
    os << "TODO";
}

void convert(std::ostream &os, const aiCamera *camera) {
    os << "TODO";
}

void convert(std::ostream &os, const aiMetadata *metadata) {
    os << '{' << NL;
    for (unsigned int i = 0; i < metadata->mNumProperties; i++) {
        os << '[';
        convert(os, metadata->mKeys[i].C_Str());
        os << "]=";
        convert(os, &metadata->mValues[i]);
        os << ';' << NL;
    }
    os << '}';
}

void convert(std::ostream &os, const aiMetadataEntry *entry) {
    if (entry->mData == nullptr) {
        os << "nil";
    }
    switch (entry->mType) {
        case aiMetadataType::AI_BOOL:
            os << b2str(*((bool *)entry->mData));
            break;
        case aiMetadataType::AI_INT32:
            os << *((int32_t *)entry->mData);
            break;
        case aiMetadataType::AI_UINT64:
            os << *((uint64_t *)entry->mData);
            break;
        case aiMetadataType::AI_FLOAT:
            os << *((float *)entry->mData);
            break;
        case aiMetadataType::AI_DOUBLE:
            os << *((double *)entry->mData);
            break;
        case aiMetadataType::AI_AISTRING:
            convert(os, ((const aiString *)entry->mData)->C_Str());
            break;
        case aiMetadataType::AI_AIVECTOR3D:
            convert(os, (const aiVector3D *)entry->mData);
            break;
        case aiMetadataType::AI_AIMETADATA:
            convert(os, (const aiMetadata *)entry->mData);  // recursion, yay!
            break;
        default:
            os << "nil";
            break;
    }
}

void convert(std::ostream &os, const aiMatrix4x4 *mat4) {
    os << '{';
    os << mat4->a1 << ", " << mat4->a2 << ", " << mat4->a3 << ", " << mat4->a4 << ", ";
    os << mat4->b1 << ", " << mat4->b2 << ", " << mat4->b3 << ", " << mat4->b4 << ", ";
    os << mat4->c1 << ", " << mat4->c2 << ", " << mat4->c3 << ", " << mat4->c4 << ", ";
    os << mat4->d1 << ", " << mat4->d2 << ", " << mat4->d3 << ", " << mat4->d4 << '}';
}

void convert(std::ostream &os, const aiMatrix3x3 *mat3) {
    os << '{';
    os << mat3->a1 << ", " << mat3->a2 << ", " << mat3->a3 << ", ";
    os << mat3->b1 << ", " << mat3->b2 << ", " << mat3->b3 << ", ";
    os << mat3->c1 << ", " << mat3->c2 << ", " << mat3->c3 << '}';
}

void convert(std::ostream &os, const aiVector3D *vec3) {
    os << '{' << vec3->x << ", " << vec3->y << ", " << vec3->z << '}';
}

void convert(std::ostream &os, const aiVector2D *vec2) {
    os << '{' << vec2->x << ", " << vec2->y << '}';
}

void convert(std::ostream &os, const aiQuaternion *quat) {
    os << '{' << quat->x << ", " << quat->y << ", " << quat->z << ", " << quat->w << '}';
}

void convert(std::ostream &os, const aiColor4D *col4) {
    os << '{' << col4->r << ", " << col4->g << ", " << col4->b << ", " << col4->a << '}';
}

void convert(std::ostream &os, const aiColor3D *col3) {
    os << '{' << col3->r << ", " << col3->g << ", " << col3->b << '}';
}

}  // namespace AssimpToLua