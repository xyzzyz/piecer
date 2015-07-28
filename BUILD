load("/third_party/grpc/grpc_proto", "proto_library")

proto_library(
  name = "piecer_proto",
  src = "piecer.proto"
)

cc_binary(
  name = "piecer",
  srcs = [
    "piecer_main.cc"
  ],
  deps = [
    ":piecer_proto",
    "//third_party/glog",
    "//third_party/gflags",
  ]
)
