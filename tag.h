#ifndef TAG_H
#define TAG_H


namespace Tag
{
	/** Create tags on the June server for values,
		that are input to the atmega. It means values sent
		to it over serial link.
	**/
	void createTag(const char *aName, bool aValue);
	void createTag(const char *aName, int aValue);
	void createTag(const char *aName, float aValue);
	void createTag(const char *aName, const char *aValue);

	/** Set value on tags

	**/
	void setValue(const char *aTagName, bool aValue);
	void setValue(const char *aTagName, int aValue);
	void setValue(const char *aTagName, float aValue);
	void setValue(const char *aTagName, const char *aValue);
};


#endif
