#pragma once

struct AssimpConverter{
    static inline glm::mat4 matToGLM(const aiMatrix4x4& mat){
        glm::mat4 m{};
        m[0][0] = mat.a1;  m[1][0] = mat.a2;  m[2][0] = mat.a3;  m[3][0] = mat.a4;
        m[0][1] = mat.b1;  m[1][1] = mat.b2;  m[2][1] = mat.b3;  m[3][1] = mat.b4;
        m[0][2] = mat.c1;  m[1][2] = mat.c2;  m[2][2] = mat.c3;  m[3][2] = mat.c4;
        m[0][3] = mat.d1;  m[1][3] = mat.d2;  m[2][3] = mat.d3;  m[3][3] = mat.d4;
        return m;
    }

    static inline glm::vec3 vecToGLM(const aiVector3D& vec){
        return glm::vec3(vec.x, vec.y, vec.z);
    }

    static inline glm::quat quatToGLM(const aiQuaternion& quadOrientarion){
        return glm::quat(quadOrientarion.w, quadOrientarion.x, quadOrientarion.y, quadOrientarion.z);
    }
};