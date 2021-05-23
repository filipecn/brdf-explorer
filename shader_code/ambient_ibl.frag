// GLOBAL ILLUMINATION ////////////////////////////////////////////////////

vec3 F = F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - max(dot(N, V), 0.0), 0.0), 5.0);

const float MAX_REFLECTION_LOD = 4.0;
vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

vec3 kS =  F;
vec3 kD = 1.0 - kS;
vec3 irradiance = texture(irradianceMap, N).rgb;
vec3 diffuse    = irradiance * albedo;
vec3 ambient    = (kD * diffuse + specular) * ao;

////////////////////////////////////////////////// PER LIGHT CALCULATION //
