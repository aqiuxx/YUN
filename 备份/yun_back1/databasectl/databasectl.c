#include "databasectl.h"
#include <stdio.h>
int verify_user(const char *username, const char *passwd)
{
	sqlite3 *db;
	if(sqlite3_open("my.db",&db) != SQLITE_OK)
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	char buf[100]={0};
	sprintf(buf,"select * from userpasswd where name=\"%s\" AND passwd=\"%s\"",username,passwd);
	printf("%s\n",buf);
	char *errmsg, **resultp;
	int nrow, ncolumn,i,j,index;
	if(sqlite3_get_table(db, buf, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n",errmsg);
		return -2;
	}
	
	index = ncolumn;
	for(i = 0; i < nrow; i++)
	{
		for(j = 0; j < ncolumn; j++)
			printf("%s:%s\n",resultp[j],resultp[index++]);
	}
	printf("row:%d,column:%d\n",nrow, ncolumn);

	if(nrow > 0)
		return 1;
	else
		return 0;


}
int user_isexist(const char *username)
{
	sqlite3 *db;
	if(sqlite3_open("my.db",&db) != SQLITE_OK)
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	char buf[100]={0};
	sprintf(buf,"select * from userpasswd where name=\"%s\"",username);
	printf("%s\n",buf);
	char *errmsg, **resultp;
	int nrow, ncolumn,i,j,index;
	if(sqlite3_get_table(db, buf, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n",errmsg);
		return -2;
	}
	
	index = ncolumn;
	for(i = 0; i < nrow; i++)
	{
		for(j = 0; j < ncolumn; j++)
			printf("%s:%s\n",resultp[j],resultp[index++]);
	}
	printf("row:%d,column:%d\n",nrow, ncolumn);

	if(nrow > 0)
		return 1;
	else
		return 0;

}
int add_user(const char *username, const char *passwd)
{
	sqlite3 *db;
	if(sqlite3_open("my.db",&db) != SQLITE_OK)
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	char buf[100]={0};
	sprintf(buf,"insert into userpasswd values(\"%s\",\"%s\")",username,passwd);
	printf("%s\n",buf);
	char *errmsg, **resultp;
	int nrow, ncolumn,i,j,index;
	if(sqlite3_get_table(db, buf, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n",errmsg);
		return -2;
	}
	return 0;

}
int create_filetable(const char *username)
{
	sqlite3 *db;
	if(sqlite3_open("my.db",&db) != SQLITE_OK)
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	char buf[100]={0};
	sprintf(buf,"create table %s (filename)",username);
	printf("%s\n",buf);
	char *errmsg, **resultp;
	int nrow, ncolumn,i,j,index;
	if(sqlite3_get_table(db, buf, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n",errmsg);
		return -2;
	}
	return 0;
	
}
static int create_passwdtable(const char *tablename)
{
	sqlite3 *db;
	if(sqlite3_open("my.db",&db) != SQLITE_OK)
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	char buf[100]={0};
	sprintf(buf,"create table %s(name,passwd)",tablename);
	printf("%s\n",buf);
	char *errmsg, **resultp;
	int nrow, ncolumn,i,j,index;
	if(sqlite3_get_table(db, buf, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n",errmsg);
		return -2;
	}

}
int file_isexist(const char *username, const char *filename)
{
	sqlite3 *db;
	if(sqlite3_open("my.db",&db) != SQLITE_OK)
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	char buf[100]={0};
	sprintf(buf,"select * from %s where filename=\"%s\"",username,filename);
	printf("%s\n",buf);
	char *errmsg, **resultp;
	int nrow, ncolumn,i,j,index;
	if(sqlite3_get_table(db, buf, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n",errmsg);
		return -2;
	}
	
	index = ncolumn;
	for(i = 0; i < nrow; i++)
	{
		for(j = 0; j < ncolumn; j++)
			printf("%s:%s\n",resultp[j],resultp[index++]);
	}
	printf("row:%d,column:%d\n",nrow, ncolumn);

	if(nrow > 0)
		return 1;
	else
		return 0;



}
int add_filename(const char *username, const char *filename)
{
	sqlite3 *db;
	if(sqlite3_open("my.db",&db) != SQLITE_OK)
	{
		printf("error:%s\n",sqlite3_errmsg(db));
		return -1;
	}
	char buf[100]={0};
	sprintf(buf,"insert into %s values(\"%s\")",username,filename);
	printf("%s\n",buf);
	char *errmsg, **resultp;
	int nrow, ncolumn,i,j,index;
	if(sqlite3_get_table(db, buf, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("error:%s\n",errmsg);
		return -2;
	}
	return 0;


}
