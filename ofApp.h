/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"

#include "sj_common.h"
#include "fft.h"
#include "particle.h"

/************************************************************
************************************************************/

/**************************************************
**************************************************/
struct AUDIO_SAMPLE{
	vector<float> Left;
	vector<float> Right;
	
	void resize(int size){
		Left.resize(size);
		Right.resize(size);
	}
};

/**************************************************
**************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	
	/********************
	********************/
	int soundStream_Input_DeviceId;
	int soundStream_Output_DeviceId;
	ofSoundStream soundStream;
	
	AUDIO_SAMPLE AudioSample;
	
	FFT fft_Filter;
	
	ofImage img_back;
	ofImage img_Logo;
	
	ofTrueTypeFont font;
	
	PARTICLE_SET *particle;
	
	/********************
	********************/
	bool b_DispGui;
	float t_StartTimeStamp;

	
	/****************************************
	****************************************/
	void audioIn(float *input, int bufferSize, int nChannels);
	void audioOut(float *output, int bufferSize, int nChannels);
	
	void draw_time(float now_sec);
	
	void save_GuiSetting();
	void load_GuiSetting();
	
	void save_GuiColor_Setting();
	void load_GuiColor_Setting();


	
public:
	/****************************************
	****************************************/
	ofApp(int _soundStream_Input_DeviceId, int _soundStream_Output_DeviceId);
	~ofApp();
	
	void exit();
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
