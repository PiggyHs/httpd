#ifndef _SQLAPI_H_
#define _SQLAPI_H_

#include <iostream>
#include <mysql.h>

class sqlApi{
	public:
		sqlApi();
		int connect();
		int insert(const std::string &name,\
				   const std::string &sex,\
				   const std::string &school,\
				   const std::string &hobby);
		int select();
		//int modify();
		//int erase();
		~sqlApi();
	private:
		MYSQL *conn_fd;
};

#endif
