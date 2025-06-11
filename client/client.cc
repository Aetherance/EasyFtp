#include"client.h"
#include"fileInfo.pb.h"
#include<fstream>
#include"Buffer.h"
#include"fcntl.h"
#include<filesystem>

#define FTP_CONNECTED "\033[32mFtp\033[0m"
#define FTP_DISCONNECTED "Ftp"

FtpClient::FtpClient() : controlSocket_(::socket(AF_INET,SOCK_STREAM,0)),
                         serverAddr_(InetAddress("localhost", 6060))
{

}

FtpClient::~FtpClient() {

}

void FtpClient::connect() {
  ::connect(controlSocket_.fd(), (struct sockaddr *)&serverAddr_, sizeof(serverAddr_));
}

void FtpClient::uploadFile(const std::string &filePath, const std::string &remoteDir , const std::string &filename) {
  fileInfo info;
  info.set_action("UPLOAD");
  info.set_file_name(filename);
  info.set_user_dir(remoteDir);

  safeSend(info.SerializeAsString());
 
  int fd = ::open(filePath.data(),O_RDONLY);
  
  while (true) {
    std::vector<char> buff;
    buff.resize(1024);

    int n = ::read(fd,buff.data(),1024);

    if(n <= 0) {
      break;
    }

    ::send(dataSockFd_,buff.data(),n,0);
  }

  ::close(fd);
  ::close(dataSockFd_);

  isConnected_ = false;
}

void FtpClient::safeSend(const std::string & message) {
  uint32_t msg_len = static_cast<uint32_t>(message.size());
  uint32_t LengthHead = htonl(msg_len);
  ::send(controlSocket_.fd(), &LengthHead, sizeof(LengthHead), 0);
  ::send(controlSocket_.fd(),message.data(),message.size(),0);
}

std::string FtpClient::safeRecv() {
  int messageLength;
  std::string message;
  ::recv(controlSocket_.fd(),&messageLength,sizeof(messageLength),0);
  message.resize(messageLength);
  ::recv(controlSocket_.fd(),message.data(),message.size(),0);
  return message;
}

void FtpClient::downloadFile(const std::filesystem::path fileDir, const std::string &remoteDir , const std::string &filename) {
  const std::string filePath = fileDir/filename;
  fileInfo info;
  info.set_action("DOWNLOAD");
  info.set_file_name(filename);
  info.set_user_dir(remoteDir);

  safeSend(info.SerializeAsString());

  int fd = ::open(filePath.data(),O_WRONLY | O_CREAT | O_TRUNC,0644);
  
  int n = -1;

  while (true) {
    std::vector<char> buff;
    buff.resize(1024);

    n = ::recv(dataSockFd_,buff.data(),1024,0);

    if(n <= 0) {
      break;
    }

    ::write(fd,buff.data(),n);
  }

  ::close(fd);
  ::close(dataSockFd_);

  isConnected_ = false;
}

void FtpClient::getDataSock() {
  fileInfo info;
  info.set_action("CONNECT");

  safeSend(info.SerializeAsString());

  std::string back = safeRecv();

  fileInfo backInfo;
  backInfo.ParseFromString(back);

  int dataSockFd = ::socket(AF_INET,SOCK_STREAM,0);
  InetAddress PeerAddr("localhost",backInfo.port());
  sockaddr_in sin = PeerAddr.getSockAddr();
  int stat = ::connect(dataSockFd,(sockaddr*)&sin,sizeof(sin));
  if(stat < 0) {
    perror("::conncect");
  }

  dataSockFd_ = dataSockFd;
  isConnected_ = true;
}

std::vector<std::string> split(const std::string s,char ch)
{
  std::vector<std::string>result;
  int pos = 0;
  while (s[pos]==ch) {
    pos++;
  }
    
  while (pos< s.size()) {
    int n = 0;
    while (s[pos+n]!=ch&&pos+n<s.size()) {
      n++;
    }
    result.push_back(s.substr(pos,n));
    pos += n;
    while (s[pos] ==ch&&pos<s.size()) {
      pos++;
    }
  }
  return result;
}


void FtpClient::controller() {
  std::cout<<"输入 help 获取帮助\n";
  std::string cmd;
  while (true) {
    std::cout<< ( isConnected_ ? FTP_CONNECTED : FTP_DISCONNECTED ) <<": ";
    std::getline(std::cin,cmd);
    std::vector<std::string> cmds = split(cmd,' ');
    if(cmds[0] == "help") {
      help();
    } else if(cmds[0] == "pasv") {
      getDataSock();
    } else if(cmds[0] == "upload" && cmds.size() >= 4) {
      uploadFile(cmds[1],cmds[2],cmds[3]);
    } else if(cmds[0] == "download" && cmds.size() >= 4) {
      downloadFile(cmds[1],cmds[2],cmds[3]);
    } else if(cmds[0] == "exit") {
      break;
    }
  }
}

void FtpClient::help() {
  std::cout<<"输入 pasv 建立主动连接\n";
  std::cout<<"输入 upload <文件路径> <远程目录> <文件名> 上传文件\n";
  std::cout<<"输入 download <本地目录> <远程目录> <文件名> 下载文件\n";
  std::cout<<"输入 exit 退出\n";
}