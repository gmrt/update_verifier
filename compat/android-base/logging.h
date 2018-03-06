#ifndef _LOG_H
#define _LOG_H

#include <cstring>
#include <iostream>
#include <sstream>

enum LogSeverity {
  VERBOSE,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL_WITHOUT_ABORT,
  FATAL,
};

class LOG {
public:
  LOG(enum LogSeverity severity) {
  }

  ~LOG() {
    std::cout << log_line_stream.str() << std::endl;
  }

  template<class T>
  LOG &operator<<(const T &x) {
    log_line_stream << x;
    return *this;
  }

protected:
  std::ostringstream log_line_stream;
};

class PLOG: public LOG {
public:
  PLOG(enum LogSeverity severity): LOG(severity) {
    log_errno = errno;
  }

  ~PLOG() {
    log_line_stream << ": " << std::strerror(errno);
  }

private:
  int log_errno;
};

#define CHECK(condition)                              \
  if (!(condition)) {                                 \
    LOG(FATAL) << "Check failed: " #condition << " "; \
    abort();                                          \
  }

#endif // _LOG_H
