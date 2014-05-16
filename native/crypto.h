#ifndef __CRYPTO_H__
#define __CRYPTO_H__
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
