//
// Created by lhyzuishuai on 2023/10/19.
//

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
// Program
GLuint ShadowProgamID;
GLuint RenderProgramID;
//VAO
GLuint  VAOID;

//VBO & EBO
GLuint CubeVertexVBOID;  //顶点坐标vbo
GLuint CubeUVVBOID;      //纹理坐标vbo
GLuint EBOID;           //EBO

const GLuint WIDTH = 800;


// Data
// 正方体模型参数
std::vector<glm::vec3> CubeVertex = {
        glm::vec3(-0.5f, -0.5f, -0.5),
        glm::vec3(0.5f, -0.5f, -0.5),
        glm::vec3(0.5f, 0.5f, -0.5),
        glm::vec3(-0.5f, 0.5f, -0.5),
        glm::vec3(-0.5f, -0.5f, 0.5),
        glm::vec3(-0.5f, 0.5f, 0.5),
        glm::vec3(0.5f, 0.5f, 0.5),
        glm::vec3(0.5f, -0.5f, 0.5),
};
std::vector<glm::vec2> CubeUV = {
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (1.0f,0.0f),
        glm::vec2 (1.0f,1.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
        glm::vec2 (0.0f,0.0f),
};
std::vector<float> CubeIndices = {

};
// 矩形模型 地板
std::vector<glm::vec3> Ground;
std::vector<glm::vec2> GroundUV;
/// shadow 相关
// FBO
GLuint DepthFBO = 0; //创建帧缓冲 用来存储第一次变换后的Z缓冲对象

// 光源
glm::vec3 LightPos(1.2f,1.0f,2.0f);

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void LoadSrc()
{
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    //Compiler and Link Shader Program
    ShadowProgamID = LoadShaders("./MyDepth.vert","MyDepth.frag");
    RenderProgramID = LoadShaders("./MyshadowMap.vert","./Myshadow.frag");
   //init VAO
    glGenVertexArrays(1,&VAOID);
    glBindVertexArray(VAOID);

    glGenFramebuffers(1,&DepthFBO);
}

int InitWindow() {
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 1024, "Tutorial 16 - Shadows", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // We would expect width and height to be 1024 and 768
    int windowWidth = 1024;
    int windowHeight = 768;
    // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    return 0;
}

void gameLoop() {
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}




int main( void ) {
    if(InitWindow() == -1){
        return -1;
    }
    LoadSrc();
    gameLoop();

    return 0;
}