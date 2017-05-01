/************************************************************
************************************************************/
#include "fft.h"

/************************************************************
************************************************************/

/******************************
C++ Vector size is returning zero
	http://stackoverflow.com/questions/31372809/c-vector-size-is-returning-zero
	
	contents
		v1.reserve(30);
		does not change the number of elements in the vector, it just makes sure the vector has enough space to save 30 elements without reallocation.
		so now, v1.size() returns zero.
		
		Use
		v1.resize(30);
		to change to number of elements in the vector.
******************************/
FFT::FFT()
: N(AUDIO_BUF_SIZE / COMPRESS)
, LastInt(0)
{
	/********************
	********************/
#ifdef SJ_DEBUG__MEASTIME
	fp_Log = fopen("../../../Log.csv", "w");
#endif	
	
	/********************
	********************/
	VboVerts.resize(N * 4); // square
	VboColor.resize(N * 4); // square
	
	fft_Gain.resize(N);
	Last_fft_Gain.resize(N);

	AudioSample_Rev.resize(N);
	
	/* 窓関数 */
	fft_window.resize(N);
	for(int i = 0; i < N; i++)	fft_window[i] = 0.5 - 0.5 * cos(2 * PI * i / N);
	
	sintbl.resize(N + N/4);
	bitrev.resize(N);
	
	/********************
	********************/
	make_bitrev();
	make_sintbl();
}

/******************************
******************************/
FFT::~FFT()
{
	/********************
	********************/
#ifdef SJ_DEBUG__MEASTIME
	fclose(fp_Log);
#endif	
}

/******************************
******************************/
void FFT::setup()
{
	/********************
	********************/
	GlobalGui->setup(N);
	GlobalGui_Col->setup();
	
	/********************
	********************/
	for(int i = 0; i < fft_Gain.size(); i++)			fft_Gain[i] = 0;
	for(int i = 0; i < Last_fft_Gain.size(); i++)		Last_fft_Gain[i] = 0;
	for(int i = 0; i < AudioSample_Rev.size(); i++)		AudioSample_Rev[i] = 0;
	
	RefreshVerts();
	Refresh_BarColor();
	
	/********************
	********************/
	Vbo.setVertexData(&VboVerts[0], VboVerts.size(), GL_DYNAMIC_DRAW);
	Vbo.setColorData(&VboColor[0], VboColor.size(), GL_DYNAMIC_DRAW);
}

/******************************
******************************/
void FFT::RefreshVerts()
{
	/********************
	********************/
	const double d_theta = 360.0 / double(N);
	
	for(int i = 0; i < N; i++){
		AXIS axis;
		
		axis.rotate(i * d_theta, ofVec3f(0, 0, 1));
		axis.translate(0, GlobalGui->GraphRadius, 0);
		
		/********************
		わざと、尖らせる
		********************/
		ofVec3f target;
		
		axis.get(ofVec3f(-GlobalGui->BarWidth_Bottom/2, 0, 0), target);
		VboVerts[i * 4 + 0].set(target);

		axis.get(ofVec3f(-GlobalGui->BarWidth_Top/2, abs( ofMap(AudioSample_Rev[i], -GlobalGui->AudioSample_Amp, GlobalGui->AudioSample_Amp, -GlobalGui->BarHeight, GlobalGui->BarHeight)), 0), target);
		VboVerts[i * 4 + 1].set(target);
		
		axis.get(ofVec3f(GlobalGui->BarWidth_Top/2, abs( ofMap(AudioSample_Rev[i], -GlobalGui->AudioSample_Amp, GlobalGui->AudioSample_Amp, -GlobalGui->BarHeight, GlobalGui->BarHeight)), 0), target);
		VboVerts[i * 4 + 2].set(target);
		
		axis.get(ofVec3f(GlobalGui->BarWidth_Bottom/2, 0, 0), target);
		VboVerts[i * 4 + 3].set(target);
	}
}

/******************************
******************************/
void FFT::Refresh_BarColor()
{
	ofColor color = GlobalGui_Col->BarColor;
	for(int i = 0; i < VboColor.size(); i++) { VboColor[i].set( double(color.r)/255, double(color.g)/255, double(color.b)/255, double(color.a)/255); }
}

/******************************
******************************/
float FFT::get_Gain(int id)
{
	return fft_Gain[id];
}

/******************************
******************************/
void FFT::update()
{
	/********************
	********************/
	RefreshVerts();
	Refresh_BarColor();
	
	/********************
	以下は、audioOutからの呼び出しだと segmentation fault となってしまった.
	********************/
	Vbo.updateVertexData(&VboVerts[0], VboVerts.size());
	Vbo.updateColorData(&VboColor[0], VboColor.size());
}

/******************************
******************************/
float FFT::cal_vector_ave(const vector<float> &data, int from, int num)
{
	/********************
	********************/
	if(num == 0) return 0;
	
	/********************
	********************/
	float sum = 0;
	
	for(int i = from; i < from + num; i++){
		sum += data[i];
	}
	
	return sum / num;
}

/******************************
******************************/
void FFT::update_fftGain(const vector<float> &AudioSample)
{
	/********************
	********************/
	float now = ofGetElapsedTimef();
	
#ifdef SJ_DEBUG__MEASTIME
	fprintf(fp_Log, "%f,", now);
#endif
	
	float dt = ofClamp(now - LastInt, 0, 0.1);
	
	if(GlobalGui->b_phaseRotation){
		GlobalGui->phase_deg = GlobalGui->phase_deg + GlobalGui->phaseRotaion_Speed * dt;
		if(360 <= GlobalGui->phase_deg) GlobalGui->phase_deg = GlobalGui->phase_deg - 360;
	}
	
	/********************
	********************/
	if(AudioSample.size() != N * COMPRESS) { ERROR_MSG(); ofExit(); }
	
	/********************
	********************/
	double x[N], y[N];
	
	for(int i = 0; i < N; i++){
		x[i] = cal_vector_ave(AudioSample, i * COMPRESS, COMPRESS) * fft_window[i];
		y[i] = 0;
	}
	
	fft(x, y);
	
	/********************
	********************/
	int _CutOff_From	= GlobalGui->CutOff_From;
	int _CutOff_To		= GlobalGui->CutOff_To;
	if(_CutOff_To < _CutOff_From){
		_CutOff_To = _CutOff_From;
		GlobalGui->CutOff_To = _CutOff_From; // CutOff_To = CutOff_From; とすると、何故かクラッシュ
	}
	
	double phase_Noise;
	if(GlobalGui->PhaseNoise_Speed_sec == 0)	phase_Noise = 0;
	else										phase_Noise = GlobalGui->PhaseNoise_Amp * ofNoise(now / GlobalGui->PhaseNoise_Speed_sec + 1.234);
	
	x[0] = 0; y[0] = 0;
	x[N/2] = 0; y[N/2] = 0;
	for(int i = 1; i < N/2; i++){
		fft_Gain[i] = sqrt(x[i] * x[i] + y[i] * y[i]);
		
		if( (_CutOff_From <= i) && (i <= _CutOff_To) ){
			x[i] = fft_Gain[i] * cos( deg2rad(GlobalGui->phase_deg + phase_Noise) );
			y[i] = fft_Gain[i] * sin( deg2rad(GlobalGui->phase_deg + phase_Noise) );
			
			/********************
			共役
			********************/
			x[N - i] = x[i];
			y[N - i] = -y[i];
			
		}else{
			x[i] = 0;		y[i] = 0;
			x[N - i] = 0;	y[N - i] = 0;
		}
	}
	
	fft(x, y, true); // reverse to time.
	
	
	/********************
	********************/
	/* Gain of Freq */
	for(int i = 0; i < fft_Gain.size(); i++){
		fft_Gain[i] = (1 - GlobalGui->k_Smoothing_Gain) * Last_fft_Gain[i] + GlobalGui->k_Smoothing_Gain * fft_Gain[i];
		Last_fft_Gain[i] = fft_Gain[i];
	}
	
	/* TimeBased */
	for(int i = 0; i < AudioSample_Rev.size(); i++){
		float _AudioSample_Rev;
		
		/* LPF */
		_AudioSample_Rev = (1 - GlobalGui->k_Smoothing) * AudioSample_Rev[i] + GlobalGui->k_Smoothing * x[i];
		
		/* Non Linear */
		if(GlobalGui->NonLinear_Range == 0){
			AudioSample_Rev[i] = _AudioSample_Rev;
		}else{
			float diff =  _AudioSample_Rev - AudioSample_Rev[i];
			const double p = GlobalGui->AudioSample_Amp * GlobalGui->NonLinear_Range;
			if(p == 0) { ERROR_MSG(); ofExit(); return; }
			const double k = 1/p;
			if( (0 < diff) && (diff < p) ){
				diff = k * diff * diff;
			}else if( (-p < diff) && (diff < 0) ){
				diff = -k * diff * diff;
			}
			
			AudioSample_Rev[i] = AudioSample_Rev[i] + diff;
		}
	}
	
	/********************
	********************/
	LastInt = now;
	
#ifdef SJ_DEBUG__MEASTIME
	fprintf(fp_Log, "%f\n", ofGetElapsedTimef());
#endif
}

/******************************
******************************/
double FFT::deg2rad(double deg)
{
	return deg * PI / 180;
}

/******************************
******************************/
void FFT::draw()
{
	/********************
	********************/
	ofPushStyle();
	ofPushMatrix();
	
		/********************
		********************/
		glPointSize(1.0);
		glLineWidth(1);
		Vbo.draw(GL_QUADS, 0, VboVerts.size());
		
	ofPopMatrix();
	ofPopStyle();
}

/******************************
******************************/
void FFT::keyPressed(int key){
}

/******************************
******************************/
void FFT::threadedFunction()
{
	while(isThreadRunning()) {
		lock();
		
		unlock();
		
		sleep(THREAD_SLEEP_MS);
	}
}

/******************************
******************************/
int FFT::fft(double x[], double y[], int IsReverse)
{
	/*****************
		bit反転
	*****************/
	int i, j;
	for(i = 0; i < N; i++){
		j = bitrev[i];
		if(i < j){
			double t;
			t = x[i]; x[i] = x[j]; x[j] = t;
			t = y[i]; y[i] = y[j]; y[j] = t;
		}
	}

	/*****************
		変換
	*****************/
	int n4 = N / 4;
	int k, ik, h, d, k2;
	double s, c, dx, dy;
	for(k = 1; k < N; k = k2){
		h = 0;
		k2 = k + k;
		d = N / k2;

		for(j = 0; j < k; j++){
			c = sintbl[h + n4];
			if(IsReverse)	s = -sintbl[h];
			else			s = sintbl[h];

			for(i = j; i < N; i += k2){
				ik = i + k;
				dx = s * y[ik] + c * x[ik];
				dy = c * y[ik] - s * x[ik];

				x[ik] = x[i] - dx;
				x[i] += dx;

				y[ik] = y[i] - dy;
				y[i] += dy;
			}
			h += d;
		}
	}

	/*****************
	*****************/
	if(!IsReverse){
		for(i = 0; i < N; i++){
			x[i] /= N;
			y[i] /= N;
		}
	}

	return 0;
}

/******************************
******************************/
void FFT::make_bitrev(void)
{
	int i, j, k, n2;

	n2 = N / 2;
	i = j = 0;

	for(;;){
		bitrev[i] = j;
		if(++i >= N)	break;
		k = n2;
		while(k <= j)	{j -= k; k /= 2;}
		j += k;
	}
}

/******************************
******************************/
void FFT::make_sintbl(void)
{
	for(int i = 0; i < N + N/4; i++){
		sintbl[i] = sin(2 * PI * i / N);
	}
}


