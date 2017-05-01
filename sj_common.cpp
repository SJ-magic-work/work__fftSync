/************************************************************
************************************************************/
#include "sj_common.h"

/************************************************************
************************************************************/
MY_GUI *GlobalGui;
MY_GUI_COLOR * GlobalGui_Col;

/************************************************************
************************************************************/

/******************************
******************************/
void MY_GUI::setup(int N)
{
	/********************
	********************/
	gui.setup();
	
	/********************
	********************/
	GuiGroup_Graph_Disp.setup("GraphDisp");
	
	GuiGroup_Graph_Disp.add(GraphRadius.setup("Radius", 131, 50, 200));
	GuiGroup_Graph_Disp.add(BarHeight.setup("BarHeight", 90, 50, 200));
	GuiGroup_Graph_Disp.add(BarWidth_Bottom.setup("BarWidth Btm", 15, 1, 20));
	GuiGroup_Graph_Disp.add(BarWidth_Top.setup("BarWidth Top", 1.5, 0, 20));
	GuiGroup_Graph_Disp.add(b_GainAdjust.setup("b_GainAdjust", false));
	GuiGroup_Graph_Disp.add(AudioSample_Amp.setup("AudioSample_Amp", 0.06, 0, 0.2));
	
	gui.add(&GuiGroup_Graph_Disp);
	
	/********************
	********************/
	GuiGroup_Graph_Filtering.setup("FilterSetting");
	
	GuiGroup_Graph_Filtering.add(CutOff_From.setup("CutOff_From", 1, 1, N/2 - 1));
	GuiGroup_Graph_Filtering.add(CutOff_To.setup("CutOff_To", 20, 1, N/2 - 1));
	GuiGroup_Graph_Filtering.add(k_Smoothing.setup("k_LPF", 0.065, 0.02, 0.1));
	GuiGroup_Graph_Filtering.add(NonLinear_Range.setup("NonLinear_Range", 0, 0, 0.1));
	GuiGroup_Graph_Filtering.add(k_Smoothing_Gain.setup("k_LPF_Gain", 0.065, 0.02, 0.1));
	
	gui.add(&GuiGroup_Graph_Filtering);
	
	/********************
	********************/
	GuiGroup_Graph_Animation.setup("Graph Animation");
	
	GuiGroup_Graph_Animation.add(b_phaseRotation.setup("b_phaseRotation", true));
	GuiGroup_Graph_Animation.add(phase_deg.setup("phase_deg", 270, 0, 360));
	GuiGroup_Graph_Animation.add(phaseRotaion_Speed.setup("Speed", 6, 0, 60));
	
	GuiGroup_Graph_Animation.add(PhaseNoise_Amp.setup("PhaseNoise_Amp", 360, 0, 360));
	GuiGroup_Graph_Animation.add(PhaseNoise_Speed_sec.setup("PhaseNoise_sec", 5, 0.1, 20));
	
	gui.add(&GuiGroup_Graph_Animation);
	
	/********************
	********************/
	GuiGroup_Logo.setup("Logo");
	
	GuiGroup_Logo.add(LogoScale.setup("LogoScale", 0.352, 0.1, 1));
	GuiGroup_Logo.add(DispTime_x.setup("Time_x", 25, 0, 100));
	GuiGroup_Logo.add(DispTime_y.setup("Time_y", -9, -100, 100));
	
	gui.add(&GuiGroup_Logo);
	
	/********************
	********************/
	GuiGroup_Total_Animation.setup("Total Animation");
	
	GuiGroup_Total_Animation.add(TotalDispNoise_Amp.setup("Noise Amp", 90, 0, 100));
	GuiGroup_Total_Animation.add(TotalDispNoise_Speed.setup("Noise Speed", 5, 1, 15));
	GuiGroup_Total_Animation.add(b_fftSync.setup("b_fftSync", true));
	GuiGroup_Total_Animation.add(TotalDisp_fftSync_Amp.setup("fftSync_Amp", 0.05, 0.01, 0.2));
	
	gui.add(&GuiGroup_Total_Animation);
	
	/********************
	********************/
	GuiGroup_particle.setup("particle");
	
	GuiGroup_particle.add(ParticleSize.setup("ParticleSize", 2.0, 1.0, 4));
	GuiGroup_particle.add(ParticleSpeedThresh.setup("Speed thresh", 0.2, 0.01, 1));
	
	GuiGroup_particle.add(Particle_friction_DownPer_sec.setup("friction sec", 0.0991, 0.01, 1.0));
	// GuiGroup_particle.add(Particle_forceScale.setup("Force Scale", 0.00019, 0.0001, 0.001));
	GuiGroup_particle.add(Particle_forceScale.setup("Force Scale", 0.000122, 0.0001, 0.001));
	
	gui.add(&GuiGroup_particle);
}

/******************************
******************************/
void MY_GUI_COLOR::setup()
{
	/********************
	********************/
	gui.setup("color", "color.xml", 230, 10);
	
	/********************
	********************/
	{
		ofColor initColor = ofColor(255, 255, 255, 255);
		ofColor minColor = ofColor(0, 0, 0, 0);
		ofColor maxColor = ofColor(255, 255, 255, 255);
		gui.add(BackImageColor.setup("BackImageColor", initColor, minColor, maxColor));
	}
	{
		ofColor initColor = ofColor(255, 255, 255, 200);
		ofColor minColor = ofColor(0, 0, 0, 0);
		ofColor maxColor = ofColor(255, 255, 255, 255);
		gui.add(BarColor.setup("BarColor", initColor, minColor, maxColor));
	}
	{
		ofColor initColor = ofColor(255, 255, 255, 200);
		ofColor minColor = ofColor(0, 0, 0, 0);
		ofColor maxColor = ofColor(255, 255, 255, 255);
		gui.add(LogoColor.setup("LogoColor", initColor, minColor, maxColor));
	}
	{
		ofColor initColor = ofColor(255, 255, 255, 200);
		ofColor minColor = ofColor(0, 0, 0, 0);
		ofColor maxColor = ofColor(255, 255, 255, 255);
		gui.add(TimeStampColor.setup("TimeStamp", initColor, minColor, maxColor));
	}

	{
		ofVec4f initColor = ofVec4f(0.2, 0.5, 1.0, 0.5);
		ofVec4f minColor = ofVec4f(0, 0, 0, 0);
		ofVec4f maxColor = ofVec4f(1, 1, 1, 1);
		
		gui.add(ParticleColor.setup("particle", initColor, minColor, maxColor));
	}
}