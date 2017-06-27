#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <wolfssl/options.h>
#include <wolfssl/ssl.h>

/* wolfSSL certificate buffers */
#define  USE_CERT_BUFFERS_2048 /* for usual certs */
#define  USE_CERT_BUFFERS_256  /* for ECC certs */
#include <wolfssl/certs_test.h>

struct buffer {
    const uint8_t *buf;
    const uint8_t *cur;
    const uint8_t *end;
};

int Receive(WOLFSSL *ssl, char *out, int outSz, void *ctx)
{
    uint8_t       *buf   = (uint8_t *)out;
    ssize_t        bufSz = (ssize_t)outSz;
    struct buffer *in    = (struct buffer *)ctx;
    ssize_t        inSz  = (ssize_t)(in->end - in->cur);
    ssize_t        recvd = inSz < bufSz ? inSz : bufSz;

    if (recvd <= 0) goto error;

    memcpy(buf, in->cur, recvd);
    in->cur += recvd;

    return (int)recvd;

error:
    if (recvd == 0) {
        /* connection closed */
        return WOLFSSL_CBIO_ERR_CONN_CLOSE;
    }
    else {
        /* some other error */
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
}

int Send(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    (void)ssl;
    (void)buf;
    (void)sz;
    (void)ctx;

    /* there is no server to hear us */
    return sz;
}

int doit(struct buffer *in)
{
    int          ret = -1;
    char         buff[256];
    WOLFSSL_CTX *ctx = NULL;
    WOLFSSL     *ssl = NULL;

    if ((ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL) {
        goto error;
    }



    /* BEGIN LOADING CERTIFICATES */
    if (wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,
                                       sizeof_ca_cert_der_2048,
                                       SSL_FILETYPE_ASN1)
        != SSL_SUCCESS) {
        goto error;
    }
    /* END LOADING CERTIFICATES */



    wolfSSL_SetIORecv(ctx, Receive);
    wolfSSL_SetIOSend(ctx, Send);
    if ((ssl = wolfSSL_new(ctx)) == NULL) {
        goto error;
    }
    wolfSSL_SetIOReadCtx(ssl, in);
    wolfSSL_SetIOWriteCtx(ssl, in);

    if (wolfSSL_connect(ssl) != SSL_SUCCESS) {
        goto error;
    }

    /* just keep reading while there's something to read */
    while((ret = wolfSSL_read(ssl, buff, sizeof(buff)-1)) > 0);

    /* enforce return convention */
    if (ret) ret = -1;

error:
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    return ret;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t sz)
{
    struct buffer  in;

    wolfSSL_Init();

    in.buf = in.cur = data;
    in.end = in.buf + sz;

    doit(&in);

    wolfSSL_Cleanup();
    return 0;
}
