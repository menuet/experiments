
#pragma once


#include "cinder/channel.h"
#include "cinder/vector.h"
#include "cinder/color.h"


class Particle
{
public:

	Particle() = default;

	Particle(const cinder::Vec3f& pos, const cinder::Vec3f& vel);

	void update(bool flatten);

	void draw() const;

	void drawTail() const;

	void pullToCenter(const cinder::Vec3f& center);

	void applyForce(float zoneRadiusSqrd, float lowThreshold, float highThreshold, float attractStrength, float repelStrength, float alignStrength, Particle& particle);

private:

	cinder::Vec3f m_pos;
	cinder::Vec3f m_posTail;
	cinder::Vec3f m_vel;
	cinder::Vec3f m_acc;
	float m_radius = 2.f;
	float m_maxSpeed = 2.f;
	float m_minSpeed = 1.f;
	float m_length = 10.0f;
};
