#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
uniform mat4 modelMatrix;
uniform mat3 normMatrix;

uniform vec3 lightDirec;
uniform vec3 lightColor;

uniform vec3 camPos;

// point light
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

// spot light
uniform vec3 spotLightPos;
uniform vec3 spotLightDirec;
uniform vec3 spotLightColor;
uniform float spotLightAngle;

// attenuation constants
uniform float attenuationConst;
uniform float attenuationLinear;
uniform float attenuationQuadratic;


uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
in vec3 vNorm;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    vec3 lightVec = normalize(-1 * lightDirec);

    //    float lightVecMag = sqrt(lightVec.x * lightVec.x + lightVec.y * lightVec.y + lightVec.z * lightVec.z);
    //    vec3 normLightVec = lightVec / lightVecMag;

    vec3 normVec = vNorm * normMatrix;

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 diffuse = max(dot(normVec, lightVec), 0) * lightColor;

    // get vertex position in world space
    vec3 vPosWorld = vec3(modelMatrix * vec4(vPos, 1.0));

    // specular
    float specularStrength = 0.1;
    vec3 viewVec = normalize(vPos - camPos);
    vec3 reflectVec = -lightVec + 2 * dot(normVec, lightVec) * normVec;
    float reflectance = pow(max(dot(viewVec, reflectVec), 0.0), 4);
    vec3 specular = specularStrength * reflectance * lightColor;

    // color from directional light
    vec3 dirColor = (ambient + diffuse + specular);


    // attenuation calculation
    float d = length(pointLightPos - vPosWorld);
    float attenuation = 1.0 / (attenuationConst + attenuationLinear * d + attenuationQuadratic * (d * d));

    // point light
    vec3 pointLightVec = normalize(pointLightPos - vPosWorld);
    vec3 pointDiffuse = max(dot(normVec, pointLightVec), 0) * pointLightColor;
    vec3 pointSpecular = specularStrength * reflectance * pointLightColor;

    // color from point light
    vec3 pointColor = (pointDiffuse + pointSpecular) * attenuation * 1000;

    // spot light
    vec3 spotLightVec = normalize(spotLightPos - vPosWorld);

    vec3 spotColor;
    if(dot(spotLightDirec, -spotLightVec) > spotLightAngle) {
        vec3 spotDiffuse = max(dot(normVec, spotLightVec), 0) * spotLightColor;

        spotColor = (spotDiffuse);

    } else {
        spotColor = vec3(0, 0, 0);
    }

    vec3 newColor = (dirColor + pointColor + spotColor) * materialColor;
    color = newColor;

}