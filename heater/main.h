//
// Created by steini on 2/3/21.
//

#ifndef HEATER_MAIN_H
#define HEATER_MAIN_H
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <string.h>

#include <messagehandler.h>
#include <messagetranslationsenter.h>
#include <message.h>
#include <usart.h>
#include <pwm.h>
#include <tag.h>
#include <adc.h>

#include "pump.h"

MessageHandler mh;
MessageTranslationSenter mts;
Pwm pwm;
Pump pump;
Adc adc;


struct Motor //Fan
{
    volatile int newLevel;
    volatile bool changed;
    volatile int currentLevel;
    int minLevel;
    int maxLevel;
    int pwmMin;
    int pwmMax;
};

Motor fan;
Motor effect;

enum State
{
    eInit,
    eStarting,
    eRunning,
    eStopping,
    eStopped,
    eSendTags
};

volatile State state = eInit;
volatile State nextState = eInit;
volatile State returnState = eInit;
volatile bool lock;
volatile int tagNumber = 0;
volatile int updateTags = 0;
volatile bool on = false;
volatile int counter = 0;
volatile int adc0Value = 0;

int numPrePumps = 250; // 25 pumps
volatile bool isBurning = false;


void init(); ///< init fan, and effect.
void initTimer();
void turnGlowPlugOn(bool aOn);

void writeStateTag();

void messageCallback(Message *&msg);
void requestDeviceName();
void requestCreateTags();

void onBoolValueChanged(char *aKey, bool aValue);
void onIntValueChanged(char *aKey, int aValue);
void onAdcValueChanged(int aValue);


#endif //HEATER_MAIN_H
