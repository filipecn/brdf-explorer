// NDF
// Beckmann
//
// D(m) =            1          *  exp(  (n . m)^2 - 1  )
//         --------------------         ----------------
//         pi * a^2 * (n . m)^4          a^2 * (n . m)^2
//
// where
//      a in [0, 1]
float normalDistributionFunction(vec3 n, vec3 m, float roughness) {
    float a = roughness;
    float a2 = a * a;

    float nm = max(dot(n, m), 0.0);
    float nm2 = nm * nm;
    float nm4 = nm2 * nm2;

    float nominator = exp((nm2 - 1)/max((a2 * nm2),0.001));
    float denominator = PI * a2 * nm4;

    return nominator / max(denominator, 0.001);
}
