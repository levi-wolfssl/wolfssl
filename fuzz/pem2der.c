/* pem2der.c
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

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
