# dsps
dynamic and scalable parameter server  
可扩展的参数服务器(使用actor模型构建)  
使用前请先编译actor_framework  
最低编译器版本要求GCC 4.8 或Clang 3.4  
git clone https://github.com/actor-framework/actor-framework  
mkdir build && cd build  
cmake ..  
make  
make install  

接下来编译dsps  
编译  
make  
运行  
cd tests  
sh local.sh num_servers num_workers ./test_connection  
其中num_servers是参数服务器数目,num_workers是工作节点数目
