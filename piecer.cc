#include "piecer/piecer.h"

#include <cstdio>
#include <cstdint>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "glog/logging.h"
#include "util/status.h"
namespace piecer {

namespace fs = boost::filesystem;

grpc::Status PiecerImpl::Read(grpc::ServerContext* context,
                              const ReadRequest* request,
                              ReadResponse* response) {
  util::Status read_status = ReadImpl(context, request, response);
  grpc::Status rpc_status;
  if (!read_status.ok()) {
    return grpc::Status(grpc::INTERNAL, read_status.error_message());
  }

  return grpc::Status::OK;
}

grpc::Status PiecerImpl::Write(grpc::ServerContext* context,
                              const WriteRequest* request,
                              WriteResponse* response) {
  util::Status write_status = WriteImpl(context, request, response);
  grpc::Status rpc_status;
  if (!write_status.ok()) {
    return grpc::Status(grpc::INTERNAL, write_status.error_message());
  }

  return grpc::Status::OK;
}

util::Status PiecerImpl::ReadImpl(grpc::ServerContext* context,
                                  const ReadRequest* request,
                                  ReadResponse* response) {
  fs::path request_path = request->path();
  if (!request_path.is_absolute()) {
    RETURN_ERROR(std::errc::invalid_argument) << "path must be absolute.";
  }

  fs::path actual_path = storage_root_ / request->path();
  FILE* file = fopen(actual_path.string().c_str(), "r");
  if (!file) {
    auto e = errno;
    if (errno == EACCES || errno == ENOENT) {
      response->set_error(ReadResponse::FILE_NOT_FOUND);
      RETURN_OK();
    } else {
      RETURN_ERROR(std::error_code(e, std::system_category()))
        << "Error during opening, errno: " << strerror(e);
    }
  }
  if (request->offset() != 0) {
    fseek(file, request->offset(), SEEK_SET);
  }

  std::vector<uint8_t> file_data(request->count());
  size_t count = fread(file_data.data(), sizeof(uint8_t), request->count(),
                       file);
  response->set_count(count);
  response->set_data(file_data.data(), count);
  if (feof(file)) {
    response->set_error(ReadResponse::END_OF_FILE);
  }
  fclose(file);

  RETURN_OK();
}

util::Status PiecerImpl::WriteImpl(grpc::ServerContext* context,
                                  const WriteRequest* request,
                                  WriteResponse* response) {
  fs::path request_path = request->path();
  if (!request_path.is_absolute()) {
    RETURN_ERROR(std::errc::invalid_argument) << "path must be absolute.";
  }

  fs::path actual_path = storage_root_ / request->path();
  FILE* file = nullptr;

  struct stat buf;
  int stat_code = stat(actual_path.string().c_str(), &buf);
  bool file_exists = false;
  if (stat_code < 0) {
    auto e = errno;
    if (e != ENOENT) {
      RETURN_ERROR(std::error_code(e, std::system_category()))
        << "Error during stat, errno: " << strerror(e);
    }
  } else {
    file_exists = true;
  }

  const char* mode = nullptr;
  if (request->mode() == WriteRequest::CREATE) {
    if (file_exists) {
      response->set_error(WriteResponse::FILE_EXISTS);
      RETURN_OK();
    }
    mode = "w";
  } else if (request->mode() == WriteRequest::APPEND) {
    if (!file_exists) {
      response->set_error(WriteResponse::FILE_NOT_FOUND);
      RETURN_OK();
    }
    mode = "a";
  }

  file = fopen(actual_path.string().c_str(), mode);
  if (!file) {
    auto e = errno;
    RETURN_ERROR(std::error_code(e, std::system_category()))
      << "Error during opening, errno: " << strerror(e);
  }

  const std::string& data = request->data();
  size_t count = fwrite(data.data(), sizeof(*data.data()), data.size(), file);
  if (count < data.size()) {
    auto e = errno;
    RETURN_ERROR(std::error_code(e, std::system_category()))
      << "Error during writing, errno: " << strerror(e);
  }

  fclose(file);
  RETURN_OK();
}

} // namespace piecer
