void gltest(void) {
    
    float vertices[] = { -.5f, -.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };  
    
    unsigned int VBO, vertexShader, fragmentShader, VAO, shaderProgram = 0;
    int success = 0;
    int size;
    char log[512]; // is this long enough?
    const char* glVertexSrc = 0; const char* glPixelShaderSrc = 0;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // should return a char* ptr to the 
    glVertexSrc = readFile("../shaders/test_vertex.glsl", &size);
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &glVertexSrc, NULL);
    glCompileShader(vertexShader);
    checkFailure(vertexShader);
    
    glPixelShaderSrc = readFile("../shaders/test_pixel.glsl", &size);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &glPixelShaderSrc, NULL);
    glCompileShader(fragmentShader);
    checkFailure(fragmentShader);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkFailure(shaderProgram);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
    
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    
    
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    
    
    
    
}
