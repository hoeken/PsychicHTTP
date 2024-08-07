#include "PsychicHttpsServer.h"

#ifdef CONFIG_ESP_HTTPS_SERVER_ENABLE

PsychicHttpsServer::PsychicHttpsServer() : PsychicHttpServer()
{
  //for a SSL server
  ssl_config = HTTPD_SSL_CONFIG_DEFAULT();
  ssl_config.httpd.open_fn = PsychicHttpServer::openCallback;
  ssl_config.httpd.close_fn = PsychicHttpServer::closeCallback;
  ssl_config.httpd.uri_match_fn = httpd_uri_match_wildcard;
  ssl_config.httpd.global_user_ctx = this;
  ssl_config.httpd.max_uri_handlers = 20;

  // each SSL connection takes about 45kb of heap
  // a barebones sketch with PsychicHttp has ~150kb of heap available
  // if we set it higher than 2 and use all the connections, we get lots of memory errors.
  // not to mention there is no heap left over for the program itself.
  ssl_config.httpd.max_open_sockets = 2;
}

PsychicHttpsServer::~PsychicHttpsServer() {}

esp_err_t PsychicHttpsServer::listen(uint16_t port, const char *cert, const char *private_key)
{
  this->_use_ssl = true;

  this->ssl_config.port_secure = port;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 2)
    this->ssl_config.servercert = (uint8_t *)cert;
    this->ssl_config.servercert_len = strlen(cert)+1;
#else
    this->ssl_config.cacert_pem = (uint8_t *)cert;
    this->ssl_config.cacert_len = strlen(cert)+1;
#endif

  this->ssl_config.prvtkey_pem = (uint8_t *)private_key;
  this->ssl_config.prvtkey_len = strlen(private_key)+1;

  return this->_start();
}

esp_err_t PsychicHttpsServer::_startServer()
{
  if (this->_use_ssl)
    return httpd_ssl_start(&this->server, &this->ssl_config);
  else
    return httpd_start(&this->server, &this->config);
}

esp_err_t PsychicHttpsServer::stop()
{
  esp_err_t ret = ESP_OK

  if (this->_use_ssl)
    ret = httpd_ssl_stop(this->server);
  else
    ret = httpd_stop(this->server);

  if(ret != ESP_OK)
  {
    ESP_LOGE(PH_TAG, "Server stop failed (%s)", esp_err_to_name(ESP_FAIL));
    return ret;
  }

  _started = false;
  ESP_LOGI(PH_TAG, "Server stopped");
  return ret;
}

#endif // CONFIG_ESP_HTTPS_SERVER_ENABLE