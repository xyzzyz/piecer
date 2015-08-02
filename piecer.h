#ifndef PIECER_PIECER_H_
#define PIECER_PIECER_H_

#include "boost/filesystem.hpp"
#include "piecer/piecer.grpc.pb.h"
#include "util/status.h"

namespace piecer {

class PiecerImpl : public Piecer::Service {
 public:
  PiecerImpl(const boost::filesystem::path& storage_root)
    : storage_root_(storage_root) {}

  virtual grpc::Status Read(grpc::ServerContext* context,
                            const ReadRequest* request,
                            ReadResponse* response);

  virtual grpc::Status Write(grpc::ServerContext* context,
                             const WriteRequest* request,
                             WriteResponse* response);

private:
  util::Status ReadImpl(grpc::ServerContext* context,
                        const ReadRequest* request,
                        ReadResponse* response);

  util::Status WriteImpl(grpc::ServerContext* context,
                         const WriteRequest* request,
                         WriteResponse* response);

  const boost::filesystem::path& storage_root_;
};

} // namespace piecer

#endif // PIECER_PIECER_H_
