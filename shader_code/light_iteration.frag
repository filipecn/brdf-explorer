// PER LIGHT CALCULATION //////////////////////////////////////////////////

// calculate per-light radiance
vec3 L = normalize(lightPosition - fPosition);
vec3 H = normalize(V + L);
float distance    = length(lightPosition - fPosition);
float attenuation = 1.0 / (distance * distance);
vec3  radiance    = lightColor * attenuation;

// Cook-Torrance BRDF
float NDF = normalDistributionFunction(N, H, roughness);
float G   = geometricShadowing(N, H, V, L, roughness);
vec3  F   = fresnel(V, H, F0);

vec3 nominator    = NDF * G * F;
float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
vec3 specular = nominator / max(denominator, 0.001);

// kS is equal to Fresnel
vec3 kS = F;
// for energy conservation, the diffuse and specular light can't
// be above 1.0 (unless the surface emits light); to preserve this
// relationship the diffuse component (kD) should equal 1.0 - kS.
vec3 kD = vec3(1.0) - kS;
// multiply kD by the inverse metalness such that only non-metals
// have diffuse lighting, or a linear blend if partly metal (pure metals
// have no diffuse light).
kD *= 1.0 - metallic;

// scale light by NdotL
float NdotL = max(dot(N, L), 0.0);

// add to outgoing radiance Lo
Lo += (kD * albedo / PI + specular) * radiance * NdotL;

////////////////////////////////////////////////// PER LIGHT CALCULATION //
