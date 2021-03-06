#include "utils.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>

#include "globals.h"
#include "../external/stb_image.h"

using std::cerr;
using std::endl;
using std::exit;

namespace s8 {
    /**
     * Initialize glfw, glad and window.
     */
    GLFWwindow *init() {
        GLFWwindow *window;
        if (!glfwInit()) {
            cerr << "Failed to initialize GLFW" << endl;
            exit(1);
        }

        glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

        // Open a window and create its OpenGL context
        window = glfwCreateWindow(s8::WINDOW_WIDTH, s8::WINDOW_HEIGHT, "s8", nullptr, nullptr);
        if (window == nullptr) {
            cerr << "Failed to open GLFW window. glfwCreateWindow returned a nullptr." << endl;
            glfwTerminate();
            exit(1);
        }
        glfwMakeContextCurrent(window); // Initialize GLEW

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            cerr << "Failed to initialize OpenGL context" << endl;
            exit(1);
        }

        return window;
    }

    GLuint loadShaders(const char *vertexFilePath, const char *fragmentFilePath) {

        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // Read the Vertex Shader code from the file
        std::string VertexShaderCode;
        std::ifstream VertexShaderStream(vertexFilePath, std::ios::in);
        if (VertexShaderStream.is_open()) {
            std::stringstream sstr;
            sstr << VertexShaderStream.rdbuf();
            VertexShaderCode = sstr.str();
            VertexShaderStream.close();
        } else {
            printf("Impossible to open %s. Make sure the file is present in the same directory as the program!\n",
                   vertexFilePath);
            getchar();
            return 0;
        }

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode;
        std::ifstream FragmentShaderStream(fragmentFilePath, std::ios::in);
        if (FragmentShaderStream.is_open()) {
            std::stringstream sstr;
            sstr << FragmentShaderStream.rdbuf();
            FragmentShaderCode = sstr.str();
            FragmentShaderStream.close();
        }

        GLint Result = GL_FALSE;
        int InfoLogLength;

        // Compile Vertex Shader
        printf("Compiling shader : %s\n", vertexFilePath);
        char const *VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        // Compile Fragment Shader
        printf("Compiling shader : %s\n", fragmentFilePath);
        char const *FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            printf("%s\n", &FragmentShaderErrorMessage[0]);
        }

        // Link the program
        printf("Linking program\n");
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        // Check the program
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDetachShader(ProgramID, VertexShaderID);
        glDetachShader(ProgramID, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return ProgramID;
    }

    GLuint loadTexture(const char *textureFilePath) {
        GLuint texture;

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        int width, height, nChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(textureFilePath, &width, &height, &nChannels, 0);

        if (!data) {
            throw std::runtime_error("Failed to load texture data");
        }


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        return texture;

    }

    void setUniform(GLuint shaderProgram, mat4 value, const char *name) {
        glUseProgram(shaderProgram);
        GLint location = glGetUniformLocation(shaderProgram, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }

    void setUniform(GLuint shaderProgram, vec3 value, const char *name) {
        glUseProgram(shaderProgram);
        GLint location = glGetUniformLocation(shaderProgram, name);
        glUniform3fv(location, 1, &value[0]);
    }

    void setUniform(GLuint shaderProgram, vec2 value, const char *name) {
        glUseProgram(shaderProgram);
        GLint location = glGetUniformLocation(shaderProgram, name);
        glUniform2fv(location, 1, &value[0]);
    }

    void setUniform(GLuint shaderProgram, int value, const char *name) {
        glUseProgram(shaderProgram);
        GLint location = glGetUniformLocation(shaderProgram, name);
        glUniform1i(location, value);
    }

    // TODO, use better randomness
    float rng(float min, float max) {
        return ((float) std::rand() / (float) RAND_MAX) * max + min;
    }

    float rng() {
        return rng(0, 1);
    }


}
