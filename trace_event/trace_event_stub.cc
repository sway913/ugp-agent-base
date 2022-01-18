// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>

#include <string>

#include "base/memory/scoped_refptr.h"
#include "base/single_thread_task_runner.h"
#include "base/trace_event/trace_event_stub.h"

namespace base {
namespace trace_event {


const char MemoryAllocatorDump::kNameSize[] = "size";
const char MemoryAllocatorDump::kNameObjectCount[] = "object_count";
const char MemoryAllocatorDump::kTypeScalar[] = "scalar";
const char MemoryAllocatorDump::kTypeString[] = "string";
const char MemoryAllocatorDump::kUnitsBytes[] = "bytes";
const char MemoryAllocatorDump::kUnitsObjects[] = "objects";

ConvertableToTraceFormat::~ConvertableToTraceFormat() = default;

void TracedValue::AppendAsTraceFormat(std::string* out) const {}

MemoryDumpProvider::~MemoryDumpProvider() = default;
void MemoryDumpManager::RegisterDumpProvider(
    MemoryDumpProvider* mdp, const char* name,
    scoped_refptr<SingleThreadTaskRunner> task_runner) {}
void MemoryDumpManager::RegisterDumpProvider(
    MemoryDumpProvider* mdp, const char* name,
    scoped_refptr<SingleThreadTaskRunner> task_runner,
    MemoryDumpProvider::Options options) {}

// static
constexpr const char* const MemoryDumpManager::kTraceCategory;

}  // namespace trace_event
}  // namespace base

namespace perfetto {

TracedDictionary TracedValue::WriteDictionary() && {
  return TracedDictionary();
}

TracedArray TracedValue::WriteArray() && {
  return TracedArray();
}

TracedArray TracedDictionary::AddArray(StaticString) {
  return TracedArray();
}

TracedArray TracedDictionary::AddArray(DynamicString) {
  return TracedArray();
}

TracedDictionary TracedDictionary::AddDictionary(StaticString) {
  return TracedDictionary();
}

TracedDictionary TracedDictionary::AddDictionary(DynamicString) {
  return TracedDictionary();
}

TracedArray TracedArray::AppendArray() {
  return TracedArray();
}

TracedDictionary TracedArray::AppendDictionary() {
  return TracedDictionary();
}

}  // namespace perfetto
