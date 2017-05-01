/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _soundStream_Input_DeviceId, int _soundStream_Output_DeviceId)
: soundStream_Input_DeviceId(_soundStream_Input_DeviceId)
, soundStream_Output_DeviceId(_soundStream_Output_DeviceId)
, b_DispGui(true)
, particle(PARTICLE_SET::getInstance())
, t_StartTimeStamp(-1)
{
}

/******************************
******************************/
ofApp::~ofApp()
{
}

//--------------------------------------------------------------
void ofApp::setup(){
	/********************
	********************/
	font.loadFont("HandyGeorge.ttf", 15);
	
	/********************
	********************/
	ofSetWindowTitle("FFT");
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetWindowShape(WIDTH, HEIGHT);
	ofSetEscapeQuitsApp(false);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	/********************
	memoryを確保は、app start後にしないと、
	segmentation faultになってしまった。
	********************/
	GlobalGui = new MY_GUI;
	GlobalGui_Col = new MY_GUI_COLOR;
	
	/********************
	********************/
	soundStream.listDevices();
	if( (soundStream_Input_DeviceId == -1) || (soundStream_Output_DeviceId == -1) ){
		ofExit();
		return;
	}
	// soundStream.setDeviceID(soundStream_DeviceId);
	/* set in & out respectively. */
	soundStream.setInDeviceID(soundStream_Input_DeviceId);  
	soundStream.setOutDeviceID(soundStream_Output_DeviceId);
	
	soundStream.setup(this, 2/* out */, 2/* in */, AUDIO_SAMPLERATE, AUDIO_BUF_SIZE, AUDIO_BUFFERS);
	
	AudioSample.resize(AUDIO_BUF_SIZE);
	
	/********************
	********************/
	img_back.load("stage.jpg");
	// img_back.load("stage.png");
	img_Logo.load("SJMAGIC_Logo.png");
	
	fft_Filter.setup();
	
	particle->setup();
}

/******************************
******************************/
void ofApp::exit()
{
	/********************
	ofAppとaudioが別threadなので、ここで止めておくのが安全.
	********************/
	soundStream.stop();
	soundStream.close();
}

/******************************
******************************/
void ofApp::save_GuiSetting()
{
	ofFileDialogResult res;
	res = ofSystemSaveDialog("GuiSetting.xml", "Save");
	if(res.bSuccess) GlobalGui->gui.saveToFile(res.filePath);
}

/******************************
******************************/
void ofApp::load_GuiSetting()
{
	ofFileDialogResult res;
	res = ofSystemLoadDialog("Load");
	if(res.bSuccess) GlobalGui->gui.loadFromFile(res.filePath);
}

/******************************
******************************/
void ofApp::save_GuiColor_Setting()
{
	ofFileDialogResult res;
	res = ofSystemSaveDialog("GuiColor_Setting.xml", "Save");
	if(res.bSuccess) GlobalGui_Col->gui.saveToFile(res.filePath);
}

/******************************
******************************/
void ofApp::load_GuiColor_Setting()
{
	ofFileDialogResult res;
	res = ofSystemLoadDialog("Load");
	if(res.bSuccess) GlobalGui_Col->gui.loadFromFile(res.filePath);
}

//--------------------------------------------------------------
void ofApp::update(){
	fft_Filter.lock();
	fft_Filter.update();
	fft_Filter.unlock();
	
	particle->update(1.0);
}

//--------------------------------------------------------------
void ofApp::draw(){

	/********************
	********************/
	float now = ofGetElapsedTimef();
	
	/********************
	********************/
	ofPushStyle();
	
		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		
		/********************
		********************/
		ofBackground(0);
		
		/********************
		********************/
		{
			ofColor color = GlobalGui_Col->BackImageColor;
			ofSetColor(color);
			img_back.draw(0, 0, ofGetWidth(), ofGetHeight());
		}
		
	ofPushMatrix();
		/********************
		********************/
		ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
		ofScale(1, -1, 1);
		
		/* */
		float dx = GlobalGui->TotalDispNoise_Amp * ofNoise(now/GlobalGui->TotalDispNoise_Speed + 1.2345);
		float dy = GlobalGui->TotalDispNoise_Amp * ofNoise(now/GlobalGui->TotalDispNoise_Speed + 16.23);
		ofTranslate(dx, dy);
		
		/* */
		if(GlobalGui->b_fftSync){
			fft_Filter.lock();
			float fft_scale = ofMap(fft_Filter.get_Gain(1), 0, GlobalGui->TotalDisp_fftSync_Amp, 1, 2.0);
			ofScale(fft_scale, fft_scale, 1);
			fft_Filter.unlock();
		}
		
		/********************
		********************/
		fft_Filter.lock();
		fft_Filter.draw();
		fft_Filter.unlock();
		
		if(GlobalGui->b_GainAdjust){
			ofSetColor(0, 0, 255, 200);
			ofDrawCircle(0, 0, GlobalGui->GraphRadius + GlobalGui->BarHeight);
		}
		
		
		ofPushStyle();
		ofPushMatrix();
		{
			ofEnableAlphaBlending();
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			
			ofScale(1, -1, 1);
			ofScale(GlobalGui->LogoScale, GlobalGui->LogoScale, 1);
			
			ofColor color = GlobalGui_Col->LogoColor;
			ofSetColor(color);
			img_Logo.setAnchorPercent(0.5, 0.5);
			img_Logo.draw(0, 0);
		}
		ofPopMatrix();
		ofPopStyle();
		
		/* */
		ofScale(1, -1, 1);
		if(0 <= t_StartTimeStamp){
			draw_time(now - t_StartTimeStamp);
		}
		ofScale(1, -1, 1);
	
	ofPopMatrix();
	ofPopStyle();
	
	/********************
	********************/
	particle->draw();
	
	/********************
	********************/
	if(b_DispGui){
		GlobalGui->gui.draw();
		GlobalGui_Col->gui.draw();
	}
}

/******************************
******************************/
void ofApp::draw_time(float now_sec)
{
	ofPushStyle();
	
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		
	/********************
	********************/
	int now_ms = now_sec * 1000;
	
	/********************
	********************/
	char buf[BUF_SIZE];
	
	int min	= now_ms / 1000 / 60;
	int sec	= now_ms / 1000 - min * 60;
	int ms	= now_ms % 1000;
	
	sprintf(buf, "%3d:%02d", min, sec);
	
	
	/********************
	********************/
	ofColor color = GlobalGui_Col->TimeStampColor;
	ofSetColor(color);
	font.drawString(buf, GlobalGui->DispTime_x, GlobalGui->DispTime_y);
	
	/*
	float w = font.stringWidth(buf);
	float h = font.stringHeight(buf);
	float x = ofGetWidth() / 2 - w / 2;
	float y = ofGetHeight() / 2 + h / 2;
	
	font.drawString(buf, x, y);
	*/
	
	ofPopStyle();
}

/******************************
audioIn/ audioOut
	同じthreadで動いている様子。
	また、audioInとaudioOutは、同時に呼ばれることはない(多分)。
	つまり、ofAppからaccessがない限り、変数にaccessする際にlock/unlock する必要はない。
	ofApp側からaccessする時は、threadを立てて、安全にpassする仕組みが必要(cf:NotUsed__thread_safeAccess.h)
******************************/
void ofApp::audioIn(float *input, int bufferSize, int nChannels)
{
    for (int i = 0; i < bufferSize; i++) {
        AudioSample.Left[i] = input[2*i];
		AudioSample.Right[i] = input[2*i+1];
    }
}  

/******************************
******************************/
void ofApp::audioOut(float *output, int bufferSize, int nChannels)
{
	/********************
	********************/
	float now_sec = ofGetElapsedTimef();
	
	/********************
	input -> output
	********************/
    for (int i = 0; i < bufferSize; i++) {
		/* */
		output[2*i] = AudioSample.Left[i];
		output[2*i+1] = AudioSample.Right[i];
    }
	
	/********************
	FFT Filtering
	1 process / block.
	********************/
	fft_Filter.lock();
	fft_Filter.update_fftGain(AudioSample.Left);
	fft_Filter.unlock();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 'a':
			if(t_StartTimeStamp == -1){
				t_StartTimeStamp = ofGetElapsedTimef();
			}else{
				t_StartTimeStamp = -1;
			}
			break;
			
		case 'c':
			save_GuiColor_Setting();
			break;
			
		case 'd':
			b_DispGui = !b_DispGui;
			break;
			
		case 'l':
			load_GuiSetting();
			break;
			
		case 's':
			save_GuiSetting();
			break;
			
		case 'm':
			load_GuiColor_Setting();
			break;
			
		case ' ':
			ofSaveScreen("image.png");
			printf("image saved\n");
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
