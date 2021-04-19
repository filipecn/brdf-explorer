// NDF
// Trowbridge-Reitz GGX
//
// D(m) =            X(n . m) * a^2
//         ----------------------------------
//         pi * (1 + (n . m)^2 * (a^2 - 1))^2
//
// where
//      X(x) = 0, x <= 0
//             1, x  > 0
//
//      a = roughnes^2
float normalDistributionFunction(vec3 n, vec3 m, float roughness) {
    float a = roughness * roughness;
    float a2 = a*a;

    float nm = max(dot(n, m), 0.0);
    float nm2 = nm * nm;

    float nominator = a2;
    float denominator = (1 + nm2 * (a2 - 1.0));
    denominator = PI * denominator * denominator;

    return nominator / max(denominator, 0.001);
}
