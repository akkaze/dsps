# dsps
dynamic and scalable parameter server
可扩展的参数服务器(使用actor模型构建)
编译
make
运行
cd tests
sh local.sh num_servers num_workers ./test_connection
其中num_servers是参数服务器数目,num_workers是工作节点数目
