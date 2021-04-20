// Geometric Implicit
//   Hoffman 2013, "Background: Physics and Math of Shading"
//
//   G(l,v,h) = (n . l) * (n . v)

float geometricShadowing(vec3 n, vec3 h, vec3 v, vec3 l, float roughness) {
    float nv = max(dot(n, v), 0.0);
    float nl = max(dot(n, l), 0.0);

    return nv * nl;
}