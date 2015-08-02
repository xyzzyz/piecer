#include <grpc/grpc.h>
#include <grpc++/channel_arguments.h>
#include <grpc++/channel_interface.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/credentials.h>
#include <grpc++/status.h>

#include "piecer/piecer.grpc.pb.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

using grpc::ChannelArguments;
using grpc::ChannelInterface;
using grpc::ClientContext;
using grpc::Status;

DEFINE_int32(port, 10000, "Listening port of Piecer RPC service.");
DEFINE_string(action, "", "read or write");
DEFINE_string(file, "", "File to ask for.");

// for read action
DEFINE_int64(offset, 0, "Offset to read from.");
DEFINE_int64(count, 0, "Number of bytes to read");

// for write action
DEFINE_string(data, "", "Bytes to write to the file.");
DEFINE_string(mode, "", "CREATE or APPEND");

namespace piecer_client {

static void RunClient() {
  LOG(INFO) << "Start the client.";
  std::shared_ptr<ChannelInterface>
    channel(grpc::CreateChannel("localhost:10000",
                                grpc::InsecureCredentials(),
                                ChannelArguments()));
  std::unique_ptr<piecer::Piecer::Stub> stub(piecer::Piecer::NewStub(channel));

  CHECK_NE(FLAGS_action, "");
  CHECK_NE(FLAGS_file, "");
  if (FLAGS_action == "read") {
    piecer::ReadRequest request;
    request.set_path(FLAGS_file);
    request.set_offset(FLAGS_offset);
    request.set_count(FLAGS_count);
    ClientContext context;
    piecer::ReadResponse response;
    Status status = stub->Read(&context, request, &response);
    if (status.IsOk()) {
      LOG(INFO) << "RPC response: count " << response.count()
                << " data: " << response.data() << ", error "
                << piecer::ReadResponse::ReadError_Name(response.error());
    } else {
      LOG(INFO) << "RPC failed: " << status.code() << " - " << status.details();
    }
  } else if (FLAGS_action == "write") {
    piecer::WriteRequest request;
    request.set_path(FLAGS_file);
    request.set_data(FLAGS_data);
    CHECK_NE(FLAGS_mode, "");
    if (FLAGS_mode == "CREATE") {
      request.set_mode(piecer::WriteRequest::CREATE);
    } else if (FLAGS_mode == "APPEND") {
      request.set_mode(piecer::WriteRequest::APPEND);
    } else {
      LOG(FATAL) << "Unknown mode: " << FLAGS_mode;
    }
    ClientContext context;
    piecer::WriteResponse response;
    Status status = stub->Write(&context, request, &response);
    if (status.IsOk()) {
      LOG(INFO) << "RPC response: error "
                << piecer::WriteResponse::WriteError_Name(response.error());
    } else {
      LOG(INFO) << "RPC failed: " << status.code() << " - " << status.details();
    }
  } else {
    LOG(FATAL) << "Unknown action: " << FLAGS_action;
  }
}

}  // namespace examples

int main(int argc, char** argv) {
  google::InstallFailureSignalHandler();
  FLAGS_logtostderr = true;
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  grpc_init();
  piecer_client::RunClient();

  grpc_shutdown();
  return 0;
}
