#version 330

#include "Common.include.glsl"

in float oDepth;
in vec2 oTex;

out vec4 FragColor;

void main()
{
#ifndef SHADOW_VSM
	FragColor = vec4(oDepth, oTex, 1.0);
#else	
	// Adjusting moments (this is sort of bias per pixel) using partial derivative
	float dx = dFdx(oDepth);
	float dy = dFdy(oDepth);
	float depthSquared = oDepth * oDepth +0.25*(dx*dx+dy*dy) ;
		
	FragColor = vec4(oDepth, oTex, depthSquared);
#endif
}
