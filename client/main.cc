#include"client.h"

int main() {
  FtpClient client;
  client.connect();

  client.uploadFile("/home/ink/Code/FtpServer/client/CMakeLists.txt","ink","CMakeLists.txt");

  client.downloadFile("/home/ink/Code/FtpServer/test/","ink","CMakeLists.txt");
  
  return 0;
}