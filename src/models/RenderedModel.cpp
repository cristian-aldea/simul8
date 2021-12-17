#include "RenderedModel.h"

#include "../common/globals.h"

using s8::setUniform;

RenderedModel::RenderedModel(GLuint shader, GLuint texture)
        : shader{shader}, texture{texture}, Model() {}

void RenderedModel::draw(mat4 parent) {
    glBindVertexArray(getVAO());
    glBindBuffer(GL_ARRAY_BUFFER, getVBO());
    glBindTexture(GL_TEXTURE_2D, texture);

    mat4 mvp = parent * getMVPMatrix();

    setUniform(shader, mvp, s8::UNIFORM_MODEL_MATRIX_NAME);

    drawVertices();
}