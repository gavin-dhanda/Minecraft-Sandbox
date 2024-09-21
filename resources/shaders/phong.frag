#version 330 core

in vec3 world_pos;
in vec3 world_normal;
in vec4 camera_pos;
out vec4 fragColor;
in vec2 fragUV;


struct Light {
    vec4 position;
    vec3 color;
    vec4 direction;
};

uniform Light light;

uniform float ka;
uniform float kd;
uniform float ks;

uniform sampler2D myTexture;

uniform float xOffset;
uniform float yOffset;
uniform int lightLength;


uniform int lightTypes[100]; // specifies what type of light it is
uniform vec4 lightDirections[100]; // specifies direction of light
uniform vec3 lightAttenuations[100]; // stores attenuation functions
uniform vec4 lightPositions[100]; // specifies light positions
uniform vec3 lightColors[100];

uniform int blockID;

void main() {
    float blend = 0.85f;
    vec4 texCol = texture(myTexture, vec2(fragUV.x + xOffset, fragUV.y + yOffset));
    vec3 textureColor = vec3(texCol);

    vec3 lightDir;
    float distanceToLight;
    fragColor = vec4(0.0f);

    for(int i = 0; i<lightLength; i++){
        vec3 currAttenuation = lightAttenuations[i];
        float attenuation = 1.0f;
        if(lightTypes[i] == 0){
            lightDir = normalize(vec3(vec3(lightPositions[i])) - vec3(world_pos));
            distanceToLight = distance(world_pos, vec3(lightPositions[i]));
            if(currAttenuation.x == (0.0) && currAttenuation.y == 0.0 && currAttenuation.z == 0.0){
                attenuation = 1.0;
            }else{
                attenuation = 1.0 / (currAttenuation.x + currAttenuation.y * distanceToLight +
                                     currAttenuation.z* distanceToLight * distanceToLight);
            }

        }
        else if(lightTypes[i] == 1){
            lightDir = vec3(-lightDirections[i]);
            attenuation = 1.0f;
        }

        vec3 toCamera = normalize(vec3(camera_pos) - vec3(world_pos));
        //vec3 toLight = vec3(normalize(-lightDir));
        float diffuse  = clamp(dot(normalize(vec3(world_normal)), lightDir), 0, 1);

        vec3 reflectedLight = reflect(-lightDir, normalize(vec3(world_normal)));

        float specular = pow(clamp(dot(toCamera, reflectedLight),0,100), 30);

        float distance = length(vec3(camera_pos) - vec3(world_pos));
        float opacity = (distance > 25.f) ? 1.f - clamp((distance - 25.f) / 10.f, 0.f, 1.f): texCol[3];
        opacity = (blockID == 5) ? opacity * .4f : opacity;

        vec3 currentColor = lightColors[i];

        fragColor += vec4(vec3((kd*(1.0f-blend)+textureColor*blend)*diffuse*attenuation*currentColor +
                        ka*textureColor + ks * specular)*currentColor, opacity)*attenuation;

    }
}
