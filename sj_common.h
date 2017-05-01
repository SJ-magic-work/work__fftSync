/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "stdio.h"

#include "ofMain.h"
#include "ofxGui.h"

/************************************************************
************************************************************/
#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);

enum{
	WIDTH = 1280,
	HEIGHT = 720,
};

enum{
	BUF_SIZE = 2000,
};

enum{
	AUDIO_BUF_SIZE = 512,
	
	AUDIO_BUFFERS = 2,
	AUDIO_SAMPLERATE = 44100,
};

enum{
	COMPRESS = 2,
};


/************************************************************
************************************************************/

/**************************************************
parameters
	ofxGuiGroup GuiGroup_Graph_Disp;
		ofxFloatSlider GraphRadius;
		ofxFloatSlider BarHeight;
		ofxFloatSlider BarWidth_Bottom;
		ofxFloatSlider BarWidth_Top;
		
		ofxToggle b_GainAdjust;
		ofxFloatSlider AudioSample_Amp;
			b_GainAdjustでBarHeightを表示し、これが埋まるようにAudio I/FとAudioSample_Ampを調整。
			その後、BarHeightで高さのmaxを決める。
			
	
	ofxGuiGroup GuiGroup_Graph_Filtering;
		ofxIntSlider CutOff_From, CutOff_To;
			fft結果の内、from <= [i] <= to の成分を残し、他をzero clearする。
			[i]について、
			44.1kHz, 512 sampleの場合、512sampleは、11.6ms.
			x[1], y[1]は、1/11.6ms = 86Hzの成分。
			x[2], y[2]は、86 x 2 Hzの成分。
			
		ofxFloatSlider k_Smoothing;
			fftで周波数軸に写影した後、Filteringを施してから再び時間軸に逆変換で戻す。
			これに掛けるLPFの係数。
			
		ofxFloatSlider NonLinear_Range;
			同じくNonLinear Filterに使用。
			zeroの時、NonLinear Filterなし。
			
		ofxFloatSlider k_Smoothing_Gain;
			周波数空間上でのGain用LPF係数.
	
	ofxGuiGroup GuiGroup_Graph_Animation;
		ofxToggle b_phaseRotation;
		ofxFloatSlider phase_deg;
		ofxFloatSlider phaseRotaion_Speed;
			fftで周波数軸に写影した後、Filteringを施してから再び時間軸に逆変換で戻す。
			ここで、Filteringを施す時、phaseを揃えることで、時間軸に戻した後のGraphを整えている。
			phaseが固定値だと、動きが少なくなるので、
				b_phaseRotation = on
			の時、phaseを回すことにした。
			
		ofxFloatSlider PhaseNoise_Amp;
		ofxFloatSlider PhaseNoise_Speed_sec;
			b_phaseRotation = onの時、phaseを揃えつつも、回転させていくが、やはり綺麗に回転していくだけだと、退屈感が出てしまう。
			そこでperlin noiseを使い、phaseを各瞬間で揺らすこととした。
			これらは、そのためのparameter.
			
			PhaseNoise_Speed_secは、この時間ごとにnoise Level = 0となる値。つまりnoise周期と考えることができる。
			大きくすると、noise変化が時間方向に緩やかになる。
	
	ofxGuiGroup GuiGroup_Logo;
		ofxFloatSlider LogoScale;
			Logo size.
			
		ofxIntSlider DispTime_x, ofxIntSlider DispTime_y;
			時間表示位置.
	
	ofxGuiGroup GuiGroup_Total_Animation;
		ofxFloatSlider TotalDispNoise_Amp;
		ofxFloatSlider TotalDispNoise_Speed;
			Logo, time, SoundSync Bar全体をperlin noiseで揺らす。
			
		ofxToggle b_fftSync;
		ofxFloatSlider TotalDisp_fftSync_Amp;
			fftGain[1]に連動.
	
	ofxGuiGroup GuiGroup_particle;
		ofxFloatSlider ParticleSize;
		ofxFloatSlider Particle_friction_DownPer_sec;
		ofxFloatSlider Particle_forceScale;
			小さくすると、動きが遅くなる。
			
		ofxFloatSlider ParticleSpeedThresh;
			速度が落ちるとAlphaが落ちる仕掛けをいれてある。
			ParticleSpeedThresh以下の時にlinearに暗くなっていく。
**************************************************/
class MY_GUI{
private:
public:
	/****************************************
	****************************************/
	void setup(int N);
	
	/****************************************
	****************************************/
	ofxPanel gui;
	
	/* */
	ofxGuiGroup GuiGroup_Graph_Disp;
	
	ofxFloatSlider GraphRadius;
	ofxFloatSlider BarHeight;
	ofxFloatSlider BarWidth_Bottom;
	ofxFloatSlider BarWidth_Top;
	
	ofxToggle b_GainAdjust;
	ofxFloatSlider AudioSample_Amp;
	
	/* */
	ofxGuiGroup GuiGroup_Graph_Filtering;
	ofxIntSlider CutOff_From;
	ofxIntSlider CutOff_To;
	ofxFloatSlider k_Smoothing;
	ofxFloatSlider NonLinear_Range;
	ofxFloatSlider k_Smoothing_Gain;
	
	/* */
	ofxGuiGroup GuiGroup_Graph_Animation;
	ofxToggle b_phaseRotation;
	ofxFloatSlider phase_deg;
	ofxFloatSlider phaseRotaion_Speed;
	
	ofxFloatSlider PhaseNoise_Amp;
	ofxFloatSlider PhaseNoise_Speed_sec;
	
	/* */
	// ofxColorSlider BarColor;
	
	/* */
	ofxGuiGroup GuiGroup_Logo;
	
	ofxFloatSlider LogoScale;
	ofxIntSlider DispTime_x;
	ofxIntSlider DispTime_y;
	
	/* */
	ofxGuiGroup GuiGroup_Total_Animation;
	ofxFloatSlider TotalDispNoise_Amp;
	ofxFloatSlider TotalDispNoise_Speed;
	
	ofxToggle b_fftSync;
	ofxFloatSlider TotalDisp_fftSync_Amp;
	
	/* */
	ofxGuiGroup GuiGroup_particle;
	
	ofxFloatSlider ParticleSize;
	ofxFloatSlider Particle_friction_DownPer_sec;
	ofxFloatSlider Particle_forceScale;
	// ofxVec4Slider ParticleColor;
	ofxFloatSlider ParticleSpeedThresh;
};

/**************************************************
**************************************************/
class MY_GUI_COLOR{
private:
public:
	/****************************************
	****************************************/
	void setup();
	
	/****************************************
	****************************************/
	ofxPanel gui;
	
	ofxColorSlider BackImageColor;
	
	ofxColorSlider BarColor;
	ofxColorSlider LogoColor;
	ofxColorSlider TimeStampColor;
	ofxVec4Slider ParticleColor;
};

/************************************************************
************************************************************/
extern MY_GUI *GlobalGui;
extern MY_GUI_COLOR * GlobalGui_Col;


