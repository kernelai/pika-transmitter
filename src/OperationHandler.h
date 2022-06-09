#pragma once
#include <wangle/channel/Handler.h>

#include <unordered_map>

#include "RespDecoder.h"

using namespace folly;
using namespace wangle;

enum class OperationType : uint8_t { READ, WRITE, ADMIN, NOT_ALLOW };

class Request {
 public:
  Request(OperationType type, std::string name) : type(type), name(name) {}

  OperationType type;
  const std::string name;
};

const std::unordered_map<std::string, OperationType> operationInfoMap = {
    {"GET", OperationType::READ},
    {"SET", OperationType::WRITE},
    {"AUTH", OperationType::ADMIN},
    {"PING", OperationType::ADMIN},
};

class OperationHandler
    : public InboundHandler<std::unique_ptr<RespCollection>, std::unique_ptr<Resp>> {
 public:
  void read(Context* ctx, std::unique_ptr<RespCollection> colls) override {
    std::for_each(colls->begin(), colls->end(),
                  [this, ctx](auto& coll) { ctx->fireRead(std::move(coll)); });
  }
};