#pragma once
#include "../../../../Share/Boost/BoostLog.h"
#include <openssl/ssl.h>



class OpenSSLContext
{
public:

    OpenSSLContext() :m_SSLContext(NULL) {}

    ~OpenSSLContext()
    {
        if (m_SSLContext != NULL)
        {
            SSL_CTX_free(m_SSLContext);
            m_SSLContext = NULL;
        }
    }

    SSL_CTX* CreateSSLContext(const char *CertFile, const char *KeyFile, const char *CaFile)
    {
        if (m_SSLContext != NULL)
        {
            return m_SSLContext;
        }

        /* Load encryption & hashing algorithms for the SSL program */
        SSL_library_init();

        /* Load the error strings for SSL & CRYPTO APIs */
        SSL_load_error_strings();

        /* Create an SSL_METHOD structure (choose an SSL/TLS protocol version) */
        const SSL_METHOD *meth = SSLv23_server_method();

        /* Create an SSL_CTX structure */
        m_SSLContext = SSL_CTX_new(meth);
        if (NULL == m_SSLContext)
        {
            g_Log.WriteError("HTTP base server could not new SSL_CTX.");
            return NULL;
        }

        /* Load the CA cert file*/
        if (SSL_CTX_load_verify_locations(m_SSLContext, CaFile, NULL) <= 0)
        {
            g_Log.WriteError("HTTP base server could not load ca cert file.");
        }

        /* Load the server certificate into the SSL_CTX structure */
        if (SSL_CTX_use_certificate_file(m_SSLContext, CertFile, SSL_FILETYPE_PEM) <= 0)
        {
            g_Log.WriteError("HTTP base server could not use certificate file.");
        }

        /* Load the private-key corresponding to the client certificate */
        if (SSL_CTX_use_PrivateKey_file(m_SSLContext, KeyFile, SSL_FILETYPE_PEM) <= 0)
        {
            g_Log.WriteError("HTTP base server could not use private key file.");
        }

        /* Check if the client certificate and private-key matches */
        if (!SSL_CTX_check_private_key(m_SSLContext))
        {
            g_Log.WriteError("HTTP base server check private key does not match certfile.");
        }

        return m_SSLContext;
    }

private:

    SSL_CTX *m_SSLContext;
};
