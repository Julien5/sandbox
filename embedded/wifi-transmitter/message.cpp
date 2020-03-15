#include "message.h"
#include <string.h>
#include "debug.h"
#include "utils.h"

namespace received {
  message::message() {
    memset(data,0,sizeof(data)/sizeof(data[0]));
  }

  template<typename T>
  void read(uint8_t * data, T * ret) {
    uint8_t * ret_addr = reinterpret_cast<uint8_t*>(ret);
    for(int i=0; i < sizeof(T); ++i) {
      ret_addr[i] = data[i];
    }
  }

  wifi_command read_wifi_command(const message &m) {
    wifi_command ret;
    uint8_t * cursor = m.data;
    read(cursor,&ret.command);
    cursor += sizeof(ret.command);
    ret.url = cursor;
    cursor += strlen(ret.url)+1;
    assert(m.length >= (strlen(ret.url)+2) );
    ret.Ldata = m.length - strlen(ret.url) - 2;
    cursor += 2;
    ret.data = cursor;
    return ret;
  }

  message make_message(const wifi_command &cmd) {
    message ret;
    ret.length = 1 + strlen(cmd.url) + 2 + cmd.Ldata;

    uint8_t * cursor = &(ret.data[0]);
    (*cursor) = (uint8_t) cmd.command;
    cursor++;

    strcpy(cursor,cmd.url);
    cursor += strlen(cmd.url)+1;

    (*cursor) = reinterpret_cast<const uint8_t*>(&cmd.Ldata)[0];
    cursor++;

    (*cursor) = reinterpret_cast<const uint8_t*>(&cmd.Ldata)[1];
    cursor++;
    
    if (cmd.Ldata>0) 
      memcpy(cursor,cmd.data,cmd.Ldata);
  
    return ret;
  }

  void test() {
    wifi_command cmd1;
    cmd1.command = 'G';
    cmd1.url = "http://foo.bar/blah";
    uint8_t block[16]={0};
    block[0]=0xFF;
    block[1]=0x11;
    block[2]=0x12;
    cmd1.data = block;
    cmd1.Ldata = sizeof(block);

    message m=make_message(cmd1);
    utils::dump(m.data,m.length);
    
    wifi_command cmd2 = read_wifi_command(m);
    DBG("%c %c\n",cmd1.command,cmd2.command);
    assert(cmd1.command == cmd2.command);
    
    DBG("%s %s\n",cmd1.url,cmd2.url);
    assert(strcmp(cmd1.url,cmd2.url)==0);

    assert(cmd1.Ldata = cmd2.Ldata);
    utils::dump(cmd1.data,cmd1.Ldata);
    utils::dump(cmd2.data,cmd2.Ldata);
    for(int i=0; i<cmd1.Ldata; ++i)
      assert(cmd1.data[i] == cmd2.data[i]);
  }
}
