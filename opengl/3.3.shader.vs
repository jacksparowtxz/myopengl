#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout(std140) uniform BlobSettings{  
    vec4 InnerColor;  
    vec4 OuterColor;  
    float RadiusInner;  
    float RadiusOuter;  
};
out vec3 ourColor;
out vec2 TexCoord;



void main()
{
	gl_Position = vec4(aPos, 1.0)*InnerColor*OuterColor;
	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x*RadiusInner, aTexCoord.y*RadiusOuter);
}