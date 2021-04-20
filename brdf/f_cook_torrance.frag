// Fresnel - Schlick
//  Cook and Torrance 1982, "A Reflectance Model for Computer Graphics"
//
//  F(v, h) = 1 * ( g - c )^2 * ( 1 + ( (g + c) * c - 1)^2 )
//            -     -----               ---------------
//            2     g + c               (g - c) * c + 1
//
//  g = sqrt(u^2 + c^2 - 1)
//
//  c = v . h
//
//  u = 1 + sqrt(f0)
//      -----------
//      1 - sqrt(f0)

vec3 fresnel(vec3 v, vec3 h, vec3 F0) {
    float f0 = F0.x;
    float sqtf0 = sqrt(f0);
    float u = (1 + sqtf0) / (1 - sqtf0);
    float c = max(dot(v, h), 0.0);
    float g = sqrt(u*u + c*c - 1);

    float k = (g - c) / (g + c);
    float m = ((g + c) * c - 1) / ((g + c) * c + 1);

    return vec3(0.5 * k * k * (1 + m * m));
}
