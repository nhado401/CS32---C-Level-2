#ifndef TABLE_INCLUDED
#define TABLE_INCLUDED

#include <string>
#include <vector>

const int MAX_SIZE = 1000;

class Table
{
public:
	Table(std::string keyColumn, const std::vector<std::string>& columns);
	~Table();
	bool good() const;
	bool insert(const std::string& recordString);
	void find(std::string key, std::vector<std::vector<std::string>>& records) const;
	int select(std::string query, std::vector<std::vector<std::string>>& records) const;

	// We prevent a Table object from being copied or assigned by
	// making the copy constructor and assignment operator unavailable.
	Table(const Table&) = delete;
	Table& operator=(const Table&) = delete;
private:
	std::string keyColumn;
	int size = 0;
	struct HashTable{
		std::string value[MAX_SIZE];
		HashTable* nextNode;
	};
	HashTable* table[MAX_SIZE];
	HashTable* label[1]; //This array object is used to keep track on the label
};

#endif 
