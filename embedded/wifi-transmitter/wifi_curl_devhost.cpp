#include "wifi_curl.h"
#include "common/debug.h"
#include "common/utils.h"
#include "common/stringawaiter.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <strings.h>
#include <string.h>
#include "common/platform.h"

wifi::wifi_curl::wifi_curl(){};
wifi::wifi_curl::~wifi_curl(){};

const char *kDataFile="/tmp/wifi.curloutput.internal";

size_t remain(size_t buffer_size, size_t pos) {
  if (pos>=buffer_size)
    return 0;
  return buffer_size-pos;
}

int exe(const std::string &method, const char* req, wifi::callback * cb,
	const uint8_t * data = nullptr, const int Ldata = 0) {
  // curl -s -X GET "http://example.com/" --output out
  std::string cmd = "curl -i --raw -s -X " + method + " ";
  if (data && Ldata) {
    DBG("%d\n",Ldata);
    std::ofstream f;
    f.open("data.bin", std::ios::out | std::ios::binary | std::ios::trunc);
    f.write((char*)data,Ldata);
    f.close();
    cmd+="--data-binary \"@data.bin\"";
    cmd+=" ";
  }
  cmd += std::string(req);
  cmd += " --output /tmp/wifi.curloutput.internal";
  DBG("exe: %s\n",cmd.c_str());
  std::system(cmd.c_str());
  auto f = fopen(kDataFile,"rb");
  auto code=errno;
  cb->status(code);
  if (!f) {
    return code;
  }
  
  uint8_t buffer[16*1024];
  memset(buffer,0,sizeof(buffer));
  size_t buffer_size = 0;
  while(true) {
    uint8_t recv_buf[16];
    memset(recv_buf, 0, sizeof(recv_buf));
    int r=fread(recv_buf, 1, sizeof(recv_buf), f);
    if (r>0) {
      if (buffer_size+r >= sizeof(buffer))
	break; // max size exceeded
      memcpy(buffer+buffer_size,recv_buf,r);
      buffer_size += r;
    } else {
      DBG("done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
      break;
    }
  }
  fclose(f);

  cb->data_length(buffer_size);

  // send chunk-wise so that arduino read buffer does not overflow.
  if (cb && buffer_size>0) {
    uint8_t buf[32];
    size_t pos=0;
    while(remain(buffer_size,pos)>0) {
      size_t size_copy = xMin(sizeof(buf),remain(buffer_size,pos));
      memcpy(buf,buffer+pos,size_copy);
      cb->data((uint8_t*)buf,size_copy);
      pos+=size_copy;
    }
  }
  std::remove(kDataFile);
  TRACE();
  if (data && Ldata) 
    std::remove("data.bin");
  TRACE();
  return 0;
}

int wifi::wifi_curl::get(const char* req, callback * r) {
  return exe("GET",req,r);
}


int wifi::wifi_curl::post(const char* req, const uint8_t * data, const uint16_t Ldata, callback * r) {
  return exe("POST",req,r,data,Ldata);
}
