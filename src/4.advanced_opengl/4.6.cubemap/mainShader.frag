#version 330 core

struct Material {
    sampler2D ambient1;
    sampler2D diffuse1;
    sampler2D specular1;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform Material material;
uniform SpotLight spotLight;
uniform PointLight pointLight;
uniform DirLight dirLight;
uniform vec3 viewPos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

vec3
calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse1, TexCoords).rgb;

    // diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse1, TexCoords).rgb;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));

    // relect
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = reflect(I, normal);
    vec3 relect = texture(skybox, R).rgb * texture(material.ambient1, TexCoords).rgb;

    return ambient + diffuse + specular + relect;
}



vec3
calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse1, TexCoords).rgb;

    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse1, TexCoords).rgb;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));

    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 /
    (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse1, TexCoords).rgb;

    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse1, TexCoords).rgb;

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular1, TexCoords));

    // radiant
    float theta = dot(lightDir, -light.direction);
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    diffuse *= intensity;
    specular *= intensity;

    return ambient + diffuse + specular;
}

void main() {
    // 属性
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    // FragColor += vec4(calcSpotLight(spotLight, normal, FragPos, viewDir), 1.0f);
    // FragColor += vec4(calcPointLight(pointLight, normal, FragPos, viewDir), 1.0f);
    FragColor += vec4(calcDirLight(dirLight, normal, viewDir), 1.0f);
}