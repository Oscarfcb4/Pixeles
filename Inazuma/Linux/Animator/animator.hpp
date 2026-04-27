#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "./bone.hpp"
#include "./animation.hpp"

struct Animator{
    Animator(){
        aCurrentTime = 0.0f;
        aFinalBoneMatrices.reserve(100);

        for(int i{}; i < 100; i+=1)
            aFinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    Animator(Animation* anim){
        aCurrentTime = 0.0f;
        aCurrentAnimation = anim;
        aFinalBoneMatrices.reserve(100);

        for(int i{}; i < 100; i+=1)
            aFinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void update(float dt){
        aDeltaTime = dt;
        if(aCurrentAnimation){
            if(!backward)
                aCurrentTime += aCurrentAnimation->getTickSeconds() * dt;
            else
                aCurrentTime -= aCurrentAnimation->getTickSeconds() * dt;
            if(looped){
                aCurrentTime = static_cast<float>(fmod(aCurrentTime, aCurrentAnimation->getDuration()));
                calculateBoneTransform(&aCurrentAnimation->getRootNode(), glm::mat4(1.0f));
            }else{
                if(backward){
                    if(aCurrentTime > 0.0f){
                        aCurrentTime = static_cast<float>(fmod(aCurrentTime, aCurrentAnimation->getDuration()));
                        calculateBoneTransform(&aCurrentAnimation->getRootNode(), glm::mat4(1.0f));
                    }
                }else{
                    if(aCurrentTime < aCurrentAnimation->getDuration()){
                        aCurrentTime = static_cast<float>(fmod(aCurrentTime, aCurrentAnimation->getDuration()));
                        calculateBoneTransform(&aCurrentAnimation->getRootNode(), glm::mat4(1.0f));
                    }
                }
            }
        }
    }

    void play(Animation* anim){
        aCurrentAnimation = anim;
        if(backward)
            aCurrentTime =  aCurrentAnimation->getDuration();
        else
            aCurrentTime = 0.0f;
    }

    void resume(Animation* anim){
        aCurrentAnimation = anim;
    }

    void rewind(float time){
        if(time < 0.0f)
            time = 0.0f;
        if(time > aCurrentAnimation->getDuration())
            time = aCurrentAnimation->getDuration();
        aCurrentTime = time;
    }

    void loop(){
        aCurrentTime += 0.1f;
        if(aCurrentTime > aCurrentAnimation->getDuration())
            aCurrentTime = 0.0f;
    }

    void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform){
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->trans;

        Bone* bone = aCurrentAnimation->findBone(nodeName);
        if(bone){
            bone->update(aCurrentTime);
            nodeTransform = bone->getLocalTransform();
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;

        auto boneInfoMap = aCurrentAnimation->getBoneIDMap();
        if(boneInfoMap.find(nodeName) != boneInfoMap.end()){
            int idx = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            aFinalBoneMatrices[idx] =  globalTransform * offset;
        }

        for(int i{}; i < node->numChildren; i+=1)
            calculateBoneTransform(&node->children[i], globalTransform);
    }

    std::vector<glm::mat4>& getBoneMatrices(){return aFinalBoneMatrices;};
    Animation* getCurrentAnimation(){return aCurrentAnimation;};
    float getCurrentTime(){return aCurrentTime;};
    float getDeltaTime(){return aDeltaTime;};
    void activateLoop(){looped = true;};
    void disableLoop(){looped = false;};
    void setLoop(bool l){looped = l;};
    void animateBackward(){backward = true;};
    void animateForward(){backward = false;};
    void switchBackward(bool dir){backward = dir;};

    private:
        std::vector<glm::mat4> aFinalBoneMatrices{};
        Animation* aCurrentAnimation{};
        float aCurrentTime{};
        float aDeltaTime{};
        bool looped{true}, backward{};
};