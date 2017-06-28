#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/ssl.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t sz)
{
    unsigned char buff[0x4000] = {0};
    int           buffSz       = sizeof(buff);

    wolfSSL_Init();

    wolfSSL_CertPemToDer(data, sz, buff, buffSz, CERT_TYPE);

    wolfSSL_Cleanup();

    return 0;
}
