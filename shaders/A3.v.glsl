#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
// TODO #D: add normal matrix
uniform mat3 normMatrix;
// TODO #A: add light uniforms
uniform vec3 lightDirec;
uniform vec3 lightColor;

uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
// TODO #C: add vertex normal
in vec3 vNorm;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // TODO #B: computer Light vector
    vec3 lightVec = -1 * lightDirec;
    float lightVecMag = sqrt(lightVec.x * lightVec.x + lightVec.y * lightVec.y + lightVec.z * lightVec.z);
    vec3 normLightVec = lightVec / lightVecMag;
    // TODO #E: transform normal vector
    vec3 normVec = vNorm * normMatrix;
    // TODO #F: perform diffuse calculation
    vec3 diffuseComponent = lightColor * materialColor * max(dot(normLightVec, normVec), 0);
    // TODO #G: assign the color for this vertex
    color = diffuseComponent;
}