#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = aPos;
	ourColor = aColor.rgb;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}