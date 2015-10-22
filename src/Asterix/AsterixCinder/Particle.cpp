
#pragma warning(disable:4244)
#include "particle.h"
#include <cinder/rand.h>
#include <cinder/gl/gl.h>
#include <cinder/app/appbasic.h>
#include <cinder/perlin.h>


Particle::Particle(const cinder::Vec3f& pos, const cinder::Vec3f& vel)
	: m_pos(pos)
	, m_vel(vel)
{
}

void Particle::update(bool flatten)
{
	if (flatten)
		m_acc.z = 0.0f;

	m_vel += m_acc;

	const auto velNormal = m_vel.normalized();
	const auto lengthSqrd = m_vel.lengthSquared();
	if (lengthSqrd > m_maxSpeed*m_maxSpeed)
		m_vel = velNormal * m_maxSpeed;
	else if (lengthSqrd < m_minSpeed*m_minSpeed)
		m_vel = velNormal * m_minSpeed;

	m_pos += m_vel;
	m_posTail = m_pos - velNormal * m_length;

	if (flatten)
		m_pos.z = 0.0f;

	m_vel *= 0.99f;
	m_acc = cinder::Vec3f::zero();
}

void Particle::draw() const
{
	cinder::gl::drawSphere(m_pos, m_radius, 8);
}

void Particle::drawTail() const
{
	cinder::gl::color(cinder::ColorA(1.0f, 1.0f, 1.0f, 1.0f));
	cinder::gl::vertex(m_pos);
	cinder::gl::color(cinder::ColorA(1.0f, 0.0f, 0.0f, 1.0f));
	cinder::gl::vertex(m_posTail );
}

void Particle::pullToCenter(const cinder::Vec3f& center)
{
	cinder::Vec3f dirToCenter = m_pos - center;
	const auto distToCenter = dirToCenter.length();
	const auto maxDistance = 300.0f;

	if (distToCenter > maxDistance)
	{
		dirToCenter.normalize();
		const auto pullStrength = 0.0001f;
		m_vel -= dirToCenter * ((distToCenter - maxDistance) * pullStrength);
	}
}

void Particle::applyForce(float zoneRadiusSqrd, float lowThreshold, float highThreshold, float attractStrength, float repelStrength, float alignStrength, Particle& particle)
{
	const auto dir = m_pos - particle.m_pos;
	const auto distSqrd = dir.lengthSquared();

	if (distSqrd > zoneRadiusSqrd)
		return;

	static const auto twoPI = static_cast<float>(M_PI * 2.);

	const auto percent = distSqrd / zoneRadiusSqrd;

	if (percent < lowThreshold)
	{
		const auto F = (lowThreshold / percent - 1.0f) * repelStrength;
		const auto dirF = dir.normalized() * F;
		m_acc += dirF;
		particle.m_acc -= dirF;
		return;
	}

	if (percent < highThreshold)
	{
		const auto thresholdDelta = highThreshold - lowThreshold;
		const auto adjustedPercent = (percent - lowThreshold) / thresholdDelta;
		const auto F = (1.0f - (cos(adjustedPercent * twoPI) * -0.5f + 0.5f)) * alignStrength;
		m_acc += particle.m_vel.normalized() * F;
		particle.m_acc += m_vel.normalized() * F;
		return;
	}

	const auto thresholdDelta = 1.0f - highThreshold;
	const auto adjustedPercent = (percent - highThreshold) / thresholdDelta;
	const auto F = -(1.0 - (cos(adjustedPercent * twoPI) * -0.5f + 0.5f)) * attractStrength;
	const auto dirF = dir.normalized() * F;
	m_acc += dirF;
	particle.m_acc -= dirF;
}
