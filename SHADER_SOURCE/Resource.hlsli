// Material Default Texture
Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t8);
//Texture2D emissiveTexture : register(t9);

// defferd
Texture2D positionTarget : register(t2);
Texture2D normalTarget : register(t3);
Texture2D albedoTarget : register(t4);
Texture2D specularTarget : register(t5);

Texture2D diffuseLightTarget: register(t6);
Texture2D specularLightTarget : register(t7);
Texture2D ShadowMapTarget : register(t10);

			//std::shared_ptr<Texture> pos = std::make_shared<Texture>();
			//std::shared_ptr<Texture> normal = std::make_shared<Texture>();
			//std::shared_ptr<Texture> albedo = std::make_shared<Texture>();
			//std::shared_ptr<Texture> specular = std::make_shared<Texture>();

//Atlas texture
TextureCube skyTexture : register(t11);
//Texture2D skySphererTexture : register(t11);

Texture2D atlasTexture : register(t12);

// Light
//StructuredBuffer<LightAttribute> lightAttributes : register(t13);
//StructuredBuffer<LightAttribute> lightAttributes3D : register(t14);

// Particle
//StructuredBuffer<Particle> particleBuffer : register(t15);
//RWStructuredBuffer<Particle> ParticleBuffer : register(u0);
//RWStructuredBuffer<ParticleShared> ParticleSharedBuffer : register(u1);

// noise
//Texture2D NoiseTexture : register(t16);

// Bone Animation
//StructuredBuffer<tFrameTrans> g_arrFrameTrans : register(t17);
//StructuredBuffer<matrix> g_arrOffset : register(t18);
//RWStructuredBuffer<matrix> g_arrFinelMat : register(u2);

// postProcess
Texture2D postProcessTexture : register(t60);
Texture2D guiGameTexture : register(t61);

//TextureCube cube : register(t61);

//Texture2D 0 : register(t60);
//Texture2D 1 : register(t61);
//Texture2D 2 : register(t60);
//Texture2D 3 : register(t61);
