#include "napi/native_api.h"
#include "ambient/detector.hpp"
#include <string>

static napi_value DetectEvents(napi_env env, napi_callback_info info) {
    // This is a stub for the HarmonyOS Native API bridge
    // In a real implementation, you'd pass audio buffers from JS to C++ here
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    std::string result = "Ambient Engine Active. Ready for audio buffers.";
    
    napi_value output;
    napi_create_string_utf8(env, result.c_str(), result.length(), &output);
    return output;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"detectEvents", nullptr, DetectEvents, nullptr, nullptr, nullptr, napi_default, nullptr}
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module ambientModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "ambient_bridge",
    .nm_priv = ((void*)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterAmbientModule(void) {
    napi_module_register(&ambientModule);
}
