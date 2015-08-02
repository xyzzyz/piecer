#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/server_credentials.h>
#include <grpc++/status.h>

#include "folly/Format.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "piecer/piecer.grpc.pb.h"
#include "piecer/piecer.h"

DEFINE_string(host, "0.0.0.0", "Listening host of Piecer RPC service");
DEFINE_int32(port, 10000, "Listening port of Piecer RPC service");

DEFINE_string(storage_root, "",
              "The path to the directory where Piecer will store files.");

namespace piecer {

static void RunServer() {
  std::string server_address =
    folly::sformat("{}:{}", FLAGS_host, FLAGS_port);

  CHECK_NE(FLAGS_storage_root, "");
  PiecerImpl service(FLAGS_storage_root);

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
  FLAGS_logtostderr = true;
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  grpc_init();
  piecer::RunServer();

  grpc_shutdown();
  return 0;
}
