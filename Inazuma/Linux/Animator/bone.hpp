#pragma once
#include <cstdint>

struct KeyPosition{
    glm::vec3 pos{};
    float time{};
};

struct KeyRotation{
    glm::quat orientation{};
    float time{};
};

struct KeyScale{
    glm::vec3 scale{};
    float time{};
};

struct Bone{
    Bone(const std::string& name, int id, aiNodeAnim* channel){
        bName = name;
        bID = id;
        bLocalTransform = glm::mat4(1.0f);
        bNumPositions = channel->mNumPositionKeys;
        for(int indx{}; indx < bNumPositions; indx+=1){
            aiVector3D aiPosition{channel->mPositionKeys[indx].mValue};
            float time{static_cast<float>(channel->mPositionKeys[indx].mTime)};
            KeyPosition data{};
            data.pos = AssimpConverter::vecToGLM(aiPosition);
            data.time = time;
            bPositions.push_back(data);
        }

        bNumRotations = channel->mNumRotationKeys;
        for(int indx{}; indx < bNumRotations; indx+=1){
            aiQuaternion aiOrientation{channel->mRotationKeys[indx].mValue};
            float time{static_cast<float>(channel->mRotationKeys[indx].mTime)};
            KeyRotation data{};
            data.orientation = AssimpConverter::quatToGLM(aiOrientation);
            data.time = time;
            bRotations.push_back(data);
        }

        bNumScales = channel->mNumScalingKeys;
        for(int indx{}; indx < bNumScales; indx+=1){
            aiVector3D aiScale{channel->mScalingKeys[indx].mValue};
            float time{static_cast<float>(channel->mScalingKeys[indx].mTime)};
            KeyScale data{};
            data.scale = AssimpConverter::vecToGLM(aiScale);
            data.time = time;
            bScales.push_back(data);
        }
    }

    void update(float timeStamp){
        glm::mat4 translation = interpolatePosition(timeStamp);
        glm::mat4 rotation = interpolateRotation(timeStamp);
        glm::mat4 scale = interpolateScale(timeStamp);
        bLocalTransform = translation * rotation * scale;
    }

    int getPositionIndex(float animationTime){
        for(int i{}; i < bNumPositions - 1; i+=1){
            if(animationTime < bPositions[i + 1].time)
                return i;
        }
        assert(0);
    }

    int getRotationIndex(float animationTime){
        for(int i{}; i < bNumRotations - 1; i+=1){
            if(animationTime < bRotations[i + 1].time)
                return i;
        }
        assert(0);
    }

    int getScaleIndex(float animationTime){
        for(int i{}; i < bNumScales - 1; i+=1){
            if(animationTime < bScales[i + 1].time)
                return i;
        }
        assert(0);
    }

    glm::mat4 getLocalTransform(){return bLocalTransform;};
    std::string getBoneName() const {return bName;};
    int getBoneID(){return bID;};

    private:
        float getScaleFactor(float last, float next, float animationTime){
            float midWayLength{animationTime - last};
            float framesDifference{next - last};
            return midWayLength / framesDifference;
        }

        glm::mat4 interpolatePosition(float animationTime){
            if(bNumPositions == 1)
                return glm::translate(glm::mat4(1.0f), bPositions[0].pos);
            
            int p0Index = getPositionIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = getScaleFactor(bPositions[p0Index].time, bPositions[p1Index].time, animationTime);
            glm::vec3 finalPosition = glm::mix(bPositions[p0Index].pos, bPositions[p1Index].pos, scaleFactor);
            return glm::translate(glm::mat4(1.0f), finalPosition);
        }

        glm::mat4 interpolateRotation(float animationTime){
            if(bNumRotations == 1)
                return glm::toMat4(glm::normalize(bRotations[0].orientation));
            
            int p0Index =  getRotationIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = getScaleFactor(bRotations[p0Index].time, bRotations[p1Index].time, animationTime);
            glm::quat finalRotation = glm::slerp(bRotations[p0Index].orientation, bRotations[p1Index].orientation, scaleFactor);
            finalRotation = glm::normalize(finalRotation);
            return glm::toMat4(finalRotation);
        }

        glm::mat4 interpolateScale(float animationTime){
            if(bNumScales == 1)
                return glm::scale(glm::mat4(1.0f), bScales[0].scale);
            
            int p0Index = getScaleIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = getScaleFactor(bScales[p0Index].time, bScales[p1Index].time, animationTime);
            glm::vec3 finalScale = glm::mix(bScales[p0Index].scale, bScales[p1Index].scale, scaleFactor);
            return glm::scale(glm::mat4(1.0f), finalScale);
        }

        std::vector<KeyPosition> bPositions{};
        std::vector<KeyRotation> bRotations{};
        std::vector<KeyScale> bScales{};
        int bNumPositions{};
        int bNumRotations{};
        int bNumScales{};

        glm::mat4 bLocalTransform{};
        std::string bName{};
        int bID{};
};