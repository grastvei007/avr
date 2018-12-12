#ifndef ADC_H
#define ADC_H


typedef void (*CallbackFunc)(float);



class Adc
{
	public:
		Adc();
		~Adc();
		void init();

		void enable();
		void disable();

		void setCallbackAdc0(CallbackFunc aCallbackFunc);

	private:
		CallbackFunc mCallbackAdc0;
};


#endif
