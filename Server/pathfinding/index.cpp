#include <napi.h>
#include <vector>
#include "pathfinding.hpp"

// native C++ function that is assigned to 'getPath' property on 'exports' object
Napi::Object getPath(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // input parameters
    std::vector<std::pair<int,int>> obstacles = {{1000,1450}, {2230,3100}, {2700,3600}, {3100,4450}, {3320,4550}};

    // return new 'Napi::String' value
    return Napi::Oject::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    
    // set a key on 'exports' object
    exports.Set(
        Napi::String::New(env, "getPath"), // property name => "getPath"
        Napi::Function::New(env, getPath) // property value => `getPath` function
    );
    
    // return exports object (always)
    return exports;
}

// register 'path' module which calls 'Init' method
NODE_API_MODULES(path, Init)