// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_TRACE_EVENT_STUB_H_
#define BASE_TRACE_EVENT_TRACE_EVENT_STUB_H_

#include <stddef.h>

#include <memory>
#include <string>

#include "base/base_export.h"
#include "base/strings/string_piece.h"
#include "base/trace_event/common/trace_event_common.h"
#include "base/trace_event/memory_allocator_dump_guid.h"
#include "base/values.h"

#define TRACE_STR_COPY(str) str
#define TRACE_ID_WITH_SCOPE(scope, ...) 0
#define TRACE_ID_GLOBAL(id) 0
#define TRACE_ID_LOCAL(id) 0

namespace trace_event_internal {

const unsigned long long kNoId = 0;

template <typename... Args>
void Ignore(Args&&... args) {}

struct IgnoredValue {
  template <typename... Args>
  IgnoredValue(Args&&... args) {}
};

}  // namespace trace_event_internal

#define INTERNAL_TRACE_IGNORE(...) \
  (false ? trace_event_internal::Ignore(__VA_ARGS__) : (void)0)

#define INTERNAL_TRACE_EVENT_ADD(...) INTERNAL_TRACE_IGNORE(__VA_ARGS__)
#define INTERNAL_TRACE_EVENT_ADD_SCOPED(...) INTERNAL_TRACE_IGNORE(__VA_ARGS__)
#define INTERNAL_TRACE_EVENT_ADD_WITH_ID(...) INTERNAL_TRACE_IGNORE(__VA_ARGS__)
#define INTERNAL_TRACE_TASK_EXECUTION(...) INTERNAL_TRACE_IGNORE(__VA_ARGS__)
#define INTERNAL_TRACE_LOG_MESSAGE(...) INTERNAL_TRACE_IGNORE(__VA_ARGS__)
#define INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW(...) \
  INTERNAL_TRACE_IGNORE(__VA_ARGS__)
#define INTERNAL_TRACE_EVENT_ADD_WITH_ID_TID_AND_TIMESTAMP(...) \
  INTERNAL_TRACE_IGNORE(__VA_ARGS__)
#define INTERNAL_TRACE_EVENT_ADD_WITH_ID_TID_AND_TIMESTAMPS(...) \
  INTERNAL_TRACE_IGNORE(__VA_ARGS__)

// Defined in application_state_proto_android.h
#define TRACE_APPLICATION_STATE(...) INTERNAL_TRACE_IGNORE(__VA_ARGS__)

#define TRACE_HEAP_PROFILER_API_SCOPED_TASK_EXECUTION \
  trace_event_internal::IgnoredValue

#define TRACE_ID_MANGLE(val) (val)

#define INTERNAL_TRACE_EVENT_GET_CATEGORY_INFO(cat) INTERNAL_TRACE_IGNORE(cat);
#define INTERNAL_TRACE_EVENT_CATEGORY_GROUP_ENABLED_FOR_RECORDING_MODE() false

#define TRACE_EVENT_API_CURRENT_THREAD_ID 0

// Typed macros. For these, we have to erase the extra args entirely, as they
// may include a lambda that refers to protozero message types (which aren't
// available in the stub). This may trigger "unused variable" errors at the
// callsite, which have to be addressed at the callsite (e.g. via
// ignore_result()).
#define TRACE_EVENT_BEGIN(category, name, ...) \
  INTERNAL_TRACE_IGNORE(category, name)
#define TRACE_EVENT_END(category, ...) INTERNAL_TRACE_IGNORE(category)
#define TRACE_EVENT(category, name, ...) INTERNAL_TRACE_IGNORE(category, name)
#define TRACE_EVENT_INSTANT(category, name, ...) \
  INTERNAL_TRACE_IGNORE(category, name)
#define PERFETTO_INTERNAL_ADD_EMPTY_EVENT() INTERNAL_TRACE_IGNORE()

template <class T>
class scoped_refptr;

namespace base {

class SingleThreadTaskRunner;

namespace trace_event {

BASE_EXPORT uint64_t GetNextGlobalTraceId();

class BASE_EXPORT ConvertableToTraceFormat {
 public:
  ConvertableToTraceFormat() = default;
  ConvertableToTraceFormat(const ConvertableToTraceFormat&) = delete;
  ConvertableToTraceFormat& operator=(const ConvertableToTraceFormat&) = delete;
  virtual ~ConvertableToTraceFormat();

  // Append the class info to the provided |out| string. The appended
  // data must be a valid JSON object. Strings must be properly quoted, and
  // escaped. There is no processing applied to the content after it is
  // appended.
  virtual void AppendAsTraceFormat(std::string* out) const = 0;
};

class BASE_EXPORT TracedValue : public ConvertableToTraceFormat {
 public:
  explicit TracedValue(size_t capacity = 0) {}

  void EndDictionary() {}
  void EndArray() {}

  void SetInteger(const char* name, int value) {}
  void SetDouble(const char* name, double value) {}
  void SetBoolean(const char* name, bool value) {}
  void SetString(const char* name, base::StringPiece value) {}
  void SetValue(const char* name, TracedValue* value) {}
  void BeginDictionary(const char* name) {}
  void BeginArray(const char* name) {}

  void SetIntegerWithCopiedName(base::StringPiece name, int value) {}
  void SetDoubleWithCopiedName(base::StringPiece name, double value) {}
  void SetBooleanWithCopiedName(base::StringPiece name, bool value) {}
  void SetStringWithCopiedName(base::StringPiece name,
                               base::StringPiece value) {}
  void SetValueWithCopiedName(base::StringPiece name, TracedValue* value) {}
  void BeginDictionaryWithCopiedName(base::StringPiece name) {}
  void BeginArrayWithCopiedName(base::StringPiece name) {}

  void AppendInteger(int) {}
  void AppendDouble(double) {}
  void AppendBoolean(bool) {}
  void AppendString(base::StringPiece) {}
  void BeginArray() {}
  void BeginDictionary() {}

  void AppendAsTraceFormat(std::string* out) const override;
};

class BASE_EXPORT TracedValueJSON : public TracedValue {
 public:
  explicit TracedValueJSON(size_t capacity = 0) : TracedValue(capacity) {}

  std::unique_ptr<base::Value> ToBaseValue() const { return nullptr; }
  std::string ToJSON() const { return ""; }
  std::string ToFormattedJSON() const { return ""; }
};

class BASE_EXPORT BlameContext {
 public:
  BlameContext(const char* category,
               const char* name,
               const char* type,
               const char* scope,
               int64_t id,
               const BlameContext* parent_context) {}

  void Initialize() {}
  void Enter() {}
  void Leave() {}
  void TakeSnapshot() {}

  const char* category() const { return nullptr; }
  const char* name() const { return nullptr; }
  const char* type() const { return nullptr; }
  const char* scope() const { return nullptr; }
  int64_t id() const { return 0; }
};

struct MemoryDumpArgs;
enum class MemoryDumpLevelOfDetail : uint32_t;

class BASE_EXPORT MemoryAllocatorDump {
 public:
  struct BASE_EXPORT Entry {};
  MemoryAllocatorDump(const std::string& absolute_name,
                      MemoryDumpLevelOfDetail,
                      const MemoryAllocatorDumpGuid&) {}
  MemoryAllocatorDump(const MemoryAllocatorDump&) = delete;
  MemoryAllocatorDump& operator=(const MemoryAllocatorDump&) = delete;
  ~MemoryAllocatorDump();
  static const char kNameSize[];
  static const char kNameObjectCount[];
  static const char kUnitsBytes[];
  static const char kUnitsObjects[];
  static const char kTypeScalar[];
  static const char kTypeString[];
  void AddScalar(const char* name, const char* units, uint64_t value) {}
  void AddString(const char* name,
                 const char* units,
                 const std::string& value) {}
  void AsValueInto(TracedValue* value) const {}
  uint64_t GetSizeInternal() const { return 0; }
  void set_flags(int flags) {}
  void clear_flags(int flags) {}
  int flags() const { return 0; }
};

class BASE_EXPORT ProcessMemoryDump {
 public:
  explicit ProcessMemoryDump(const MemoryDumpArgs& dump_args) {}
  ProcessMemoryDump(ProcessMemoryDump&&) {}
  ~ProcessMemoryDump() {}

  ProcessMemoryDump& operator=(ProcessMemoryDump&&);

  MemoryAllocatorDump* CreateAllocatorDump(const std::string& absolute_name) {
    return nullptr;
  }
  MemoryAllocatorDump* CreateAllocatorDump(
      const std::string& absolute_name,
      const MemoryAllocatorDumpGuid& guid) {
    return nullptr;
  }
};

class ProcessMemoryDump;

class BASE_EXPORT MemoryDumpProvider {
 public:
   struct Options {};
  MemoryDumpProvider(const MemoryDumpProvider&) = delete;
  MemoryDumpProvider& operator=(const MemoryDumpProvider&) = delete;
  virtual ~MemoryDumpProvider();

  virtual bool OnMemoryDump(const MemoryDumpArgs& args,
                            ProcessMemoryDump* pmd) = 0;

 protected:
  MemoryDumpProvider() = default;
};

class BASE_EXPORT MemoryDumpManager {
 public:
  static constexpr const char* const kTraceCategory =
      TRACE_DISABLED_BY_DEFAULT("memory-infra");

  static MemoryDumpManager* GetInstance() { return nullptr; }

  MemoryDumpManager(const MemoryDumpManager&) = delete;
  MemoryDumpManager& operator=(const MemoryDumpManager&) = delete;

  void RegisterDumpProvider(MemoryDumpProvider* mdp,
                            const char* name,
                            scoped_refptr<SingleThreadTaskRunner> task_runner);
  void RegisterDumpProvider(MemoryDumpProvider* mdp,
                            const char* name,
                            scoped_refptr<SingleThreadTaskRunner> task_runner,
                            MemoryDumpProvider::Options options);
  void UnregisterDumpProvider(MemoryDumpProvider* mdp) {}
  void UnregisterAndDeleteDumpProviderSoon(
      std::unique_ptr<MemoryDumpProvider> mdp) {}

 private:
  MemoryDumpManager() {}
  virtual ~MemoryDumpManager();      
};

class BASE_EXPORT TraceLog : public MemoryDumpProvider {
 public:
  static TraceLog* GetInstance() { return nullptr; }

  TraceLog(const TraceLog&) = delete;
  TraceLog& operator=(const TraceLog&) = delete;

  int process_id() const { return 0; }

 private:
  explicit TraceLog(int generation) {}
};
}  // namespace trace_event
}  // namespace base

// Stub implementation for
// perfetto::StaticString/ThreadTrack/TracedValue/TracedDictionary/TracedArray.
namespace perfetto {

namespace internal {
template <typename T>
class has_traced_value_support {};
}  // namespace internal

class TracedArray;
class TracedDictionary;

class StaticString {
 public:
  template <typename T>
  StaticString(T) {}
};

class DynamicString {
 public:
  template <typename T>
  explicit DynamicString(T) {}
};

class TracedValue {
 public:
  void WriteInt64(int64_t) && {}
  void WriteUInt64(uint64_t) && {}
  void WriteDouble(double) && {}
  void WriteBoolean(bool) && {}
  void WriteString(const char*) && {}
  void WriteString(const char*, size_t) && {}
  void WriteString(const std::string&) && {}
  void WritePointer(const void*) && {}

  TracedDictionary WriteDictionary() &&;
  TracedArray WriteArray() &&;
};

class TracedDictionary {
 public:
  TracedValue AddItem(StaticString) { return TracedValue(); }
  TracedValue AddItem(DynamicString) { return TracedValue(); }

  template <typename T>
  void Add(StaticString, T&&) {}
  template <typename T>
  void Add(DynamicString, T&&) {}

  TracedDictionary AddDictionary(StaticString);
  TracedDictionary AddDictionary(DynamicString);
  TracedArray AddArray(StaticString);
  TracedArray AddArray(DynamicString);
};

class TracedArray {
 public:
  TracedValue AppendItem() { return TracedValue(); }

  template <typename T>
  void Append(T&&) {}

  TracedDictionary AppendDictionary();
  TracedArray AppendArray();
};

template <class T>
void WriteIntoTracedValue(TracedValue, T&&) {}

template <typename T>
void WriteIntoTracedValueWithFallback(TracedValue context,
                                      T&& value,
                                      const std::string&) {}
                                      
}  // namespace perfetto

#endif  // BASE_TRACE_EVENT_TRACE_EVENT_STUB_H_
