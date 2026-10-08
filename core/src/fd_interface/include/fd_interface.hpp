#include <string>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#ifndef FD_INTERFACE
#define FD_INTERFACE

class FD_Interface {
protected:
    int fd;
public:
    explicit FD_Interface() {};
    virtual int get_read_fd() = 0;
    virtual int get_write_fd() = 0;
    virtual void attempt_reconnect() = 0;
    virtual void close_fd() = 0;
};

class Path_FD : public FD_Interface {
protected:
    std::string path;
    virtual int open_file() = 0;
public:
    explicit Path_FD(std::string path);
    virtual int get_read_fd() override;
    virtual int get_write_fd() override;
    virtual void attempt_reconnect() override;
    virtual void close_fd() override;
    virtual ~Path_FD();
};

class Direct_FD : public FD_Interface {
protected:
    int fd_write;
public:
    Direct_FD(int fd_read, int fd_write);
    virtual int get_read_fd() override;
    virtual int get_write_fd() override;
    virtual void attempt_reconnect() override;
    virtual void close_fd() override;
    virtual ~Direct_FD();
};

#endif // FD_INTERFACE