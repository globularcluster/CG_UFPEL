#version 330

in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;
uniform float time;

uniform sampler2D sceneBuffer;
uniform sampler2D noiseTex; 
uniform sampler2D maskTex; 
uniform float elapsedTime; // seconds
uniform float luminanceThreshold; // 0.2
uniform float colorAmplification; // 4.0

void main(){

	vec3 finalColor;
	
	vec2 t;           
    t.x = 0.4*sin(elapsedTime*50.0);                                 
    t.y = 0.4*cos(elapsedTime*50.0); 

    float m = texture(maskTex, gl_PointCoord).r;
    vec3 n = texture(noiseTex, (gl_PointCoord*3.5) + t).rgb;
    vec3 c = texture(renderedTexture, UV + (n.xy*0.005)).rgb;

    float lum = dot(vec3(0.30, 0.59, 0.11), c);
    if (lum < luminanceThreshold)
        c *= colorAmplification;

    vec3 visionColor = vec3(0.1, 0.95, 0.2);
    finalColor.rgb = (c + (n * 0.2)) * visionColor; //* m;

	//color = texture( renderedTexture, UV + 0.005*vec2( sin(time+1024.0*UV.x),cos(time+768.0*UV.y)) ).xyz ;
	color = vec4(finalColor, 1.0);
    
}