#pragma once
#include <map>

struct TModel;
struct TR_Model;

struct AssimpNodeData{
    glm::mat4 trans{};
    std::string name{};
    int numChildren{};
    std::vector<AssimpNodeData> children{};
};

struct Animation{
    Animation() = default;

    Animation(const std::string& path, TR_Model* model){
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        aiAnim = scene->mAnimations[0];
        aDuration = static_cast<float>(aiAnim->mDuration);
        aTickSecond = static_cast<float>(aiAnim->mTicksPerSecond);
        gt = scene->mRootNode->mTransformation;
        readHeirarchyData(aRootNode, scene->mRootNode);
        readMissingBones(aiAnim, model);
    }

    ~Animation(){}

    void setAnimation(const std::string& path, TR_Model* model){
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        aiAnim = scene->mAnimations[0];
        aDuration = static_cast<float>(aiAnim->mDuration);
        aTickSecond = static_cast<float>(aiAnim->mTicksPerSecond);
        gt = scene->mRootNode->mTransformation;
        readHeirarchyData(aRootNode, scene->mRootNode);
        readMissingBones(aiAnim, model);
    }

    Bone* findBone(const std::string& name){
        auto it = std::find_if(aBones.begin(), aBones.end(),
            [&] (const Bone& b){
                return b.getBoneName() == name;
            }
        );
        if(it == aBones.end()) return nullptr;
        else return &(*it);
    }

    inline float getTickSeconds(){return aTickSecond;};

    inline float getDuration(){return aDuration;};

    inline std::vector<Bone>& getBones(){return aBones;};

    inline const AssimpNodeData& getRootNode(){return aRootNode;};

    inline const std::map<std::string, _BoneInfo>& getBoneIDMap(){return aBoneInfoMap;};

    aiMatrix4x4 gt{};

    private:
        void readMissingBones(const aiAnimation* anim, TR_Model* model);

        void readHeirarchyData(AssimpNodeData& dest, const aiNode* src){
            assert(src);

            dest.name = src->mName.data;
            dest.trans = AssimpConverter::matToGLM(src->mTransformation);
            dest.numChildren = src->mNumChildren;

            for(unsigned int i{}; i < src->mNumChildren; i+=1){
                AssimpNodeData newData{};
                readHeirarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }

        float aDuration{};
        float aTickSecond{};
        aiAnimation* aiAnim{};
        std::vector<Bone> aBones{};
        AssimpNodeData aRootNode{};
        std::map<std::string, _BoneInfo> aBoneInfoMap{};
};