#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{             
	// 光坐标和FragPos之间的距离（世界空间）
	float lightDistance = length(FragPos.xyz - lightPos);

	// 映射到[0-1]
	lightDistance = lightDistance / far_plane;

	// 写入深度
	gl_FragDepth = lightDistance;
}