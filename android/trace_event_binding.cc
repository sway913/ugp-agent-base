// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <jni.h>

#include <set>

#include "base/android/jni_string.h"
#include "base/android/trace_event_binding.h"
#include "base/base_jni_headers/TraceEvent_jni.h"
#include "base/macros.h"
#include "base/trace_event/base_tracing.h"
#include "base/tracing_buildflags.h"

#if BUILDFLAG(ENABLE_BASE_TRACING)
#include "base/trace_event/trace_event_impl.h"  // no-presubmit-check
#endif  // BUILDFLAG(ENABLE_BASE_TRACING)

namespace base {
namespace android {

#if BUILDFLAG(ENABLE_BASE_TRACING)

namespace {

class TraceEnabledObserver
    : public trace_event::TraceLog::EnabledStateObserver {
 public:
  ~TraceEnabledObserver() override = default;

  // trace_event::TraceLog::EnabledStateObserver:
  void OnTraceLogEnabled() override {
    JNIEnv* env = base::android::AttachCurrentThread();
    base::android::Java_TraceEvent_setEnabled(env, true);
  }
  void OnTraceLogDisabled() override {
    JNIEnv* env = base::android::AttachCurrentThread();
    base::android::Java_TraceEvent_setEnabled(env, false);
  }
};

}  // namespace

static void JNI_TraceEvent_RegisterEnabledObserver(JNIEnv* env) {
  bool enabled = trace_event::TraceLog::GetInstance()->IsEnabled();
  base::android::Java_TraceEvent_setEnabled(env, enabled);
  trace_event::TraceLog::GetInstance()->AddOwnedEnabledStateObserver(
      std::make_unique<TraceEnabledObserver>());
}

static void JNI_TraceEvent_StartATrace(
    JNIEnv* env,
    const JavaParamRef<jstring>& category_filter) {
  std::string category_filter_utf8 =
      ConvertJavaStringToUTF8(env, category_filter);
  base::trace_event::TraceLog::GetInstance()->StartATrace(category_filter_utf8);
}

static void JNI_TraceEvent_StopATrace(JNIEnv* env) {
  base::trace_event::TraceLog::GetInstance()->StopATrace();
}

static void JNI_TraceEvent_SetupATraceStartupTrace(
    JNIEnv* env,
    const JavaParamRef<jstring>& category_filter) {
  std::string category_filter_utf8 =
      ConvertJavaStringToUTF8(env, category_filter);
  base::trace_event::TraceLog::GetInstance()->SetupATraceStartupTrace(
      category_filter_utf8);
}

#else  // BUILDFLAG(ENABLE_BASE_TRACING)

// Empty implementations when TraceLog isn't available.
static void JNI_TraceEvent_RegisterEnabledObserver(JNIEnv* env) {
  base::android::Java_TraceEvent_setEnabled(env, false);
}
static void JNI_TraceEvent_StartATrace(JNIEnv* env,
                                       const JavaParamRef<jstring>&) {}
static void JNI_TraceEvent_StopATrace(JNIEnv* env) {}
static void JNI_TraceEvent_SetupATraceStartupTrace(
    JNIEnv* env,
    const JavaParamRef<jstring>&) {}

#endif  // BUILDFLAG(ENABLE_BASE_TRACING)

namespace {

// Boilerplate for safely converting Java data to TRACE_EVENT data.
class TraceEventDataConverter {
 public:
  TraceEventDataConverter(JNIEnv* env, jstring jname, jstring jarg)
      : name_(ConvertJavaStringToUTF8(env, jname)),
        has_arg_(jarg != nullptr),
        arg_(jarg ? ConvertJavaStringToUTF8(env, jarg) : "") {}

  TraceEventDataConverter(const TraceEventDataConverter&) = delete;
  TraceEventDataConverter& operator=(const TraceEventDataConverter&) = delete;

  ~TraceEventDataConverter() = default;

  // Return saved values to pass to TRACE_EVENT macros.
  const char* name() { return name_.c_str(); }
  const char* arg_name() { return has_arg_ ? "arg" : nullptr; }
  const char* arg() { return has_arg_ ? arg_.c_str() : nullptr; }

 private:
  std::string name_;
  bool has_arg_;
  std::string arg_;
};

}  // namespace

static void JNI_TraceEvent_Instant(JNIEnv* env,
                                   const JavaParamRef<jstring>& jname,
                                   const JavaParamRef<jstring>& jarg) {
  TraceEventDataConverter converter(env, jname, jarg);
  if (converter.arg()) {
    TRACE_EVENT_INSTANT_WITH_FLAGS1(
        internal::kJavaTraceCategory, converter.name(),
        TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY |
            TRACE_EVENT_SCOPE_THREAD,
        converter.arg_name(), converter.arg());
  } else {
    TRACE_EVENT_INSTANT_WITH_FLAGS0(
        internal::kJavaTraceCategory, converter.name(),
        TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY |
            TRACE_EVENT_SCOPE_THREAD);
  }
}

static void JNI_TraceEvent_Begin(JNIEnv* env,
                                 const JavaParamRef<jstring>& jname,
                                 const JavaParamRef<jstring>& jarg) {
  TraceEventDataConverter converter(env, jname, jarg);
  if (converter.arg()) {
    TRACE_EVENT_BEGIN_WITH_FLAGS1(
        internal::kJavaTraceCategory, converter.name(),
        TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY,
        converter.arg_name(), converter.arg());
  } else {
    TRACE_EVENT_BEGIN_WITH_FLAGS0(
        internal::kJavaTraceCategory, converter.name(),
        TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY);
  }
}

static void JNI_TraceEvent_End(JNIEnv* env,
                               const JavaParamRef<jstring>& jname,
                               const JavaParamRef<jstring>& jarg) {
  TraceEventDataConverter converter(env, jname, jarg);
  if (converter.arg()) {
    TRACE_EVENT_END_WITH_FLAGS1(
        internal::kJavaTraceCategory, converter.name(),
        TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY,
        converter.arg_name(), converter.arg());
  } else {
    TRACE_EVENT_END_WITH_FLAGS0(
        internal::kJavaTraceCategory, converter.name(),
        TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY);
  }
}

static void JNI_TraceEvent_BeginToplevel(JNIEnv* env,
                                         const JavaParamRef<jstring>& jtarget) {
  std::string target = ConvertJavaStringToUTF8(env, jtarget);
  TRACE_EVENT_BEGIN_WITH_FLAGS0(
      internal::kToplevelTraceCategory, target.c_str(),
      TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY);
}

static void JNI_TraceEvent_EndToplevel(JNIEnv* env,
                                       const JavaParamRef<jstring>& jtarget) {
  std::string target = ConvertJavaStringToUTF8(env, jtarget);
  TRACE_EVENT_END_WITH_FLAGS0(
      internal::kToplevelTraceCategory, target.c_str(),
      TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY);
}

static void JNI_TraceEvent_StartAsync(JNIEnv* env,
                                      const JavaParamRef<jstring>& jname,
                                      jlong jid) {
  TraceEventDataConverter converter(env, jname, nullptr);
  TRACE_EVENT_NESTABLE_ASYNC_BEGIN_WITH_FLAGS0(
      internal::kJavaTraceCategory, converter.name(), TRACE_ID_LOCAL(jid),
      TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY);
}

static void JNI_TraceEvent_FinishAsync(JNIEnv* env,
                                       const JavaParamRef<jstring>& jname,
                                       jlong jid) {
  TraceEventDataConverter converter(env, jname, nullptr);
  TRACE_EVENT_NESTABLE_ASYNC_END_WITH_FLAGS0(
      internal::kJavaTraceCategory, converter.name(), TRACE_ID_LOCAL(jid),
      TRACE_EVENT_FLAG_JAVA_STRING_LITERALS | TRACE_EVENT_FLAG_COPY);
}

}  // namespace android
}  // namespace base
