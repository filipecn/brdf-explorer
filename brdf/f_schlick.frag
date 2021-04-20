// Fresnel - Schlick
//  Burley 2012, "Physically-Based Shading at Disney"
//
//  F(v, h) = f0 + (1 - f0) * (1 - (v . h))^5

vec3 fresnel(vec3 v, vec3 h, vec3 F0) {
    float f0 = F0.x;
    float vh = clamp(dot(h, v), 0.0, 1.0);
    return vec3(f0 + (1.0 - f0) * pow(max(1.0 - vh, 0.0), 5.0));
}
