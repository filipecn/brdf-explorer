// Geometric Shadowing - Smith GGX
//
//   G(l,v,h) = G1(l) * G1(v)
//
//
//   G(v) =                   2 * (n . v)
//           --------------------------------------------
//           (n . v) + sqrt(a^2 + (1 - a^2) * (n . v)^2 )

float G1(float nv, float roughness) {
    float a2 = roughness * roughness;

    float nom   = 2 * nv;
    float denom = nv + sqrt(a2 + (1 - a2) * nv * nv);

    return nom / max(denom, 0.0001);
}

float geometricShadowing(vec3 n, vec3 h, vec3 v, vec3 l, float roughness) {
    float nv = max(dot(n, v), 0.0);
    float nl = max(dot(n, l), 0.0);
    float ggx2 = G1(nv, roughness);
    float ggx1 = G1(nl, roughness);

    return ggx1 * ggx2;
}