load("/third_party/grpc/grpc_proto", "proto_library")

proto_library(
  name = "piecer_proto",
  src = "piecer.proto"
)

cc_library(
  name = "piecer_impl",
  srcs = ["piecer.cc"],
  hdrs = ["piecer.h"],
  deps = [
    ":piecer_proto",
    "//util",
    "//third_party/boost:filesystem",
    "//third_party/glog",
    "//third_party/gflags",
  ]
)

cc_binary(
  name = "piecer",
  srcs = ["piecer_main.cc"],
  deps = [
    ":piecer_proto",
    ":piecer_impl",
    "//util",
    "//third_party/folly",
    "//third_party/glog",
    "//third_party/gflags",
  ]
)

cc_binary(
  name = "piecer_client",
  srcs = ["piecer_client.cc"],
  deps = [
    ":piecer_proto",
    "//util",
    "//third_party/glog",
    "//third_party/gflags",
  ]
)
