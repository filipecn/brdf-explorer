vec3 N = normalize(fNormal);
vec3 V = normalize(cameraPosition - fPosition);
vec3 R = reflect(-V, N);

// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
vec3 F0 = vec3(f0);
F0 = mix(F0, albedo, metallic);

// reflectance equation
vec3 Lo = vec3(0.0);
