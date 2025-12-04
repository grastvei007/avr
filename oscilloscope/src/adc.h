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

namespace adc
{
    extern bool enabled_channels[];
    extern volatile float channel_value[];

    enum channel
    {
        Adc0 = 0,
        Adc1,
        Adc2,
        Adc3,
        Adc4,
        Adc5
    };

    void init();
    void enable(channel c);

} // end namespace

#endif
