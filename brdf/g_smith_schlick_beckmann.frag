// Geometric Shadowing - Smith - GGX

float GeometrySchlickGGX(float nv, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = nv;
    float denom = nv * (1.0 - k) + k;

    return nom / denom;
}

float geometricShadowing(vec3 n, vec3 h, vec3 v, vec3 l, float roughness) {
    float nv = max(dot(n, v), 0.0);
    float nl = max(dot(n, l), 0.0);
    float ggx2 = GeometrySchlickGGX(nv, roughness);
    float ggx1 = GeometrySchlickGGX(nl, roughness);

    return ggx1 * ggx2;
}