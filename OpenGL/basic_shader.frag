#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Colour;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColour;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 dirLightDirection;

uniform sampler2D shadowMap;

float ShadowCalculation(vec4 FragPosLightSpace, vec3 N, vec3 L)
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;


    if (projCoords.z > 1.0)
        return 0.0;

    float bias = 0.001 + 0.005 * (1.0 - dot(N,L));

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(
                shadowMap,
                projCoords.xy + vec2(x, y) * texelSize
            ).r;

            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
    

    return shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265 * denom * denom;

    return nom / denom;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 CookTorranceBRDF(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness)
{
    vec3 H = normalize(V + L);

    // Fresnel
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // Normal Distribution
    float D = DistributionGGX(N, H, roughness);

    // Geometry
    float G = GeometrySmith(N, V, L, roughness);

    // Cook-Torrance BRDF
    float NdotL = max(dot(N, L), 0.0);
    vec3 numerator = D * F * G;
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.001; 
    vec3 specular = numerator / denominator;
    specular = clamp(specular, 0.0, 1.0);

    // kS = specular, kD = diffuse
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 diffuse = kD * albedo / 3.14159265 * NdotL;

    return (diffuse + specular);
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 L = normalize(-dirLightDirection);

    float shadow = ShadowCalculation(FragPosLightSpace, N, L);

    float lightConstant = 1.0;
    float lightLinear = 0.001;
    float lightQuadratic = 0.00002;
    vec4 ambient = vec4(Colour  * 0.005 , 1.0);

    float distance = length(lightPos - FragPos);
    //float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));

    vec3 radiance = vec3(1.0); //* attenuation;
    vec3 pbrCol = CookTorranceBRDF(N, V, L, Colour, 0.0, 0.2) * radiance * (1.0 - shadow); // max(dot(N, L), 0.0) * 

    vec3 viewDir = normalize(viewPos - FragPos);
    //vec4 textureCol = texture(objTexture, TexCoord);

    FragColour = ambient + vec4(pow( pbrCol, vec3(1.0/2.2)) , 1.0);

}