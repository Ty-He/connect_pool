cmake_minimum_required(VERSION 3.17)

project(mysqlpool)

set(CMAKE_CXX_STANDARD 14)

set(JSONCPP_PATH D:/code/Json/Jsoncpp_source/jsoncpp-master/jsoncpp-master)
set(MYSQL_PATH D:/ruanjian/vscode/mysql-8.0.33-winx64)

# include_directories(D:/ruanjian/vscode/mysql-8.0.33-winx64/include)
# include_directories("D:/code/Json/Jsoncpp_source/jsoncpp-master/jsoncpp-master/include")
include_directories(${JSONCPP_PATH}/include)
include_directories(${MYSQL_PATH}/include)

file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB head ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# 这里生成可执行程序时要指定.h头文件，要不智能提示报错
# 很奇怪，一对配对的.h和.cpp在下面这条命令需要先指定.cpp文件再指定.h文件
#add_executable(mysqlpool mysql_conn.cpp mysql_conn.h connection_pool.cpp connection_pool.h test.cpp)
add_executable(mysqlpool ${src} ${head})

# mysql
# target_link_directories(mysqlpool PUBLIC D:/ruanjian/vscode/mysql-8.0.33-winx64/include)
# target_link_libraries(mysqlpool PUBLIC D:/ruanjian/vscode/mysql-8.0.33-winx64/lib/libmysql.lib)
# target_link_libraries(mysqlpool PUBLIC D:/ruanjian/vscode/mysql-8.0.33-winx64/lib/libmysql.dll)

target_link_directories(mysqlpool PUBLIC ${MYSQL_PATH}/include)
target_link_libraries(mysqlpool PUBLIC ${MYSQL_PATH}/lib/libmysql.lib)
target_link_libraries(mysqlpool PUBLIC ${MYSQL_PATH}/lib/libmysql.dll)

# jsoncpp

#target_include_directories(mysqlpool PUBLIC D:/code/Json/Jsoncpp_source/jsoncpp-master/jsoncpp-master/include)
#target_link_libraries(mysqlpool PUBLIC D:/code_cpp/json/lib/libjsoncpp.a)

target_include_directories(mysqlpool PUBLIC ${JSONCPP_PATH}/include)
target_link_libraries(mysqlpool PUBLIC D:/code_cpp/json/lib/libjsoncpp.a)