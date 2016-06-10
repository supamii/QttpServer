#ifndef __NATIVE_CRYPTO_H__
#define __NATIVE_CRYPTO_H__

#ifdef TO_BE_DECIDED
namespace native
{
    namespace crypto
    {
        class SecureContext
        {
        public:
            X509_STORE* ca_store_;
            SL_CTX* ctx_;

            static const int kMaxSessionSize = 10 * 1024;
        protected:
            SecureContext()
                : ca_store_(NULL)
                , ctx_(NULL)
            {}
        }
        template <class Base>
        class SSLWrap
        {
        public:
            enum Kind
            {
                kClient,
                kServer
            }
        }

    }
}
#endif

#endif
