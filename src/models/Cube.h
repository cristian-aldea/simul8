#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

using glm::mat4;
using glm::vec3;

class Cube {
public:
    explicit Cube(GLuint shaderProgram);
    static void loadCube();

    void draw(mat4 parent);
private:
    static unsigned int cubeVAO;
    static unsigned int cubeVBO;
    static unsigned int cubeIBO;
    static const int numVertices = 8;
    static const int numIndices = 36;

    vec3 position;
    GLuint shaderProgram;

};