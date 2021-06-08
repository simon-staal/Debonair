#include <napi.h>
#include <vector>
#include "pathfinding.hpp"

// native C++ function that is assigned to 'getPath' property on 'exports' object
Napi::Object getPath(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if ()

    // input parameters
    std::pair<int,int> pos = info[0];
    std::pair<int,int> dest = info[1];
    std::vector<std::pair<int,int>> obstacles = info[2];


    // return new 'Napi::String' value
    return Napi::Oject::New(env, result);
}

Napi::Value Add(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 2) {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsNumber() || !info[1].IsNumber()) {
    Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
    return env.Null();
  }

  double arg0 = info[0].As<Napi::Number>().DoubleValue();
  double arg1 = info[1].As<Napi::Number>().DoubleValue();
  Napi::Number num = Napi::Number::New(env, arg0 + arg1);

  return num;
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