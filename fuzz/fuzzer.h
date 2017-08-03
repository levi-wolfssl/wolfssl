#ifndef fuzz_fuzzer_h
#define fuzz_fuzzer_h

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t sz);

#endif
