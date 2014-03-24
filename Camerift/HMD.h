#ifndef HEADER_HMD
#define HEADER_HMD

#include "OVR.h"
using namespace OVR;

typedef OVR::Util::Render::StereoEye hmdcontroler_eye;
#define EYE_LEFT OVR::Util::Render::StereoEye_Left
#define EYE_RIGHT OVR::Util::Render::StereoEye_Right

void InitHMD(); 
void DestroyHMD();

class hmdcontroler
{
public:
	hmdcontroler();
	~hmdcontroler();

	bool GetValid() { return m_valid; }
	int GetW() {return m_hmd.HResolution;}
	int GetH() {return m_hmd.VResolution;};
	void Update();
	float GetXAngle();
	float GetYAngle();
	float GetZAngle();
	
	float* GetDistortion(hmdcontroler_eye eye);
	float GetEyeDistance();
	const char* GetMonitorName();
	OVR::Vector2f GetLenseCenter(hmdcontroler_eye eye);
	OVR::Vector2f GetScreenCenter(hmdcontroler_eye eye);
	OVR::Vector2f GetScale(hmdcontroler_eye eye);
	OVR::Vector2f GetScaleIn(hmdcontroler_eye eye);
	
	
private:
	Ptr<DeviceManager>				m_pManager;
	Ptr<HMDDevice>					m_pHMD;
	Ptr<SensorDevice>				m_pSensor;
	HMDInfo							m_hmd;
	SensorFusion					m_SFusion;
	Util::Render::StereoConfig      m_stereo_config;

	bool							m_valid;
	float							*m_pDistortionK;
	float							m_EyeDistance;
	const char*						m_MonitorName;

	float							m_EyeRoll;
	float							m_EyePitch;
	float							m_EyeYaw;
	float							m_LastEyeYaw;
	
};

#endif