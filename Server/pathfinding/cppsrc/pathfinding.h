#include <napi.h>

namespace pathfinding {
    std::string genPath(std::string pos, std::string dest, std::string obs);
    Napi::String GenPathWrapped(const Napi::CallbackInfo &info);

    Napi::Object Init(Napi::Env env, Napi::Object exports);
}