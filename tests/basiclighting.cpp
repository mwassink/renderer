Model test(Light* lightOut) {
    //Model model = addModel("../tests/models/tiefighter/star-wars-vader-tie-fighter.obj", 0);
    Model model = addModel("../tests/models/barrel/barrel.obj", "../tests/models/barrel/barrelsquare.bmp", 512, 512 );
    CoordinateSpace objSpace;
    objSpace.origin = Vector3(0, 0, -60);
    objSpace.r = Vector3(-1, 0, 0);
    objSpace.s = Vector3(0, -1, 0);
    objSpace.t = Vector3(0, 0, 1);
    model.modelSpace = objSpace;
    model.mesh.diffuseColor = Vector3(0.6f, 0.6f, 0.6f);
    model.mesh.specColor = Vector3(1.0f, 1.0f, 1.0f);
    
    Light light;
    light.worldSpaceCoord = Vector3(0.0f, 0.0f, -70.0f);
    light.color = Vector3(1.0f, 1.0f, 1.0f);
    light.irradiance = 50.0f;
    activateModel(&model);
    shadeLightBasic(&model,&light);
    *lightOut = light;
    return model;
}
