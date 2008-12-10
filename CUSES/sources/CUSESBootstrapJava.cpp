#include <exception>
#include <jni.h>
#include "j2pCCGS.hxx"
#include "CUSESBootstrap.hpp"

extern "C" { JWRAP_PUBLIC_PRE jobject Java_cellml_1bootstrap_CUSESBootstrap_createCUSESBootstrap(JNIEnv* env, jclass clazz) JWRAP_PUBLIC_POST; }

jobject
Java_cellml_1bootstrap_CUSESBootstrap_createCUSESBootstrap(JNIEnv* env, jclass clazz)
{
  return wrap_cellml_services_CUSESBootstrap(env, CreateCUSESBootstrap());
}