// Geometric Neumann
//  Neumann et al. 1999, "Compact metallic reflectance models"
//
//   G(l,v,h) =  (n . l) * (n . v)
//              --------------------
//              max((n . l),(n . v))

float geometricShadowing(vec3 n, vec3 h, vec3 v, vec3 l, float roughness) {
    float nv = max(dot(n, v), 0.0);
    float nl = max(dot(n, l), 0.0);

    return nv * nl / max(max(nv, nl), 0.0001);
}