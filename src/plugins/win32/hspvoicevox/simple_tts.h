#pragma once
#include <iostream>
#include "voicevox_core.h"

int tts_init(void);
void tts_term(void);
int tts_load(int id);
char* tts_getwav(char* utf8str, int speaker_id, int* size);
void tts_closewav(void);
const char *tts_geterror(void);
const char* tts_getversion(void);
const char* tts_getmetas(void);

/*
std::string GetOpenJTalkDict();
std::wstring GetWaveFileName();
std::wstring GetExePath();
std::wstring GetExeDirectory();
void OutErrorMessage(VoicevoxResultCode messageCode);
std::string wide_to_utf8_cppapi(std::wstring const& src);
std::wstring utf8_to_wide_cppapi(std::string const& src);
*/
