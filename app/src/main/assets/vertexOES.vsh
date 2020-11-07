#version 300 es
//uniform mat4 uMVPMatrix;
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 aTextureCoord;
out vec2 yuvTexCoords;
void main(){
    gl_Position = vPosition;
    gl_PointSize = 10.0;
    yuvTexCoords = aTextureCoord;
}