vec3 color = ambient + Lo;

// HDR tonemapping
color = color / (color + vec3(1.0));
// gamma correct
color = pow(color, vec3(1.0/2.2));

fragColor = vec4(color, 1.0);