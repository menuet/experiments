
#pragma once


#include "Particle.h"
#include <vector>


class ParticleController
{
public:

	void update(bool flatten);

	void draw() const;

	void setParticleCount(unsigned int particleCount);

	void pullParticlesToCenter(const cinder::Vec3f& center);

	void applyForceToParticles(float zoneRadiusSqrd, float lowThreshold, float highThreshold, float attractStrength, float repelStrength, float alignStrength);

private:

	std::vector<Particle> m_particles;
};
