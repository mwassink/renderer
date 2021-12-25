// These will test the shadow mapping capabilities of the renderer


Renderer* shadowRenderer = 0;
RendererUtil* shadowRendererUtil = 0;
void testShadow(Array<Model>* models, SpotLight* light ) {
    
    ASSERT(shadowRenderer != 0 && shadowRendererUtil != 0)
    // all these models should be added already
    shadowRenderer->ShadowPass(models->data, light, models->sz );

#if 1
    for (int i = 0; i < models->sz; ++i) {
        shadowRenderer->renderModel(&(*models)[i], light);
    }

#endif
}
