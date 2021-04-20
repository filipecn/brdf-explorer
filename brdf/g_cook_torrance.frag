// Geometric Cook-Torrance
//  Cook and Torrance 1982, "A Reflectance Model for Computer Graphics"
//
//   G(l,v,h) =   min(1, 2 * (n . h) * (n . v), 2 * (n . h) * (n . l) )
//                       --------------------   --------------------
//                             (v . h)                (v . h)

float geometricShadowing(vec3 n, vec3 h, vec3 v, vec3 l, float roughness) {
    float nv = max(dot(n, v), 0.0);
    float nl = max(dot(n, l), 0.0);
    float nh = max(dot(n, h), 0.0);

    float k = 2 * nh / max(dot(v, h), 0.0001);

    return min(1, min(k * nv, k * nl));
}