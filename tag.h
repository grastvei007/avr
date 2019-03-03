#ifndef TAG_H
#define TAG_H


namespace Tag
{
	/** Create tags on the June server for values,
		that are input to the atmega. It means values sent
		to it over serial link.
	**/
	void createTag(char *aName, bool aValue);
	void createTag(char *aName, int aValue);
	void createTag(char *aName, float aValue);
	void createTag(char *aName, char *aValue);

	/** Set value on tags

	**/
	void setValue(char *aTagName, bool aValue);
	void setValue(char *aTagName, int aValue);
	void setValue(char *aTagName, float aValue);
	void setValue(char *aTagName, char *aValue);
};


#endif
