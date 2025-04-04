#ifndef __pack_icon_resource
#define __pack_icon_resource
	int StrReplace(char *buf, char *mae, char *ato);
	int StrReplaceALL(char *buf, char *mae, char *ato);
	int GetIconResourcePath(char *exepath, char *iconpath, char *respath, char *manifestpath/*manifest by Tetr@pod*/, int _activeFootyID, int *upx, int *lang, char *extension);
	BOOL _PathFileExists(LPCTSTR pszPath);
	BOOL _PathRemoveFileSpec(LPTSTR pszPath);
	void RunIconChange(int activeFootyID);
#endif