
#pragma once


#include "particlecontroller.h"
#include <cinder/app/appbasic.h>
#include <cinder/imageio.h>
#include <cinder/gl/texture.h>
#include <cinder/perlin.h>
#include <cinder/camera.h>
#include <cinder/params/params.h>


class AsterixApp : public cinder::app::AppBasic
{
public:

	AsterixApp();

	virtual void prepareSettings(Settings* settings) override;

	virtual void setup() override;

	virtual void update() override;

	virtual void draw() override;

private:

	ParticleController m_particleController;
	cinder::CameraPersp m_camera;
	cinder::Vec3f m_eye;
	cinder::Vec3f m_center;
	cinder::Vec3f m_up;
	cinder::params::InterfaceGlRef m_params;
	cinder::Quatf m_sceneRotation;
	float m_cameraDistance = 500.f;
	bool m_centralGravity = true;
	float m_zoneRadius = 65.f;
	float m_lowThreshold = .4f;
	float m_highThreshold = .7f;
	float m_attractStrength = 0.005f;
	float m_repelStrength = 0.01f;
	float m_alignStrength = 0.01f;
	bool m_flatten = false;
	float m_particleCount = 500;
};
