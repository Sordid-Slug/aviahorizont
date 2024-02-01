//Данный вариант создавался под микроконтроллер, где потребление памяти было критически важно, оттого так много if-else
#define M_PI 3.14159265358979323846

#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


const GLchar* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 position;\n"

	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 projection;\n"

	"void main() {\n"
	"	gl_Position = projection * view * model * vec4(position, 1.0f);\n"
	"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
    "out vec4 color;\n"

    "void main() {\n"
    "   color = vec4(0.6f, 0.4f, 0.2f, 1.0f);\n"
    "}\0";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Some kind window error\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        printf("GLEW is not OK\n");
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glewInit();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);  
    glViewport(0, 0, width, height);

    GLint success;
    GLchar infolog[512];
    GLuint shaderProgram = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); 

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		printf("%s\n", infolog);
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
		printf("%s\n", infolog);
	}

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		printf("%s\n", infolog);
	}
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLfloat background[] = {
        0.5f, 0.75f, 0.0f,   // center
        0.0f, 0.75f, 0.0f,   // left
        0.0f, 0.0f, 0.0f,   // left bottom
        0.5f, 0.0f, 0.0f,   // bottom
        1.0f, 0.0f, 0.0f,   // right bottom
        1.0f, 0.75f, 0.0f    // right
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(background), background, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    float curAngle = 0, tangage = 0;
    GLfloat timevalue = 0, center[2] = {0.5f, 0.1f}; // в background тоже поменяй
    background[1] = center[1];

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.0f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        timevalue = glfwGetTime();

        mat4x4 model;
        mat4x4 view;
        mat4x4 projection;

        mat4x4_identity(model);
        mat4x4_identity(view);
        mat4x4_identity(projection);
        mat4x4_ortho(projection, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (const GLfloat*)(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(background), background, GL_STATIC_DRAW);

        curAngle = fmod(curAngle, 2 * M_PI);
        curAngle = curAngle < 0 ? 2 * M_PI + curAngle : curAngle;

        float yx0 = center[1] - center[0] * tan(curAngle);
        float yx1 = center[1] + (1 - center[0]) * tan(curAngle);
        float xy0 = center[0] - center[1] / tan(curAngle);
        float xy1 = center[0] + (1.0f - center[1]) / tan(curAngle);

        // 0.5f, 0.5f, 0.0f,   // center        0,1
        // 0.0f, 0.5f, 0.0f,   // left          3,4
        // 0.0f, 0.0f, 0.0f,   // left bottom   6,7
        // 0.5f, 0.5f, 0.0f,   // bottom        9, 10
        // 1.0f, 0.0f, 0.0f,   // right bottom  12, 13
        // 1.0f, 0.5f, 0.0f    // right         15, 16

        if (curAngle < (M_PI / 4)) {
            background[3] = 0.0f;
            background[4] = yx0;

            background[6] = 0.0f;
            background[7] = 0.0f;

            background[9] = 0.0f;
            background[10] = 0.0f;

            background[12] = 1.0f;
            background[13] = 0.0f;

            background[15] = 1.0f;
            background[16] = yx1;

            if (yx0 > 1) {
                background[3] = 1.0f;
                background[4] = yx1;

                background[6] = 1.0f;
                background[7] = 1.0f;

                background[9] = 1.0f;
                background[10] = 1.0f;

                background[12] = 1.0f;
                background[13] = 1.0f;

                background[15] = xy1;
                background[16] = 1.0f;
            } else if (yx1 < 0) {
                background[3] = xy0;
                background[4] = 0.0f;

                background[6] = 1.0f;
                background[7] = 0.0f;

                background[9] = 1.0f;
                background[10] = 1.0f;

                background[12] = 0.0f;
                background[13] = 1.0f;

                background[15] = 0.0f;
                background[16] = yx0;
            } else if (yx0 < 0) {
                background[3] = xy0;
                background[4] = 0.0f;

                background[6] = 1.0f;
                background[7] = 0.0f;

                background[9] = 1.0f;
                background[10] = 0.0f;

                background[12] = 1.0f;
                background[13] = 0.0f;

                background[15] = 1.0f;
                background[16] = yx1;
            } else if (yx1 > 1) {
                background[3] = 0.0f;
                background[4] = yx0;

                background[6] = 0.0f;
                background[7] = 0.0f;

                background[9] = 1.0f;
                background[10] = 0.0f;

                background[12] = 1.0f;
                background[13] = 1.0f;

                background[15] = xy1;
                background[16] = 1.0f;
            }

        } else if (curAngle >= (M_PI / 4) && curAngle < (3 * M_PI / 4)) {
            background[3] = xy0;
            background[4] = 0.0f;

            background[6] = 1.0f;
            background[7] = 0.0f;

            background[9] = 1.0f;
            background[10] = 0.0f;

            background[12] = 1.0f;
            background[13] = 1.0f;

            background[15] = xy1;
            background[16] = 1.0f;

            if (yx0 < 0 && yx1 <= 1 && yx1 >= 0) {
                background[3] = xy0;
                background[4] = 0.0f;

                background[6] = 1.0f;
                background[7] = 0.0f;

                background[9] = 1.0f;
                background[10] = 0.0f;

                background[12] = 1.0f;
                background[13] = 0.0f;

                background[15] = 1.0f;
                background[16] = yx1;
            } else if (yx1 >= 1 && yx0 >= 0 && yx0 <= 1) {
                background[3] = 0.0f;
                background[4] = yx0;

                background[6] = 0.0f;
                background[7] = 0.0f;

                background[9] = 1.0f;
                background[10] = 0.0f;

                background[12] = 1.0f;
                background[13] = 1.0f;

                background[15] = xy1;
                background[16] = 1.0f;
            } else if (yx0 >= 1 && yx1 <= 1 && yx1 >= 0) {
                background[3] = 1.0f;
                background[4] = yx1;

                background[6] = 1.0f;
                background[7] = 1.0f;

                background[9] = 1.0f;
                background[10] = 1.0f;

                background[12] = 1.0f;
                background[13] = 1.0f;

                background[15] = xy1;
                background[16] = 1.0f;
            } else if (yx1 < 0 && yx0 <= 1 && yx0 >= 0) {
                background[3] = xy0;
                background[4] = 0.0f;

                background[6] = 1.0f;
                background[7] = 0.0f;

                background[9] = 1.0f;
                background[10] = 1.0f;

                background[12] = 0.0f;
                background[13] = 1.0f;

                background[15] = 0.0f;
                background[16] = yx0;
            }
        } else if (curAngle >= (3 * M_PI / 4) && curAngle < (5 * M_PI / 4)) {
            background[3] = 1.0f;
            background[4] = yx1;

            background[6] = 1.0f;
            background[7] = 1.0f;

            background[9] = 1.0f;
            background[10] = 1.0f;

            background[12] = 0.0f;
            background[13] = 1.0f;

            background[15] = 0.0f;
            background[16] = yx0;

            if (yx1 < 0 && yx0 >= 0 && yx1 <= 1) {
                background[3] = xy0;
                background[4] = 0.0f;

                background[6] = 1.0f;
                background[7] = 0.0f;

                background[9] = 1.0f;
                background[10] = 1.0f;

                background[12] = 0.0f;
                background[13] = 1.0f;

                background[15] = 0.0f;
                background[16] = yx0;
            } else if (yx0 >= 1 && yx1 >= 0 && yx1 <= 1) {
                background[3] = 1.0f;
                background[4] = yx1;

                background[6] = 1.0f;
                background[7] = 1.0f;

                background[9] = 1.0f;
                background[10] = 1.0f;

                background[12] = 1.0f;
                background[13] = 1.0f;

                background[15] = xy1;
                background[16] = 1.0f;
            } else if (yx1 >= 1 && yx0 >= 0 && yx0 <= 1) {
                background[3] = xy1;
                background[4] = 1.0f;

                background[6] = 0.0f;
                background[7] = 1.0f;

                background[9] = 0.0f;
                background[10] = 1.0f;

                background[12] = 0.0f;
                background[13] = 1.0f;

                background[15] = 0.0f;
                background[16] = yx0;
            } else if (yx0 < 0 && yx1 >= 0 && yx1 <= 1) {
                background[3] = 1.0f;
                background[4] = yx1;

                background[6] = 1.0f;
                background[7] = 1.0f;

                background[9] = 0.0f;
                background[10] = 1.0f;

                background[12] = 0.0f;
                background[13] = 0.0f;

                background[15] = xy0;
                background[16] = 0.0f;
            }
        } else if (curAngle >= (5 * M_PI / 4) && curAngle < (7 * M_PI / 4)) {
            background[3] = xy1;
            background[4] = 1.0f;

            background[6] = 0.0f;
            background[7] = 1.0f;

            background[9] = 0.0f;
            background[10] = 0.0f;

            background[12] = 0.0f;
            background[13] = 0.0f;

            background[15] = xy0;
            background[16] = 0.0f;

            if (yx0 < 0 && yx1 <= 1 && yx1 >= 0) {
                background[3] = 1.0f;
                background[4] = yx1;

                background[6] = 1.0f;
                background[7] = 1.0f;

                background[9] = 0.0f;
                background[10] = 1.0f;

                background[12] = 0.0f;
                background[13] = 0.0f;

                background[15] = xy0;
                background[16] = 0.0f;
                
            } else if (yx1 >= 1 && yx0 >= 0 && yx0 <= 1) {
                background[3] = xy1;
                background[4] = 1.0f;

                background[6] = 0.0f;
                background[7] = 1.0f;

                background[9] = 0.0f;
                background[10] = 1.0f;

                background[12] = 0.0f;
                background[13] = 1.0f;

                background[15] = 0.0f;
                background[16] = yx0;
            } else if (yx0 >= 1 && yx1 <= 1 && yx1 >= 0) {
                background[3] = xy1;
                background[4] = 1.0f;

                background[6] = 0.0f;
                background[7] = 1.0f;

                background[9] = 0.0f;
                background[10] = 0.0f;

                background[12] = 1.0f;
                background[13] = 0.0f;

                background[15] = 1.0f;
                background[16] = yx1;
            } else if (yx1 < 0 && yx0 >= 0 && yx0 <= 1) {
                background[3] = 0.0f;
                background[4] = yx0;

                background[6] = 0.0f;
                background[7] = 0.0f;

                background[9] = 0.0f;
                background[10] = 0.0f;

                background[12] = 0.0f;
                background[13] = 0.0f;

                background[15] = xy0;
                background[16] = 0.0f;
            }
        } else if (curAngle >= (7 * M_PI / 4)) {
            background[3] = 0.0f;
            background[4] = yx0;

            background[6] = 0.0f;
            background[7] = 0.0f;

            background[9] = 1.0f;
            background[10] = 0.0f;

            background[12] = 1.0f;
            background[13] = 0.0f;

            background[15] = 1.0f;
            background[16] = yx1;

            if (yx1 <= 0) {
                background[3] = 0.0f;
                background[4] = yx0;

                background[6] = 0.0f;
                background[7] = 0.0f;

                background[9] = 0.0f;
                background[10] = 0.0f;

                background[12] = 0.0f;
                background[13] = 0.0f;

                background[15] = xy0;
                background[16] = 0.0f;
            } else if (yx0 >= 1) {
                background[3] = xy1;
                background[4] = 1.0f;

                background[6] = 0.0f;
                background[7] = 1.0f;

                background[9] = 0.0f;
                background[10] = 0.0f;

                background[12] = 1.0f;
                background[13] = 0.0f;

                background[15] = 1.0f;
                background[16] = yx1;
            }
        }

        glfwSwapBuffers(window);

        curAngle = 5 * sin(timevalue);
        center[1] = 0.5 + (0.5 * sin(timevalue));
        background[1] = center[1];
    }
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
