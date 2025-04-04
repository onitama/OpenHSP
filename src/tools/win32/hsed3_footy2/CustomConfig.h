#ifndef __custom_config
#define __custom_config
	void GetINIFullPath(char *inipath);
	int GetINIForceFont(void);
	BOOL GetINICustomColor(void);
	void SetINIForceFont(BOOL f);
	void SetINICustomColor(int f);
	void SetINICustomString(char *key, char *value);

	void SetINICustomInt(int f, char *s);
	void SetINICustomBOOL(BOOL f, char *s);
	int GetINICustomInt(char *s);
	BOOL GetINICustomBOOL(char *s);
	DWORD GetINICustomString(char *key, char *value, int maxLength);
#endif