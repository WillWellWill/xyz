#ifndef __NSPR_WORD_DB_H_INCLUDED__
#define __NSPR_WORD_DB_H_INCLUDED__


namespace nspr
{

class WordDb
{
public:
	WordDb();
	~WordDb();

	u_char *WordRandom();
	void Update(u_char *word);

private:
	u_char *m_dbPath;
    sqlite3 *m_db;
	u_char m_word[4096];

}; // class WordDb

} // namespace nspr

#endif // ifndef __NSPR_WORD_DB_H_INCLUDED__

