/*This file is part of June.

June is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.*/


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

		void readNext();

        // get
        float getChannelReading(Channel aChannel);
		Channel getCurrentChannel() const;

		bool isDataReady() const;
		
		// ISR call
		void valueReady();

	private:
		void setAdmux();

	private:
		callbackFuncAdc mCallback;
		bool mChannel[5]; ///< enable/disable channels
		float mCurrentReading[5];
		Channel mCurrentChannel;
		volatile bool mDataReady;

};


//static Adc adc;


#endif
