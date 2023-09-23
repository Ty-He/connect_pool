#include "mysql_conn.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory> // shared_ptr

namespace ty
{
    class connection_pool
    {
    public:
        static connection_pool* get_connect_pool();
        // 删除复制构造
        connection_pool(connection_pool&) = delete;
        connection_pool& operator=(connection_pool&) = delete;
        // 提供外部接口 , 取出一个可用连接
        std::shared_ptr<mysql_conn> get_connection();
        // 资源释放
        ~connection_pool();
    private:
        // 实现懒汉模式
        connection_pool();
        // 解析Json数据
        bool parse_json();
        // 生产/回收连接
        void produce_connection();
        void recycle_connection();
        void add_connection();

        std::string ip;
        unsigned short port;
        std::string user;
        std::string passwd;
        std::string dbname;
        // 存放数据库连接
        std::queue<mysql_conn*> connect_que;
        int min_size;
        int max_size;
        // 指定最多空闲的时间
        int freetime;
        // 指定线程阻塞的超时时长
        int timeout;

        // 线程操作
        std::mutex lock;
        std::condition_variable cond_prod;
        std::condition_variable cond_cons;

        // 连接池开关
        bool is_work;
    };
}