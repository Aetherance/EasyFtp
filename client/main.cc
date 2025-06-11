#include"client.h"

int main() {
  FtpClient client;
  client.connect();

  client.controller();

  return 0;
}