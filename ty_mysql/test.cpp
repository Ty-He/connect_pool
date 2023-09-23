#include "connection_pool.h"
#include <iostream>

void op1(int begin, int end)
{
    int success = 0;
    for(int i = begin; i < end; ++i)
    {
        if(i > 1000 && i % 1000 == 0) std::cout << i << std::endl;
        // ty::mysql_conn conn;
        ty::mysql_conn* conn = new ty::mysql_conn;
        if(conn->connect("root", "245869", "ty_data", "localhost")) success++;
        //    std::cout << "success" << std::endl;
        char sql[1024] = { 0 };
        sprintf(sql, "insert into info values(%d, 'aaa', 1)", i);
        conn->update(sql); 
        delete conn;
    }
    std::cout << "success = " << success << std::endl;
}

// 使用连接池
void op2(ty::connection_pool* pool ,int begin, int end)
{
    for(int i = begin; i < end; ++i)
    {
        auto conn = pool->get_connection();
        // conn->connect("root", "245869", "ty_data", "localhost");
        char sql[1024] = { 0 };
        sprintf(sql, "insert into info values(%d, 'aaa', 1)", i);
        conn->update(sql); 
    }
}

void test1()
{
#if 1 // 单线程 不用连接池 15657ms
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    op1(0, 5000);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto tm = end - begin;
    std::cout << "not use pool : " << tm.count() << "ns," << tm.count() / 1000000 << "ms" << std::endl; 
#else  // 单线程 连接池 3092ms
    ty::connection_pool* pool = ty::connection_pool::get_connect_pool();
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    op2(pool, 0, 5000);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto tm = end - begin;
    std::cout << "use pool" << tm.count() << "ns," << tm.count() / 1000000 << "ms" << std::endl; 
#endif
}

void test2()
{
#if 1  // 多线程 不用连接池 4040ms
    ty::mysql_conn conn;
    conn.connect("root", "245869", "ty_data", "localhost");
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    //op1(0, 5000);
    std::thread t1(op1, 0, 1000);
    std::thread t2(op1, 0, 1000);
    std::thread t3(op1, 0, 1000);
    std::thread t4(op1, 0, 1000);
    std::thread t5(op1, 0, 1000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto tm = end - begin;
    std::cout << "not use pool : " << tm.count() << "ns," << tm.count() / 1000000 << "ms" << std::endl; 
#else // 连接池 多线程 1232ms
    ty::connection_pool* pool = ty::connection_pool::get_connect_pool();
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::thread t1(op2, pool, 0, 1000);
    std::thread t2(op2, pool, 0, 1000);
    std::thread t3(op2, pool, 0, 1000);
    std::thread t4(op2, pool, 0, 1000);
    std::thread t5(op2, pool, 0, 1000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto tm = end - begin;
    std::cout << "use pool" << tm.count() << "ns," << tm.count() / 1000000 << "ms" << std::endl; 
#endif
}

int main()
{
    test1();
    return 0;
}