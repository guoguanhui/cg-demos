#version 120

#ifdef BLUR_Diffusion
	uniform sampler2D StretchTex;
#endif

uniform sampler2D InputTex;	// inputTex �C Texture being convolved  
uniform float GaussWidth; // Scale �C Used to widen Gaussian taps.  GaussWidth should be the standard deviation. 
uniform vec2 Step; // Blur direction conbined with pixel size

void main()
{ 
#ifdef BLUR_Diffusion
	vec2 stretch = texture2D( StretchTex, gl_TexCoord[0].xy).xy;  
	float netFilterWidth = GaussWidth * stretch.x;
#else
	#ifdef BLUR_Stretch
		vec2 stretch = texture2D( InputTex, gl_TexCoord[0].xy).xy;  
		float netFilterWidth = GaussWidth * stretch.x;;
	#else
		float netFilterWidth = GaussWidth;
	#endif
#endif

	 // Gaussian curve �C standard deviation of 1.0  
    float weights[7] = float[7](0.006,0.061,0.242,0.383,0.242,0.061,0.006);  
    vec2 coords = gl_TexCoord[0].xy - Step * netFilterWidth * 3.0; 
    vec4 sum = vec4(0.0);  
    for( int i = 0; i < 7; i++ )  
    {  
		vec4 tap = texture2D( InputTex,  coords );  
	    sum += weights[i] * tap;  
		coords += Step * netFilterWidth;  
    }  

	gl_FragColor = sum;
}