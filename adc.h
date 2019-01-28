#ifndef ADC_H
#define ADC_H



/** \brief Callback when there are changes on the adc reading.
**/

typedef void (*callbackFuncAdc)(int);



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

        void setCallbackFunc(callbackFuncAdc aCallbackFunc);

		bool isChannelEnabled(Channel aChannel);
		void setChannelValue(float aValue, Channel aChannel);
		Channel nextEnabledChannel();

        // get
        float getChannelReading(Channel aChannel);

		// ISR call
		void valueReady();

	private:
		void setAdmux();

	private:
		callbackFuncAdc mCallback;
		bool mChannel[5]; ///< enable/disable channels
		float mCurrentReading[5];
		Channel mCurrentChannel;

};


static Adc adc;


#endif
