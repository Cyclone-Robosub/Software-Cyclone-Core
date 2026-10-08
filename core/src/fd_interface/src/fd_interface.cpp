#include "fd_interface.hpp"

Path_FD::Path_FD(std::string path) : 
    FD_Interface(),
    path(path) 
     {}

int Path_FD::get_read_fd() {
    return fd;
}

int Path_FD::get_write_fd() {
    return fd;
}

void Path_FD::attempt_reconnect() {
    fd = open_file();
}

void Path_FD::close_fd() {
    if (fd >= 0) {
        close(fd);
    }
}

Path_FD::~Path_FD() {
    close_fd();
}

Direct_FD::Direct_FD(int fd_read, int fd_write) :
    FD_Interface()
     {
        this->fd = fd_read;
        this->fd_write = fd_write;
}

int Direct_FD::get_read_fd() {
    return fd;
}
int Direct_FD::get_write_fd() {
    return fd_write;
}
void Direct_FD::attempt_reconnect() {
    return;
}
void Direct_FD::close_fd() {
    if (fd >= 0) {
        close(fd);
    }
    if (fd_write >= 0 && fd_write != fd) {
        close(fd_write);
    }
}

Direct_FD::~Direct_FD() {
    close_fd();
}
