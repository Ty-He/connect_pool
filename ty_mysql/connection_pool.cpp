#include <fstream>
#include <json/json.h>
#include "connection_pool.h"

ty::connection_pool::connection_pool()
{
    this->is_work = true;
    if(!this->parse_json()) return;
    // 添加连接
    for(int i = 0; i < this->min_size; ++i)
        add_connection();
    // 添加工作线程
    std::thread producer(ty::connection_pool::produce_connection, this);
    std::thread manager(ty::connection_pool::recycle_connection, this);
    producer.detach();
    manager.detach();
}

ty::connection_pool* ty::connection_pool::get_connect_pool()
{
    // C++11之后线程安全
    static connection_pool pool;
    return &pool;
}

// 这里要用到Jsoncpp库
bool ty::connection_pool::parse_json()
{
    std::ifstream ifs("user.json");
    Json::Reader rd;
    Json::Value root; // 存放取出的数据
    if(!rd.parse(ifs, root)) return false;
    ifs.close();
    if(root.isObject())
    {
        this->ip = root["ip"].asString();
        this->port = root["port"].asInt();
        this->user = root["user"].asString();
        this->passwd = root["passwd"].asString();
        this->dbname = root["dbname"].asString();
        this->max_size = root["max_size"].asInt();
        this->min_size = root["min_size"].asInt();
        this->freetime = root["freetime"].asInt();
        this->timeout = root["timeout"].asInt();
    }
    return true;
}

void ty::connection_pool::produce_connection()
{
    // 一直运行
    while(this->is_work)
    {
        std::unique_lock<std::mutex> locker(this->lock); 
        this->cond_prod.wait(locker, [this](){
            return this->connect_que.size() < this->min_size; 
        }); // lambda 返回true，继续向下执行，添加数据库连接
        
        add_connection();
        // 唤醒消费者
        this->cond_cons.notify_one();
    }
}

void ty::connection_pool::add_connection()
{
    ty::mysql_conn* conn = new ty::mysql_conn();
    conn->connect(this->user, this->passwd, this->dbname, this->ip);
    conn->flushed_alive();
    this->connect_que.push(conn);
}

void ty::connection_pool::recycle_connection()
{
    // 同理，这个也要一直进行检测
    while(this->is_work)
    {
        // 先睡一会，不占用CPU资源
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // 加锁
        std::lock_guard<std::mutex> locker(this->lock);
        while(this->connect_que.size() > (this->min_size + this->max_size) / 2)
        {
            ty::mysql_conn* conn = this->connect_que.front();
            if(conn->get_freetime() > this->freetime)
            {
                // 删除
                this->connect_que.pop();
                delete conn;
            } else break;
        }
    }
}

std::shared_ptr<ty::mysql_conn> ty::connection_pool::get_connection()
{
    // 上锁，上条件变量，此函数相当于消费者
    std::unique_lock<std::mutex> locker(this->lock);
    while(this->connect_que.empty())
    {
        this->cond_cons.wait_for(locker, std::chrono::milliseconds(this->timeout));
        if(this->connect_que.empty()) continue;
    }
    std::shared_ptr<ty::mysql_conn> res(this->connect_que.front(), [this](ty::mysql_conn* conn) {
        this->lock.lock();  // 或者使用 std::lock_guard
        this->connect_que.push(conn);
        conn->flushed_alive();
        this->lock.unlock();
    });
    this->connect_que.pop(); // 弹出队列
    // 唤醒生产者
    this->cond_prod.notify_one();
    return res;
}

ty::connection_pool::~connection_pool()
{
    this->is_work = false;
    // 析构队列中指向的堆内存
    while(!this->connect_que.empty())
    {
        delete this->connect_que.front();
        this->connect_que.pop();
    }
}