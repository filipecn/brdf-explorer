#version 440 core
layout(location = 0) out vec4 fragColor;

in vec3 fNormal;
in vec3 fPosition;
in vec3 fUV;

uniform vec3 cameraPosition;

// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;
// fresnel reflectance at normal incidence
uniform float f0;

// lights
uniform vec3 lightPosition;
uniform vec3 lightColor;

const float PI = 3.14159265359;
