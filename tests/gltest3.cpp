CoordinateSpace cameraSpace;
CoordinateSpace objSpace;
GLuint program;
glTriangleNames names;
void doDraw(Mesh& mesh) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    f32 znear = 10.0f;
    f32 zfar = 500.0f;
    Matrix4 mvp = glModelViewProjection(objSpace, cameraSpace, 3.14f/6.0f, 16.0f/9.0f,znear, zfar );

    // Look for the uniform location
    int mvpLoc = glGetUniformLocation( program, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&mvp.data[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names.ebo);
    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
}

const float vertices[] = {
    -0.5f, -0.5f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 1.0f,
     0.0f,  0.5f, 0.0f, 1.0f,
};

void addThings(f32* verts, int numVerts);

Mesh gltest3(void) {
    
    const char* s = "../tests/models/tiefighter/star-wars-vader-tie-fighter.obj";
    Mesh mesh = loadMesh(s, 0);
    program = setShaders("../shaders/test_vertex3.glsl", "../shaders/test_pixel3.glsl");
    glUseProgram(program);
    
    cameraSpace.origin = Vector3(0, 0, 40);
    cameraSpace.r = Vector3(1, 0, 0);
    cameraSpace.s = Vector3(0, -1, 0);
    cameraSpace.t = Vector3(0, 0, -1);

    
    objSpace.origin = Vector3(0, 0, 60);
    objSpace.r = Vector3(1, 0, 0);
    objSpace.s = Vector3(0, 1, 0);
    objSpace.t = Vector3(0, 0, 1);
    f32 zfar = 500.0f;
    f32 znear = 10.0f;
    Matrix4 mvp = glModelViewProjection(objSpace, cameraSpace, 3.14f/6.0f, 16.0f/9.0f,znear, zfar );

    // Look for the uniform location
    int mvpLoc = glGetUniformLocation( program, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (f32*)&mvp.data[0]);
    printf("First 10 normalized device coords: \n");
    
    for (int i = 0; i < 10; ++i) {
        Vector4 v(mesh.vertices[i].coord.x, mesh.vertices[i].coord.y, mesh.vertices[i].coord.z, mesh.vertices[i].coord.w);
        v = mvp * v;
        printf("%f %f %f %f\n", v.x, v.y, v.z, v.w);
    }

    addBasicTexturedVerticesToShader(mesh.vertices, mesh.triangles, mesh.numVertices, mesh.numIndices, 0, 1, 2, &names );
    //addThings((float*) vertices, 3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, names.ebo);
    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
    return mesh;
    
    
}
