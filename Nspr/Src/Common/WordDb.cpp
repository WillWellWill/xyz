#include "Nspr.h"

#include "WordDb.h"


namespace nspr
{

WordDb::WordDb()
	: m_dbPath((u_char *)"/opt/Code/Sqlite3/engword.db")
{
	memcpy(m_word, "will", sizeof("will"));
}

WordDb::~WordDb()
{
}

static int callback(void *user, int argc, char **argv, char **azColName) {
    int i;
    for(i=0; i<argc; i++){
      NsprInfo("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
	  if (argv[i]) {
		  ((WordDb *)user)->Update((u_char *)argv[i]);
	  }
    }
    return 0;
}
	
void WordDb::Update(u_char *word)
{
	memcpy(m_word, word, ngx_strlen(word) + 1);
}

u_char *WordDb::WordRandom()
{
	srand(time(nullptr));
    char *zErrMsg = 0;
	if (sqlite3_open((char *)m_dbPath, &m_db)) {
		sqlite3_close(m_db);
		return (u_char *)"";
	}
	int i = 0;
	int number, count = 0;
	for (i = 0; i < 100; i++) {
		number = (int)(12000.0 * rand()/(RAND_MAX + 1.0));
		NsprDebug("rand number = %d", number);
		count += number;
	}
	for (i = 0; i < number % 100; i++) {
		rand();
	}
	for (i = 0; i < count % 100; i++) {
		number = (int)(12000.0 * rand()/(RAND_MAX + 1.0));
	}
	NsprDebug("last rand number = %d", number);
	char sql[1024] = {0};
	snprintf(sql, sizeof(sql), "select Word from words where ID like %d;", number);
	if( sqlite3_exec(m_db, sql, callback, this, &zErrMsg) != SQLITE_OK) {
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(m_db);
	return (u_char *)m_word;
}

} // namespace nspr

