#include "piecer/piecer.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/server_credentials.h>
#include <grpc++/status.h>

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(port, 10000, "Listening port of Piecer RPC service");

namespace piecer {

class PiecerImpl : public Piecer::Service {
 public:
  virtual grpc::Status Read(grpc::ServerContext* context,
                            const ReadRequest* request, 
                            ReadResponse* response) {
    return grpc::Status::OK;
  }

 private:
};

static void RunServer() {
  std::string server_address = "0.0.0.0:10000";
  PiecerImpl service;

  LOG(INFO) << "Server listening on " << server_address;
  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address,
                           grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();
}

}  // namespace examples

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  grpc_init();
  piecer::RunServer();

  grpc_shutdown();
  return 0;
}
