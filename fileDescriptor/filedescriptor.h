#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

#include <fstream>

class FileDescriptor
{
public:
    FileDescriptor(std::string filename);
    ~FileDescriptor();
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    std::string getFilename() const { return m_filename; }
    size_t getSize() const { return m_size; }
    std::ifstream& getIfstream() { return m_ifstream; }
    bool isOpen() const { return m_ifstream.is_open(); }
private:
    std::ifstream m_ifstream;
    size_t m_size{0}; //in bytes
    std::string m_filename;
};

#endif // FILEDESCRIPTOR_H
