#include "UbuntuMtpDatabase.h"

#include <MtpServer.h>
#include <MtpStorage.h>

#include <iostream>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace
{
struct FileSystemConfig
{
    static const int file_group = 32011; // phablet user.
    static const int file_perm = 0664;
    static const int directory_perm = 0755;
};

android::MtpStorage* home_storage = new android::MtpStorage(
    MTP_STORAGE_FIXED_RAM, 
    "/home/phablet",
    "Local storage", 
    1024 * 1024 * 100,  /* 100 MB reserved space, to avoid filling the disk */
    false,
    1024 * 1024 * 1024 * 2  /* 2GB arbitrary max file size */);
}

int main(int argc, char** argv)
{
    int fd = open("/dev/mtp_usb", O_RDWR);
    
    if (fd < 0)
    {
        std::cout << "Error opening /dev/mtp_usb, aborting now..." << std::endl;
    }
        
    std::shared_ptr<android::MtpServer> server
    {
        new android::MtpServer(
            fd, 
            new android::UbuntuMtpDatabase(),
            false, 
            FileSystemConfig::file_group, 
            FileSystemConfig::file_perm, 
            FileSystemConfig::directory_perm)
    };
    server->addStorage(home_storage);
    server->run();

    /*
      std::thread t{[&server](){ server->run(); }};

      sigset_t signal_set;
      sigemptyset(&signal_set);
      sigaddset(&signal_set, SIGINT);
      int signal;
      sigwait(&signal_set, &signal);

      if (t.joinable())
      t.join();
    */
    
}