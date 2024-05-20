#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
void main()
{
	TexCoords = aPos;
	vec4 pos = projection * view * vec4(aPos, 1.0);
	//gl_Position = pos.xyzw;//天空盒第一个渲染

	gl_Position = pos.xyww; //天空盒最后一个渲染：使用early-z技术
	
}
