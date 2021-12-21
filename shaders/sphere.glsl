#version 450 core
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D imageOut;
uniform float radius;
uniform vec3 center;
uniform vec3 colorSphere;
// position from the POV of the eye, so eye is (0, 0, 0)
// ray is o + dt
// 30 degrees FOV
#define TAN15  0.26794919243
void main() {
	
	ivec2 dimensions = imageSize(imageOut);
	ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
	float r2 = radius * radius;
	float z = dimensions.y/2 / TAN15; // 30 degree FOV
	float x = (float(dimensions.x/-2 + coords.x));
	float y = (float(dimensions.y/-2 + coords.y));
	vec3 d = vec3(x, y, z);
	d = normalize(d);
	float dProjLength= dot(d, center);
	float ll = dot(center, center);
	if (dProjLength < 0 && dot(center, center) < r2) {
		vec4 emptyPixel = vec4(0.0, 1.0, 0.0, 1.0);
		imageStore(imageOut, coords, emptyPixel);
	} else {
		float m2 = ll - dProjLength * dProjLength;
		if (m2 > r2) {
			vec4 emptyPixel = vec4(1.0, 0.0, 1.0, 1.0);
			imageStore(imageOut, coords, emptyPixel);
		} else {
			float q = sqrt(r2 - m2);
			//float t = dProjLength + q; //inside the sphere 
			//t = dProjLength - q;  outside the sphere
			vec4 colorNew = vec4(colorSphere, 1.0);
			imageStore(imageOut, coords, colorNew);
		}

	}
    if (dProjLength < 0) {
        dProjLength = dProjLength * -1.0;
    }
    //vec4 col = vec4(d , 1.0);
    //vec4 col = vec4(dProjLength / 100.0, 0.0, 0.0, 1.0);
    //imageStore(imageOut, coords, col);

}
