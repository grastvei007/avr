#ifndef TAG_H
#define TAG_H


namespace tag
{

void set_value(char *subsystem, char *name, int value);
void set_value(char *subsystem, char *name, float value);
void set_value(char *subsystem, char *name, bool value);

};


#endif
