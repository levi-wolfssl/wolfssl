#include <stdlib.h>
#include <stdint.h>

#include <wolfssl/options.h>
#include <wolfssl/ssl.h>

/* testing wolfSSL_CTX_use_certificate_buffer with PEM as the filetype*/

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t sz)
{
    WOLFSSL_CTX *ctx;
    int          ret;

    wolfSSL_Init();

    ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method());

    wolfSSL_CTX_load_verify_buffer(ctx, data, sz, SSL_FILETYPE_PEM);

    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    return 0;
}
