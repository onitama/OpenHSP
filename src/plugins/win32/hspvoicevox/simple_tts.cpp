// simple_tts.cpp
//

#include "simple_tts.h"

#include <Windows.h>
#include <pathcch.h>
#include <shlwapi.h>
#include <string.h>

#include <array>
#include <codecvt>
#include <iostream>
#include <vector>
#include <fstream>

#include "voicevox_core.h"

#pragma comment(lib, "voicevox_core.lib")

#define OPENJTALK_DICT_NAME "open_jtalk_dic_utf_8-1.11"

static bool voicevox_ready = false;
static VoicevoxInitializeOptions options;
static VoicevoxResultCode result;
static unsigned int output_binary_size = 0;
static uint8_t* output_wav = NULL;
static const char* errorStr = NULL;


void tts_term(void)
{
    if (voicevox_ready) {
        voicevox_finalize();
        voicevox_ready = false;
    }
}

int tts_init(void)
{
    tts_term();
    options = voicevox_make_default_initialize_options();
    options.open_jtalk_dict_dir = OPENJTALK_DICT_NAME;
    options.acceleration_mode = VoicevoxAccelerationMode::VOICEVOX_ACCELERATION_MODE_CPU;
    options.load_all_models = false;
    result = voicevox_initialize(options);

    if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
        errorStr = voicevox_error_result_to_message(result);
        return -1;
    }
    voicevox_ready = true;
    return 0;
}

int tts_load(int id)
{
    if ( voicevox_ready == false ) return -1;
    uint32_t speaker_id = id;
    result = voicevox_load_model(speaker_id);
    if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
        errorStr = voicevox_error_result_to_message(result);
        return 1;
    }
    return 0;
}

char *tts_getwav(char* utf8str, int speaker_id, int *size)
{
    if (voicevox_ready == false) return NULL;
    tts_closewav();

    VoicevoxTtsOptions voptions = voicevox_make_default_tts_options();
    result = voicevox_tts(utf8str, speaker_id, voptions, &output_binary_size, &output_wav);
    if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
        errorStr = voicevox_error_result_to_message(result);
        *size = 0;
        return NULL;
    }
    *size = output_binary_size;
    return (char *)output_wav;
}

void tts_closewav(void)
{
    if (output_wav) {
        voicevox_wav_free(output_wav);
        output_wav = NULL;
    }
    output_binary_size = 0;
}

const char* tts_geterror(void)
{
    if (errorStr == NULL) return "";
    return errorStr;
}


const char* tts_getversion(void)
{
    if (voicevox_ready == false) return "";
    return voicevox_get_version();
}

const char* tts_getmetas(void)
{
    if (voicevox_ready == false) return "";
    return voicevox_get_metas_json();
}


#if 0
int main() {
  std::wcout.imbue(std::locale(""));
  std::wcin.imbue(std::locale(""));

  std::wcout << L"生成する音声の文字列を入力" << std::endl;
  std::wcout << L">";
  std::wstring speak_words;
  std::wcin >> speak_words;

  std::wcout << L"coreの初期化中" << std::endl;


  VoicevoxResultCode result = VoicevoxResultCode::VOICEVOX_RESULT_OK;

  VoicevoxInitializeOptions options = voicevox_make_default_initialize_options();
  options.open_jtalk_dict_dir = "open_jtalk_dic_utf_8-1.11";
  options.acceleration_mode = VoicevoxAccelerationMode::VOICEVOX_ACCELERATION_MODE_CPU;
  options.load_all_models = false;
  result = voicevox_initialize(options);

  if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
    std::wcout << L"coreの初期化に失敗しました" << std::endl;
    return 0;
  }

  uint32_t speaker_id = 3;

  result = voicevox_load_model(speaker_id);
  if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
      std::wcout << L"coreのvoicevox_load_model失敗しました" << std::endl;
      return 0;
  }

  std::wcout << L"音声生成中" << std::endl;
  unsigned int output_binary_size = 0;
  uint8_t* output_wav = nullptr;
  VoicevoxTtsOptions voptions = voicevox_make_default_tts_options();
  result = voicevox_tts(wide_to_utf8_cppapi(speak_words).c_str(), speaker_id, voptions, &output_binary_size, &output_wav);
  if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
    OutErrorMessage(result);
    return 0;
  }

  {
    //音声ファイルの保存
    std::ofstream out_stream(GetWaveFileName().c_str(), std::ios::binary);
    out_stream.write(reinterpret_cast<const char*>(output_wav), output_binary_size);
    std::wcout << GetWaveFileName() << L" に保存しました。" << std::endl;
  }  //ここでファイルが閉じられる

  std::wcout << L"音声再生中" << std::endl;
  PlaySound((LPCTSTR)output_wav, nullptr, SND_MEMORY);

  std::wcout << L"音声データの開放" << std::endl;
  voicevox_wav_free(output_wav);

}

/// <summary>
/// OpenJTalk辞書のパスを取得します。
/// </summary>
/// <returns>OpenJTalk辞書のパス</returns>
std::string GetOpenJTalkDict() {
  wchar_t buff[MAX_PATH] = {0};
  PathCchCombine(buff, MAX_PATH, GetExeDirectory().c_str(), OPENJTALK_DICT_NAME);
  std::string retVal = wide_to_utf8_cppapi(buff);
  return retVal;
}

/// <summary>
/// 音声ファイル名を取得します。
/// </summary>
/// <returns>音声ファイルのフルパス</returns>
std::wstring GetWaveFileName() {
  wchar_t buff[MAX_PATH] = {0};
  PathCchCombine(buff, MAX_PATH, GetExeDirectory().c_str(), L"speech.wav");
  return std::wstring(buff);
}

/// <summary>
/// 自分自身のあるパスを取得する
/// </summary>
/// <returns>自分のexeのフルパス</returns>
std::wstring GetExePath() {
  wchar_t buff[MAX_PATH] = {0};
  GetModuleFileName(nullptr, buff, MAX_PATH);
  return std::wstring(buff);
}

/// <summary>
/// 自分自身のあるディレクトリを取得する
/// </summary>
/// <returns>自分のexeのあるディレクトリ</returns>
std::wstring GetExeDirectory() {
  wchar_t buff[MAX_PATH] = {0};
  wcscpy_s(buff, MAX_PATH, GetExePath().c_str());
  //フルパスからファイル名の削除
  PathRemoveFileSpec(buff);
  return std::wstring(buff);
}

/// <summary>
/// コンソール画面にエラーメッセージを出力します。
/// </summary>
/// <param name="messageCode">メッセージコード</param>
void OutErrorMessage(VoicevoxResultCode messageCode) {
  const char* utf8Str = voicevox_error_result_to_message(messageCode);
  std::wstring wideStr = utf8_to_wide_cppapi(utf8Str);
  std::wcout << wideStr << std::endl;
}

/// <summary>
/// ワイド文字列をUTF8に変換します。
/// </summary>
/// <param name="src">ワイド文字列</param>
/// <returns>UTF8文字列</returns>
/// <remarks>
/// https://nekko1119.hatenablog.com/entry/2017/01/02/054629 から引用
/// </remarks>
std::string wide_to_utf8_cppapi(std::wstring const& src) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(src);
}

/// <summary>
/// UTF8をワイド文字に変換します。
/// </summary>
/// <param name="src">UTF8文字列</param>
/// <returns>ワイド文字列</returns>
/// <remarks>
/// https://nekko1119.hatenablog.com/entry/2017/01/02/054629 から引用
/// </remarks>
std::wstring utf8_to_wide_cppapi(std::string const& src) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(src);
}
#endif


