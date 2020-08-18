#version 330 core

in vec2 v_tex;
uniform sampler2D texSampler;

#define GAMMA_CORRECTION 0
#define GAMMA 2.2

#define HDR 0
#define EXPOSURE 1

out vec4 color;

void main()
{
    #if HDR
        vec3 hdrColor = texture(texSampler, v_tex).rgb;
    
        // Exposure tone mapping
        vec3 mapped = vec3(1.0) - exp(-hdrColor * EXPOSURE);
        // Gamma correction 
        mapped = pow(mapped, vec3(1.0 / GAMMA));
    
        color = vec4(mapped, 1.0);
    #else
        #if GAMMA_CORRECTION
        vec3 sampledColor = texture(texSampler, v_tex).rgb;
        //Gamma correction
        color = vec4(pow(sampledColor, vec3(1.0 / GAMMA)), 1.0);
        #else
            color = texture(texSampler, v_tex);
        #endif
    #endif
}