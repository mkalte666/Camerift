/*
File: HMD.h
Purpose: Contains the functions to communicate with the oculus sdk
Author(s): Malte Kieﬂling (mkalte666)
*/
#include <iostream>
#include "HMD.h"

double RadtoDegree(double rad) 
{
	return rad*(180/3.14159265359); //PI!
}

void InitHMD()
{
	OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
}

void DestroyHMD()
{
	OVR::System::Destroy();
}

hmdcontroler::hmdcontroler()
{
	m_valid = false;
	m_pDistortionK = new float[4];
	m_pDistortionK[0] = m_pDistortionK[1] = m_pDistortionK[2] = m_pDistortionK[3] = 0.0f;
	m_pManager = *DeviceManager::Create();
	m_pHMD = *m_pManager->EnumerateDevices<HMDDevice>().CreateDevice();
	if(m_pHMD == NULL) {
		std::cout << "Warning: No HMD detected!\n";
		return;
	}

	m_pDistortionK = new float[3];
	 
	if (m_pHMD->GetDeviceInfo(&m_hmd)) {
		m_MonitorName = m_hmd.DisplayDeviceName;
		m_EyeDistance = m_hmd.InterpupillaryDistance;
		m_pDistortionK = m_hmd.DistortionK;
	}

	m_pSensor = *m_pHMD->GetSensor();
	if (m_pSensor)
		m_SFusion.AttachToSensor(m_pSensor);

	m_LastEyeYaw = 0.0f;
	m_SFusion.SetGravityEnabled(true);
	m_SFusion.SetYawCorrectionEnabled(true);
	m_stereo_config.SetFullViewport(OVR::Util::Render::Viewport(0,0, m_hmd.HResolution, m_hmd.VResolution));
    m_stereo_config.SetStereoMode(OVR::Util::Render::Stereo_LeftRight_Multipass);
	if (m_hmd.HScreenSize > 0.140f) // 7"
		m_stereo_config.SetDistortionFitPointVP(-1.0f, 0.0f);
    else
        m_stereo_config.SetDistortionFitPointVP(0.0f, 1.0f);

	//FOV (we set this static - i will find out how to calculate this dynamic, but for now...)
	m_vFOV = 110.0f;
	m_hFOV = 90.0f;

	m_valid = true;
}

hmdcontroler::~hmdcontroler()
{
	delete m_pDistortionK;
	m_pSensor.Clear();
	m_pHMD.Clear();
	m_pManager.Clear();
	
}

void hmdcontroler::Update()
{
	Quatf hmdOrient = m_SFusion.GetOrientation();
	//float yaw = 0.0f;
	hmdOrient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&m_EyeYaw, &m_EyePitch, &m_EyeRoll);
	//std::cout << yaw << std::endl;
	//m_EyeYaw += (yaw-m_LastEyeYaw);
	//m_LastEyeYaw = yaw;
}

float hmdcontroler::GetXAngle()
{
	if(!GetValid()) return 0;
	return (float)RadtoDegree(m_EyePitch);
}

float hmdcontroler::GetYAngle()
{
	if(!GetValid()) return 0;
	return (float)RadtoDegree(m_EyeYaw);
}

float hmdcontroler::GetZAngle()
{
	if(!GetValid()) return 0;
	return (float)RadtoDegree(m_EyeRoll);
}

float* hmdcontroler::GetDistortion(hmdcontroler_eye eye)
{
	if(!GetValid()) return m_pDistortionK;
	const OVR::Util::Render::StereoEyeParams& params = m_stereo_config.GetEyeRenderParams(eye);
	const OVR::Util::Render::DistortionConfig& distortion = *params.pDistortion;
	memcpy(&m_pDistortionK[0], &distortion.K[0], 4);
	return m_pDistortionK;
}

float hmdcontroler::GetEyeDistance()
{
	if(!GetValid()) return 0;
	return m_EyeDistance;
}

const char* hmdcontroler::GetMonitorName()
{
	if(!GetValid()) return "NoHMD";
	return m_MonitorName;
}

OVR::Vector2f hmdcontroler::GetLenseCenter(hmdcontroler_eye eye) 
{
	if(!GetValid()) return OVR::Vector2f(0,0);
	const OVR::Util::Render::StereoEyeParams& params = m_stereo_config.GetEyeRenderParams(eye);
	if(params.pDistortion) {
		unsigned int width = m_hmd.HResolution;
        unsigned int height = m_hmd.VResolution;
		const OVR::Util::Render::DistortionConfig& distortion = *params.pDistortion;
		float w  = float(params.VP.w) / float(width),
                          h  = float(params.VP.h) / float(height),
                          x  = float(params.VP.x) / float(width),
                          y  = float(params.VP.y) / float(height);
		float as = float(params.VP.w) / float(params.VP.h);
						if(eye == EYE_RIGHT) return Vector2f(x + (w - distortion.XCenterOffset * 0.5f)*0.5f,        y + h*0.5f);
						return Vector2f(x + (w + distortion.XCenterOffset * 0.5f)*0.5f,        y + h*0.5f);
	}
	
	//So we need a default return. and because we can, its 0
	return OVR::Vector2f(0,0);
}

OVR::Vector2f hmdcontroler::GetScreenCenter(hmdcontroler_eye eye) 
{
	if(!GetValid()) return OVR::Vector2f(0,0);
	const OVR::Util::Render::StereoEyeParams& params = m_stereo_config.GetEyeRenderParams(eye);
	if(params.pDistortion) {
		unsigned int width = m_hmd.HResolution;
        unsigned int height = m_hmd.VResolution;
		const OVR::Util::Render::DistortionConfig& distortion = *params.pDistortion;
		float w  = float(params.VP.w) / float(width),
                          h  = float(params.VP.h) / float(height),
                          x  = float(params.VP.x) / float(width),
                          y  = float(params.VP.y) / float(height);
		float as = float(params.VP.w) / float(params.VP.h);
		return Vector2f(x + w*0.5f,y + h*0.5f);
	}
	
	//So we need a default return. and because we can, its 0
	return OVR::Vector2f(0,0);
}

OVR::Vector2f hmdcontroler::GetScale(hmdcontroler_eye eye) 
{
	if(!GetValid()) return OVR::Vector2f(0,0);
	const OVR::Util::Render::StereoEyeParams& params = m_stereo_config.GetEyeRenderParams(eye);
	if(params.pDistortion) {
		unsigned int width = m_hmd.HResolution;
        unsigned int height = m_hmd.VResolution;
		const OVR::Util::Render::DistortionConfig& distortion = *params.pDistortion;
		float scaleFactor               = 1.0f / distortion.Scale;
		float w  = float(params.VP.w) / float(width),
                          h  = float(params.VP.h) / float(height),
                          x  = float(params.VP.x) / float(width),
                          y  = float(params.VP.y) / float(height);
		float as = float(params.VP.w) / float(params.VP.h);
		return Vector2f((w/2) * scaleFactor,(h/2) * scaleFactor * as);
	}

	//So we need a default return. and because we can, its 0
	return OVR::Vector2f(0,0);
}

OVR::Vector2f hmdcontroler::GetScaleIn(hmdcontroler_eye eye) 
{
	if(!GetValid()) return OVR::Vector2f(0,0);
	const OVR::Util::Render::StereoEyeParams& params = m_stereo_config.GetEyeRenderParams(eye);
	if(params.pDistortion) {
		unsigned int width = m_hmd.HResolution;
		
        unsigned int height = m_hmd.VResolution;
		const OVR::Util::Render::DistortionConfig& distortion = *params.pDistortion;
		float scaleFactor               = 1.0f / distortion.Scale;
		float w  = float(params.VP.w) / float(width),
                          h  = float(params.VP.h) / float(height),
                          x  = float(params.VP.x) / float(width),
                          y  = float(params.VP.y) / float(height);
		float as = float(params.VP.w) / float(params.VP.h);
		return Vector2f((2/w),(2/h) / as);
	}
	
	//So we need a default return. and because we can, its 0
	return OVR::Vector2f(0,0);
}

float hmdcontroler::GetVScreenSize()
{
	return m_hmd.VScreenSize;
}

float hmdcontroler::GetHScreenSize()
{
	return m_hmd.HScreenSize;
}

float hmdcontroler::GetIpd()
{
	return m_hmd.InterpupillaryDistance;
}