#include "mysql_conn.h"
// #include <iostream>

ty::mysql_conn::mysql_conn()
{
    mysql = mysql_init(nullptr);
    mysql_set_character_set(this->mysql, "utf8");
    if(mysql == nullptr) mysql_error(this->mysql);
}

ty::mysql_conn::~mysql_conn()
{
    // 释放结果集
    this->free_res(); 
    // 释放mysql实例 // 这里写 if(!mysql) 纯他妈的做力控判断空指针习惯了卧槽 
    if(mysql) mysql_close(mysql);
}

bool ty::mysql_conn::connect(std::string user, std::string passwd, std::string db_name, std::string ip,unsigned int port)
{
    if(!this->mysql) return false;
    if(this->mysql != mysql_real_connect(this->mysql, ip.c_str(), user.c_str(), passwd.c_str(), db_name.c_str(), port, nullptr, 0))
    {
        // 单线程不使用连接池，提示 Too many connections
        // std::cout << mysql_error(this->mysql) << std::endl;
        return false;
    } 
    return true;
}

bool ty::mysql_conn::update(std::string sql)
{
    // mysql_query 在成功时返回0
    return !mysql_query(this->mysql, sql.c_str());
    /*
    if(mysql_query(this->mysql, sql.c_str()) != 0)
        std::cout <<mysql_error(this->mysql) << std::endl;
    // Lost connection to MySQL server during query
    return true;
    */
}

bool ty::mysql_conn::query(std::string sql)
{
    // 每次调用query时要清空上次查询得到的结果集
    free_res();     // 第一次调用时由于在free_res中有判空行为
    if(mysql_query(this->mysql, sql.c_str())) return false;
    return (this->result = mysql_store_result(this->mysql)) != nullptr;
}

bool ty::mysql_conn::next()
{
    if(this->result) 
    {
        this->record = mysql_fetch_row(this->result);
        return this->record != nullptr;
    }
    return false;
}

std::string ty::mysql_conn::value(int index)
{
    // 先获得记录中的字段个数
    unsigned int field_count = mysql_num_fields(this->result);
    if(index < 0 || index >= field_count) return std::string();
    char* val = this->record[index];
    // val内部可能有\0，要指定初始化长度
    return std::string(val, mysql_fetch_lengths(this->result)[index]);
}

bool ty::mysql_conn::get_affairs()
{
    // 这里默认禁止自动提交
    return mysql_autocommit(this->mysql, false);
    // 这里发现上面函数的返回值为bool类型
}

bool ty::mysql_conn::commit()
{
    return mysql_commit(this->mysql);
}

bool ty::mysql_conn::rollback()
{
    return mysql_rollback(this->mysql);
}

void ty::mysql_conn::free_res()
{
    if(this->result) 
    {
        mysql_free_result(this->result);
        this->result = nullptr; // 再次置空
    }
}

void ty::mysql_conn::flushed_alive()
{
    this->begin_point = std::chrono::steady_clock::now();
}

long long ty::mysql_conn::get_freetime()
{
    std::chrono::milliseconds res = std::chrono::duration_cast<std::chrono::milliseconds>
                                (std::chrono::steady_clock::now() - this->begin_point);
    return res.count();
}