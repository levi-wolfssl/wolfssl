#ifndef fuzz_fuzzer_h
#define fuzz_fuzzer_h
#ifdef __cplusplus
extern "C" {
#endif

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t sz);

#ifdef __cplusplus
}
#endif
#endif
