#version 430 core

in vec2 v_pos;
layout( binding = 1, rgba8ui ) uniform uimage2D current;

out vec4 fragment_output;

void main()
{
	fragment_output = imageLoad(current, ivec2(imageSize(current)*(0.5*(v_pos+vec2(1.0)))))/256.0;
}
