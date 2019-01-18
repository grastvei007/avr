#ifndef ADC_H
#define ADC_H


typedef void (*CallbackFunc)(float);



class Adc
{
	public:
		enum Channel
		{
			eAdc0 = 0,
			eAdc1,
			eAdc2,
			eAdc3,
			eAdc4,
			eAdc5
		};

		Adc();
		~Adc();
		void init();

		void enable();
		void disable();

		void enableChannel(Channel aChannel);
		void disableChannel(Channel aChannel);

		void setCallbackFunc(CallbackFunc aCallbackFuncm, Channel aChannel);

		bool isChannelEnabled(Channel aChannel);
		void setChannelValue(float aValue, Channel aChannel);
		Channel nextEnabledChannel();

		// ISR call
		void valueReady();

	private:
		void setAdmux();

	private:
		CallbackFunc mCallbackFunc[5];
		bool mChannel[5]; ///< enable/disable channels
		float mCurrentReading[5];
		Channel mCurrentChannel;

};


static Adc adc;


#endif
