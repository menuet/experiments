
#include "particlecontroller.h"
#include <cinder/app/appbasic.h>
#include <cinder/rand.h>
#include <cinder/vector.h>
#include <algorithm>


void ParticleController::update(bool flatten)
{
	for (auto&& particle : m_particles)
		particle.update(flatten);
}

void ParticleController::draw() const
{
	cinder::gl::color(cinder::ColorA(1.0f, 1.0f, 1.0f, 1.0f));
	for (auto&& particle : m_particles)
		particle.draw();

	glBegin(GL_LINES);
	for (auto&& particle : m_particles)
		particle.drawTail();
	glEnd();
}

void ParticleController::setParticleCount(unsigned int particleCount)
{
	const auto oldCount = m_particles.size();

	m_particles.resize(particleCount);

	for (auto i = oldCount; i < particleCount; ++i)
	{
		auto randVec = cinder::Rand::randVec3f();
		cinder::Vec3f pos = randVec * cinder::Rand::randFloat(50.f, 250.f);
		cinder::Vec3f vel = randVec * 2.f;
		m_particles[i] = Particle(pos, vel);
	}
}

void ParticleController::pullParticlesToCenter(const cinder::Vec3f& center)
{
	for (auto&& particle : m_particles)
		particle.pullToCenter(center);
}

void ParticleController::applyForceToParticles(float zoneRadiusSqrd, float lowThreshold, float highThreshold, float attractStrength, float repelStrength, float alignStrength)
{
	const auto iterEnd = end(m_particles);
	for (auto iter = begin(m_particles); iter != iterEnd; ++iter)
	{
		auto& particle1 = *iter;
		std::for_each(std::next(iter), iterEnd, [&](Particle& particle2)
		{
			particle1.applyForce(zoneRadiusSqrd, lowThreshold, highThreshold, attractStrength, repelStrength, alignStrength, particle2);
		});
	}
}
