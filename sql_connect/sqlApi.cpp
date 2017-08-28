#include "sqlApi.h"

sqlApi::sqlApi()
{
	conn_fd = mysql_init(NULL);
}

int sqlApi::connect()
{
	int ret = 0;
	if(mysql_real_connect(conn_fd, "127.0.0.1", "root", "",\
				"Linux3_class", 3306, NULL, 0)){
		std::cout << "connect success" << std::endl;
	}else{
		std::cerr << "connect failed" << std::endl;
		ret = -1;
	}

	return ret;
}

int sqlApi::insert(const std::string &name,const std::string &sex,\
		const std::string &school, const std::string &hobby)
{
	std::string sql = "INSERT INTO students (name,sex,school,hobby) VALUES ('";
	sql += name;
	sql += "','";
	sql += sex;
	sql += "','";
	sql += school;
	sql += "','";
	sql += hobby;
	sql += "')";
	int ret = mysql_query(conn_fd, sql.c_str());
	if( 0 == ret ){
		std::cout << "insert success!" << std::endl;
	}else{
		std::cout << sql << " " << ret <<std::endl;
	}
}

int sqlApi::select()
{
	std::string sql = "SELECT * FROM students";
	if(mysql_query(conn_fd, sql.c_str()) == 0){
		MYSQL_RES *res = mysql_store_result(conn_fd);
		if(res){
			int i, j;
			int rows = mysql_num_rows(res);
			int cols = mysql_num_fields(res);
			std::cout << "rows: " << rows << " cols: " << cols << std::endl;
			MYSQL_FIELD *fl;
			for(; fl = mysql_fetch_field(res); ){
				std::cout << fl->name << '\t';
			}
			std::cout <<"<br/>"<<std::endl;
			for(i = 0; i < rows; i++ ){
				MYSQL_ROW row = mysql_fetch_row(res);
				//char *[]
				for(j = 0; j < cols; j++){
					std::cout << row[j] << '\t';
				}
				std::cout <<"<br/>"<<std::endl;
			}
		}

		free(res);
		return 0;
	}
	return -1;
}

sqlApi::~sqlApi()
{
	mysql_close(conn_fd);
}










