#version 300 es

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec2 aTextureCoord;

out vec2 yuvTexCoords;

uniform mat4 uMVPMatrix;

void main(){
    gl_Position =  uMVPMatrix * vec4(vPosition,0.0f,1.0f);
    //gl_Position =  vec4(vPosition,0.0,1.0);
    gl_PointSize = 10.0;
    yuvTexCoords = aTextureCoord;
}