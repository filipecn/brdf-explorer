// Geometric Kelemen
//  Kelemen 2001, "A microfacet based coupled specular-matte brdf model with importance sampling"
//
//   G(l,v,h) =  (n . l) * (n . v)
//              --------------------
//                    (n . h)^2

float geometricShadowing(vec3 n, vec3 h, vec3 v, vec3 l, float roughness) {
    float nv = max(dot(n, v), 0.0);
    float nl = max(dot(n, l), 0.0);
    float nh = max(dot(n, h), 0.0);

    return nv * nl / max(nh * nh, 0.0001);
}