#include "filedescriptor.h"

FileDescriptor::FileDescriptor(std::string filename)
    : m_filename(filename)
{
    m_ifstream.open(m_filename, std::ios::binary);
    if (m_ifstream.is_open()) {
        // get length of file:
        m_ifstream.seekg (0, m_ifstream.end);
        m_size = m_ifstream.tellg();
        m_ifstream.seekg (0, m_ifstream.beg);
    }
}

FileDescriptor::~FileDescriptor() {
    if (m_ifstream.is_open()) {
        m_ifstream.close();
    }
}

