// try to draw a triangle
#include <gl/gl.h>
#include <stdio.h>
char* readFile(const char* name);
void checkFailure(void);

void gltest(void) {
    
    float vertices[] = { -.5f, -.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  
    
    unsigned int VBO, vertexShader, fragmentShader, VAO, shaderProgram = 0;
    int success = 0;
    char log[512]; // is this long enough?
    char* glVertexSrc = 0; char* glPixelShaderSrc = 0;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);
    
    glGenBuffer(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices-sz, vertices->data, GL_STATIC_DRAW);
    
    glVertexSrc = readFile("test_vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &glVertexSrc, NULL);
    glCompileShader(vertexShader);
    checkFailure();
    
    glPixelShaderSrc = readFile("test_pixel.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkFailure();
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkFailure();
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
    
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    
    
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    
    
    
    
}


