# pika-transmtter
pika-transmitter 是一个双写工具。在上线新版本时为了提供服务的高可用，通过transmitter双写数据到两个不同的集群中，从而使得当新集群遇到bug时，业务的请求可以转发到稳定版本集群中。

## 功能
对于redis的写请求，会被转发到backend的master、slave集群中。master response返回时，会把请求转发给客户端。slave的response会被忽略。