#include <string>
#include <chrono>
#include <mysql.h>


namespace ty
{
    class mysql_conn
    {
    public:
        // 初始化数据库连接
        mysql_conn();
        // 清理资源
        ~mysql_conn();
        // 连接数据库
        bool connect(std::string user, std::string passwd, std::string db_name, std::string ip, unsigned int port = 3306);
        // 更新数据库 insert update delete
        bool update(std::string sql);
        // 查询数据库 select desc
        bool query(std::string sql);
        // 遍历查询得到结果集,即取出一条记录
        bool next(); 
        // 得到结果集中对应索引的字段值
        std::string value(int index);
        // 创建事务
        bool get_affairs();
        // 提交事务
        bool commit();
        // 回滚
        bool rollback();

        // 刷新开始空闲(停留在队列中)的时长
        void flushed_alive();
        long long get_freetime(); // ms
    private:
        MYSQL* mysql;
        MYSQL_RES* result = nullptr; // 结果集
        MYSQL_ROW record = nullptr; // 每一条记录
        void free_res();
        // 记录开始空闲时长
        std::chrono::steady_clock::time_point begin_point;
    };
}

