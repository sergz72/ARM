#include <crypto.h>
#include <net_client.h>

const unsigned char default_aes_key[] =
    {};
esp_aes_context aes_ctx;

void crypto_init(void)
{
  esp_aes_init(&aes_ctx);
  esp_aes_setkey(&aes_ctx, server_params.aes_key, 128);
}
