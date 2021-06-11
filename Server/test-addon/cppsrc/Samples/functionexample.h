#include <napi.h>

namespace functionexample {
    std::string hello();
    Napi::String HelloWrapped(const Napi::CallbackInfo &info);

    int add(int a, int b);
    Napi::Number AddWrapped(const Napi::CallbackInfo &info);

    Napi::Object Init(Napi::Env env, Napi::Object exports);
}

namespace pathfinding {
    std::string genPath(std::string pos, std::string dest, std::string obs);
    Napi::String GenPathWrapped(const Napi::CallbackInfo &info);

    Napi::Object Init(Napi::Env env, Napi::Object exports);
}