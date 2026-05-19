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
#include <filesystem>
#include <fstream>

#include "voicevox_core.h"

#define OPENJTALK_DICT_NAME "open_jtalk_dic_utf_8-1.11"
#define MODEL_DIR_NAME "model"

#pragma comment(lib, "voicevox_core.lib")

//
// https://github.com/VOICEVOX/voicevox_core/blob/main/example/cpp/windows/simple_tts/simple_tts.cpp
//

static bool voicevox_ready = false;
static VoicevoxInitializeOptions options;
static VoicevoxLoadOnnxruntimeOptions load_ort_options;
static VoicevoxResultCode result;
static const VoicevoxOnnxruntime* onnxruntime;
static OpenJtalkRc* open_jtalk;
static VoicevoxSynthesizer* synthesizer;
static uintptr_t output_binary_size = 0;
static uint8_t* output_wav = NULL;
static const char* errorStr = NULL;


void tts_term(void)
{
    if (voicevox_ready) {
        voicevox_synthesizer_delete(synthesizer);
        voicevox_ready = false;
    }
}

int tts_init(void)
{
    tts_term();

    options = voicevox_make_default_initialize_options();
    //options.acceleration_mode = VoicevoxAccelerationMode::VOICEVOX_ACCELERATION_MODE_CPU;

    load_ort_options = voicevox_make_default_load_onnxruntime_options();
    result = voicevox_onnxruntime_load_once(load_ort_options, &onnxruntime);
    if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
        errorStr = voicevox_error_result_to_message(result);
        return -1;
    }
    result = voicevox_open_jtalk_rc_new(OPENJTALK_DICT_NAME, &open_jtalk);
    if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
        errorStr = voicevox_error_result_to_message(result);
        return -2;
    }
    result = voicevox_synthesizer_new(onnxruntime, open_jtalk, options, &synthesizer);
    if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
        errorStr = voicevox_error_result_to_message(result);
        return -3;
    }
    voicevox_open_jtalk_rc_delete(open_jtalk);
    voicevox_ready = true;
    return 0;
}

int tts_load(char * filename)
{
    if ( voicevox_ready == false ) return -1;
    static VoicevoxVoiceModelFile* model;

    result = voicevox_voice_model_file_open(filename, &model);
    if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
        errorStr = voicevox_error_result_to_message(result);
        return 1;
    }

    result = voicevox_synthesizer_load_voice_model(synthesizer, model);
    if (result != VoicevoxResultCode::VOICEVOX_RESULT_OK) {
        errorStr = voicevox_error_result_to_message(result);
        return 1;
    }
    voicevox_voice_model_file_delete(model);
    return 0;
}

char *tts_getwav(char* utf8str, int speaker_id, int *size)
{
    if (voicevox_ready == false) return NULL;
    tts_closewav();

    VoicevoxTtsOptions voptions = voicevox_make_default_tts_options();
    result = voicevox_synthesizer_tts(synthesizer, utf8str, speaker_id, voptions, &output_binary_size, &output_wav);
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
    return voicevox_synthesizer_create_metas_json(synthesizer);
}
