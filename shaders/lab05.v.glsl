#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
// TODO #D: add normal matrix
uniform mat3 normMat;
// TODO #A: add light uniforms
uniform vec3 lightDir;
uniform vec3 lightCol;


uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
// TODO #C: add vertex normal
layout(location = 1) in vec3 vertex;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // TODO #B: computer Light vector
    vec3 Normal = normalize(lightDir* -1);
    // TODO #E: transform normal vector
    //Normal = normMat* Normal;
    vec3 vertexnorm = vertex*normMat;

    // TODO #F: perform diffuse calculation
    vec3 I = lightCol*materialColor* max(dot(Normal, vertexnorm), 0);
    // TODO #G: assign the color for this vertex
    color = I;
}