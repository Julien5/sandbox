#include "wifi_curl.h"
#include "common/debug.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>

wifi::wifi_curl::wifi_curl(){};
wifi::wifi_curl::~wifi_curl(){};


int exe(const std::string &method, const char* req, wifi::callback * r,
	const uint8_t * data = nullptr, const int Ldata = 0) {
  // curl -s -X GET "http://example.com/" --output out
  std::string cmd = "curl -s -X " + method + " ";
  if (data && Ldata) {
    std::ofstream f;
    f.open("data.bin", std::ios::out | std::ios::binary | std::ios::trunc);
    f.write((char*)data,Ldata);
    f.close();
    cmd+="--data-binary \"@data.bin\"";
    cmd+=" ";
  }
  cmd += std::string(req);
  cmd += " --output out";
  DBG("exe: %s\n",cmd.c_str());
  std::system(cmd.c_str());
  std::ifstream f("out",std::ios::binary);
  if (!f.is_open()) {
    TRACE();
    r->status(1);
    r->crc(false);
    return 1;
  }
  TRACE();
  r->status(0);
  std::vector<char> out(std::istreambuf_iterator<char>(f), {});
  DBG("size:%d\n",int(out.size()));
  r->data_length(out.size());
  std::string line;
  while (!out.empty()) {
    const size_t L = xMin(8U,out.size());
    r->data(reinterpret_cast<uint8_t*>(out.data()),L);
    out.erase(out.begin(),out.begin()+L);
  }
  f.close();
  TRACE();
  std::remove("out");
  if (data && Ldata)
    std::remove("data.bin");
  r->crc(true);
  TRACE();
  return 0;
}

int wifi::wifi_curl::get(const char* req, callback * r) {
  return exe("GET",req,r);
}


int wifi::wifi_curl::post(const char* req, const uint8_t * data, const int Ldata, callback * r) {
  return exe("POST",req,r,data,Ldata);
}
