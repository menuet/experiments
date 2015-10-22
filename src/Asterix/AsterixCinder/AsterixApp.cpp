
#include "asterixapp.h"


AsterixApp::AsterixApp()
	: m_eye(0.0f, 0.0f, 500.0f)
	, m_center(cinder::Vec3f::zero())
	, m_up(cinder::Vec3f::yAxis())
{
}

void AsterixApp::prepareSettings(Settings* settings)
{
	settings->setWindowSize(1280, 720);
	settings->setFrameRate(60.0f);
}

void AsterixApp::setup()
{
	m_camera.setPerspective(60.0f, getWindowAspectRatio(), 5.0f, 3000.0f);
	m_camera.lookAt(m_eye, m_center, m_up);
	cinder::gl::setMatrices(m_camera);
	cinder::gl::rotate(m_sceneRotation);

	m_params = cinder::params::InterfaceGl::create("Flocking", cinder::Vec2i(225, 200));
	m_params->addParam("Scene Rotation", &m_sceneRotation);
	m_params->addParam("Eye Distance", &m_cameraDistance, "min=50.0 max=1000.0 step=50.0 keyIncr=s keyDecr=w");
	m_params->addSeparator();
	m_params->addParam("Center Gravity", &m_centralGravity, "keyIncr=g");
	m_params->addParam("Flatten", &m_flatten, "keyIncr=f");
	m_params->addSeparator();
	m_params->addParam("Zone Radius", &m_zoneRadius, "min=10.0 max=100.0 step=1.0 keyIncr=z keyDecr=Z");
	m_params->addParam("Low Threshold", &m_lowThreshold, "min=0.025 max=1.0 step=0.025 keyIncr=l keyDecr=L");
	m_params->addParam("High Threshold", &m_highThreshold, "min=0.025 max=1.0 step=0.025 keyIncr=h keyDecr=H");
	m_params->addSeparator();
	m_params->addParam("Attract Strength", &m_attractStrength, "min=0.001 max=0.1 step=0.001 keyIncr=a keyDecr=A");
	m_params->addParam("Repel Strength", &m_repelStrength, "min=0.001 max=0.1 step=0.001 keyIncr=r keyDecr=R");
	m_params->addParam("Align Strength", &m_alignStrength, "min=0.001 max=0.1 step=0.001 keyIncr=o keyDecr=O");
	m_params->addSeparator();
	m_params->addParam("Particles Count", &m_particleCount, "min=100 max=10000 step=50 keyIncr=p keyDecr=P");

	m_particleController.setParticleCount(static_cast<unsigned int>(m_particleCount));
}

void AsterixApp::update()
{
	m_eye = cinder::Vec3f(0.0f, 0.0f, m_cameraDistance);
	m_camera.lookAt(m_eye, m_center, m_up);
	cinder::gl::setMatrices(m_camera);
	cinder::gl::rotate(m_sceneRotation);

	m_particleController.setParticleCount(static_cast<unsigned int>(m_particleCount));
	m_particleController.applyForceToParticles(m_zoneRadius * m_zoneRadius, m_lowThreshold, m_highThreshold, m_attractStrength, m_repelStrength, m_alignStrength);
	if (m_centralGravity)
		m_particleController.pullParticlesToCenter(m_center);
	m_particleController.update(m_flatten);
}

void AsterixApp::draw()
{
	cinder::gl::clear(cinder::Color(0, 0, 0.01f), true);
	cinder::gl::enableDepthRead();
	cinder::gl::enableDepthWrite();

	m_particleController.draw();

	cinder::gl::disableDepthRead();
	cinder::gl::disableDepthWrite();
	cinder::gl::setMatricesWindow(getWindowSize());
	cinder::gl::pushModelView();

	cinder::gl::translate(cinder::Vec3f(117.0f, getWindowHeight() - 117.0f, 0.0f));

	cinder::gl::color(cinder::ColorA(1.0f, 0.25f, 0.25f, 1.0f));
	cinder::gl::drawSolidCircle(cinder::Vec2f::zero(), m_zoneRadius);

	cinder::gl::color(cinder::ColorA(0.25f, 1.0f, 0.25f, 1.0f));
	cinder::gl::drawSolidCircle(cinder::Vec2f::zero(), m_zoneRadius * m_highThreshold);

	cinder::gl::color(cinder::ColorA(0.25f, 0.25f, 1.0f, 1.0f));
	cinder::gl::drawSolidCircle(cinder::Vec2f::zero(), m_zoneRadius * m_lowThreshold);

	cinder::gl::color(cinder::ColorA(1.0f, 1.0f, 1.0f, 0.25f));
	cinder::gl::drawStrokedCircle(cinder::Vec2f::zero(), 100.0f);

	cinder::gl::popModelView();

	m_params->draw();
}
