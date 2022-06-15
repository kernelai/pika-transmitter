#pragma once
#include <wangle/channel/Handler.h>

#include <unordered_map>

#include "RespDecoder.h"

using namespace folly;
using namespace wangle;

enum class OperationType : uint8_t { READ, WRITE, ADMIN, NOT_ALLOW };

struct Request {
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

class RequestHandler : public InboundHandler<std::unique_ptr<RespCollection>,
                                               std::unique_ptr<Request>> {
 public:
  void read(Context* ctx, std::unique_ptr<RespCollection> colls) override {
    std::for_each(colls->begin(), colls->end(), [this, ctx](auto& coll) {
      auto request = createRequest(ctx, coll);
      if (request != nullptr) {
        ctx->fireRead(std::move(request));
      }
    });
  }

  /*
   * get operation type
   */
  OperationType getOperationType(const std::string& name) {
    auto it = operationInfoMap.find(name);
    if (it == operationInfoMap.end()) {
      return OperationType::NOT_ALLOW;
    }
    return it->second;
  }
  /*
   * get operation from resp
   */
  std::string getOperation(Context* ctx, std::unique_ptr<Resp>& resp) {
    if (resp->get()->type == REDIS_REPLY_STRING) {
      return std::string(resp->get()->str);
    }
    if (resp->get()->type == REDIS_REPLY_ARRAY) {
      if (resp->get()->elements != 0) {
        return std::string(resp->get()->element[0]->str);
      }
    }
    return std::string();
  }

  /*
   * create request from resp
   */
  std::unique_ptr<Request> createRequest(Context* ctx,
                                         std::unique_ptr<Resp>& resp) {
    auto operation = getOperation(ctx, resp);
    auto type = getOperationType(operation);
    if (type == OperationType::NOT_ALLOW) {
      fail(ctx, "not allow operation: " + operation);
      return nullptr;
    }
    return std::make_unique<Request>(type, operation);
  }

  void fail(Context* ctx, const std::string& msg) {
    ctx->fireReadException(folly::make_exception_wrapper<std::runtime_error>(
        "request handle error: " + msg));
  }
};