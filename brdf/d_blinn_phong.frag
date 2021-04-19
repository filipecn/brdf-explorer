// NDF
// Blinn-Phong
//
// D(m) =            X(n . m) * (a + 2) * (n . m)^a
//                              -------
//                                2pi
//
// where
//      X(x) = 0, x <= 0
//             1, x  > 0
//
//      a in [0, inf]
//
//      A more convenient form is a = k^s, where s in [0,1] and k is
//      an upper bound for a, ex: 8192
float normalDistributionFunction(vec3 n, vec3 m, float roughness) {
    float a = roughness;
    float nm = max(dot(n, m), 0.0);
    float nma = pow(nm, a);

    float nominator = nm * nma * (a + 2.0);
    float denominator = 2.0 * PI;

    return nominator / max(denominator, 0.001);
}
