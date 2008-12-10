#include <exception>
#include <jni.h>
#include "j2pAnnoTools.hxx"
#include "AnnoToolsBootstrap.hpp"

extern "C" { JWRAP_PUBLIC_PRE jobject Java_cellml_1bootstrap_AnnoToolsBootstrap_createAnnotationToolService(JNIEnv* env, jclass clazz) JWRAP_PUBLIC_POST; }

jobject
Java_cellml_1bootstrap_AnnoToolsBootstrap_createAnnotationToolService(JNIEnv* env, jclass clazz)
{
  return wrap_cellml_services_AnnotationToolService(env, CreateAnnotationToolService());
}