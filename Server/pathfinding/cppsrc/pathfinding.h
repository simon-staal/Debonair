#include <napi.h>

namespace pathfinding {
    // Generates path as a JSON string {"points":[{"x":point1, "y":point1}, {"x":point2, "y":point2}, ect...]}
    std::string genPath(std::string pos, std::string dest, std::string obs);
    // Wrapper function to pass arguments / return value between node.js and genPath(), designates entry point
    Napi::String GenPathWrapped(const Napi::CallbackInfo &info);

    // Used to set export key with wrapped function
    Napi::Object Init(Napi::Env env, Napi::Object exports);
}