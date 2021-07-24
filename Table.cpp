#Code was written by Nha Do in Spring 2021

#include "Table.h"
#include <string>
#include <vector>
#include <functional>
#include <cstdlib>

using namespace std;

int hashValue(string keyField);
bool stringToDouble(string s, double& d);
string convert(string s);

//Below is the StringParser Implementation
class StringParser
{
public:
	StringParser(std::string text = "")
	{
		setString(text);
	}

	void setString(std::string text)
	{
		m_text = text;
		m_start = 0;
	}

	bool getNextField(std::string& field);

private:
	std::string m_text;
	size_t m_start;
};

bool StringParser::getNextField(std::string& fieldText)
{
	m_start = m_text.find_first_not_of(" \t\r\n", m_start);
	if (m_start == std::string::npos)
	{
		m_start = m_text.size();
		fieldText = "";
		return false;
	}
	if (m_text[m_start] != '\'')
	{
		size_t end = m_text.find_first_of(" \t\r\n", m_start + 1);
		fieldText = m_text.substr(m_start, end - m_start);
		m_start = end;
		return true;
	}
	fieldText = "";
	for (;;)
	{
		m_start++;
		size_t end = m_text.find('\'', m_start);
		fieldText += m_text.substr(m_start, end - m_start);
		m_start = (end != std::string::npos ? end + 1 : m_text.size());
		if (m_start == m_text.size() || m_text[m_start] != '\'')
			break;
		fieldText += '\'';
	}
	return true;
}

Table::Table(string keyColumn, const vector<string>& columns)
{
	this->keyColumn = keyColumn;
	this->size = columns.size();
	label[0] = new HashTable;
	for (int i = 0; i < MAX_SIZE; i++)
	{
		table[i] = new HashTable;
		for (int j = 0; j < size; j++)
		{
			label[0]->value[j] = columns[j];
			table[i]->value[j] = "";
		}
		table[i]->nextNode = nullptr;
	}
	label[0]->nextNode = nullptr;
}

bool Table::good() const
{
	if (keyColumn == "" || size == 0)
		return false;
	int countInvalid = 0;
	for (int j = 0; j < size; j++)
	{
		if (keyColumn != label[0]->value[j])
			countInvalid++;
	}
	if (countInvalid == size)
		return false;
	return true;
}

Table::~Table()
{
	HashTable* temp;
	HashTable* temp_next;
	for (int i = 0; i < MAX_SIZE; i++) 
	{
		temp = table[i];
		while (temp != NULL) {
			temp_next = temp->nextNode;
			delete temp;
			temp = temp_next;
		} 
		table[i] = NULL;
	}
	delete label[0];
}

bool Table::insert(const std::string& recordString)
{
	StringParser parser(recordString);
	string getKeyColumn;
	string s;
	string track;
	int k;
	int index;
	
	//Check if input string is valid
	int count = 0;
	while (parser.getNextField(track))
		count++;
	if (count == size)
	{
		StringParser par1(recordString);
		//Find the key column in recordString
		for (int i = 0; i < size; i++)
		{
			//Find the corresponding index of key field
			for (k = 0; k < size; k++)
			{
				if (keyColumn == label[0]->value[k])
					break;
			}

			for (int m = 0; m < k+1; m++)
			{
				if (par1.getNextField(getKeyColumn)) {}
			}				
			break;
		}

		index = hashValue(getKeyColumn);

		StringParser par2(recordString);

		//If there is no exsisted node before
		if (table[index]->value[k] == "")
		{
			for (int j = 0; j < size; j++)
			{
				if (par2.getNextField(s))
					table[index]->value[j] = s;
			}
		}
		else
		{
			//Create a new node and pass the value to it
			HashTable* ptr = table[index]; //Head node to traverse the linked-list
			HashTable* node = new HashTable;
			for (int j = 0; j < size; j++)
			{
				if (par2.getNextField(s))
					node->value[j] = s;
			}
			node->nextNode = nullptr;

			//Find the last node to insert
			while(ptr->nextNode != nullptr)
				ptr = ptr->nextNode;
			
			ptr->nextNode = node; //Change nextNode's pointer
		}
		return true;
	}
	else
		return false;	
}

void Table::find(string key, vector<vector<string>>& records) const
{
	int count = 0;
	int index = hashValue(key);
	HashTable* ptr_count = table[index];
	HashTable* ptr = table[index];

	//Count number of element in table[index]
	while (ptr_count->nextNode != nullptr)
	{
		ptr_count = ptr_count->nextNode;
		count++;
	}

	//Resize and put the values into vector
	records.resize(count + 1);
	for (int i = 0; i <= count; i++)
	{
		for (int j = 0; j < size; j++)
			records[i].push_back(ptr->value[j]);
		ptr = ptr->nextNode;
	}
}

int Table::select(string query,vector<vector<string>>& records) const
{
	StringParser first(query);
	StringParser second(query);
	string track;
	string getTerm;
	string str[3]; //Hold values extracted from query
	double d0,d2; //Hold the numerical value of string
	double d; //Hold the numerical value of element in table
	int k = 0; //Hold the index of column name

	//Check if query is valid
	int count = 0;
	while (first.getNextField(track))
		count++;
	if (count == 3)
	{
		//Split query and put it in str array
		for (int i = 0; i < count; i++)
		{
			if (second.getNextField(getTerm))
				str[i] = getTerm;
		}

		//Check validation of the first term
		int countInvalid = 0;
		for (k = 0; k < size; k++)
		{
			if (str[0] != label[0]->value[k])
				countInvalid++;
			else if (str[0] == label[0]->value[k])
				break;
		}
		if (countInvalid == size)
		{
			records.resize(0);
			return -1;
		}

		//Check validation of the second term
		if (str[1] != "<" && str[1] != "<=" && str[1] != ">" && str[1] != ">=" && str[1] != "!=" && str[1] != "==" && str[1] != "=" && convert(str[1]) != "LT" && convert(str[1]) != "LE" && convert(str[1]) != "GT" && convert(str[1]) != "GE" && convert(str[1]) != "NE" && convert(str[1]) != "EQ")
		{
			records.resize(0);
			return -1;
		}

		//If validation is verified, then we process the function
		int countSize = 0;
		int countError = 0;

		//For "<" case
		if (str[1] == "<" && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if (ptr_count->value[k] < str[2])
						countSize++;
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if (ptr_count->value[k] < str[2])
						{
							countSize++;
						}
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if (ptr_count->value[k] < str[2])
							countSize++;
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if (ptr_push->value[k] < str[2])
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if (ptr_push->value[k] < str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if (ptr_push->value[k] < str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
					}
				}
			}
			return 0;
		}

		//For "<=" case
		if (str[1] == "<=" && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if (ptr_count->value[k] <= str[2])
						countSize++;
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if (ptr_count->value[k] <= str[2])
						{
							countSize++;
						}
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if (ptr_count->value[k] <= str[2])
							countSize++;
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if (ptr_push->value[k] <= str[2])
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if (ptr_push->value[k] <= str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if (ptr_push->value[k] <= str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
					}
				}
			}
			return 0;
		}

		//For ">" case
		if (str[1] == ">" && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if (ptr_count->value[k] > str[2])
						countSize++;
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if (ptr_count->value[k] > str[2])
						{
							countSize++;
						}
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if (ptr_count->value[k] > str[2])
							countSize++;
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if (ptr_push->value[k] > str[2])
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if (ptr_push->value[k] > str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if (ptr_push->value[k] > str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
					}
				}
			}
			return 0;
		}

		//For ">=" case
		if (str[1] == ">=" && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if (ptr_count->value[k] >= str[2])
						countSize++;
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if (ptr_count->value[k] >= str[2])
						{
							countSize++;
						}
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if (ptr_count->value[k] >= str[2])
							countSize++;
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if (ptr_push->value[k] >= str[2])
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if (ptr_push->value[k] >= str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if (ptr_push->value[k] >= str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
					}
				}
			}
			return 0;
		}

		//For "!=" case
		if (str[1] == "!=" && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if (ptr_count->value[k] != str[2])
						countSize++;
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if (ptr_count->value[k] != str[2])
						{
							countSize++;
						}
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if (ptr_count->value[k] != str[2])
							countSize++;
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if (ptr_push->value[k] != str[2])
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if (ptr_push->value[k] != str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if (ptr_push->value[k] != str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
					}
				}
			}
			return 0;
		}

		//For "==" and "=" cases
		if ((str[1] == "==" || str[1] == "=") && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if (ptr_count->value[k] == str[2])
						countSize++;
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if (ptr_count->value[k] == str[2])
						{
							countSize++;
						}
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if (ptr_count->value[k] == str[2])
							countSize++;
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if (ptr_push->value[k] == str[2])
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if (ptr_push->value[k] == str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if (ptr_push->value[k] == str[2])
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize)
								return 0;
							index_records++;
						}
					}
				}
			}
			return 0;
		}

		//For "LT" case
		if ((convert(str[1]) == "LT" || convert(str[1]) == "<") && (!stringToDouble(str[0], d0)) && (!stringToDouble(str[2], d2))) //If str[2] is badly formed
		{
			records.resize(0);
			return -1;
		}
		if ((convert(str[1]) == "LT" || convert(str[1]) == "<") && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if ((stringToDouble(ptr_count->value[k], d) && (stringToDouble(str[2], d2)) && d < d2))
						countSize++;
					if (!(stringToDouble(ptr_count->value[k], d)))
						countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_count->value[k], d) && d < d2))
						{
							countSize++;
						}
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if ((stringToDouble(ptr_count->value[k], d) && d < d2))
							countSize++;
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if ((stringToDouble(ptr_push->value[k], d) && d < d2))
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d < d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d < d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
					}
				}
			}
			if (countError != 0)
				return countError;
			else
				return 0;
		}

		//For "LE" case
		if ((convert(str[1]) == "LE" || convert(str[1]) == "<=") && (!stringToDouble(str[0], d0)) && (!stringToDouble(str[2], d2))) //If str[2] is badly formed
		{
			records.resize(0);
			return -1;
		}
		if ((convert(str[1]) == "LE" || convert(str[1]) == "<=") && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if ((stringToDouble(ptr_count->value[k], d) && (stringToDouble(str[2], d2)) && d <= d2))
						countSize++;
					if (!(stringToDouble(ptr_count->value[k], d)))
						countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_count->value[k], d) && d <= d2))
						{
							countSize++;
						}
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if ((stringToDouble(ptr_count->value[k], d) && d <= d2))
							countSize++;
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if ((stringToDouble(ptr_push->value[k], d) && d <= d2))
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d <= d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d <= d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
					}
				}
			}
			if (countError != 0)
				return countError;
			else
				return 0;
		}

		//For "GT" case
		if ((convert(str[1]) == "GT" || convert(str[1]) == ">") && (!stringToDouble(str[0], d0)) && (!stringToDouble(str[2], d2))) //If str[2] is badly formed
		{
			records.resize(0);
			return -1;
		}
		if ((convert(str[1]) == "GT" || convert(str[1]) == ">") && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if ((stringToDouble(ptr_count->value[k], d) && (stringToDouble(str[2], d2)) && d > d2))
						countSize++;
					if (!(stringToDouble(ptr_count->value[k], d)))
						countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_count->value[k], d) && d > d2))
						{
							countSize++;
						}
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if ((stringToDouble(ptr_count->value[k], d) && d > d2))
							countSize++;
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if ((stringToDouble(ptr_push->value[k], d) && d > d2))
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d > d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d > d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
					}
				}
			}
			if (countError != 0)
				return countError;
			else
				return 0;
		}

		//For "GE" case
		if ((convert(str[1]) == "GE" || convert(str[1]) == ">=") && (!stringToDouble(str[0], d0)) && (!stringToDouble(str[2], d2))) //If str[2] is badly formed
		{
			records.resize(0);
			return -1;
		}
		if ((convert(str[1]) == "GE" || convert(str[1]) == ">=") && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if ((stringToDouble(ptr_count->value[k], d) && (stringToDouble(str[2], d2)) && d >= d2))
						countSize++;
					if (!(stringToDouble(ptr_count->value[k], d)))
						countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_count->value[k], d) && d >= d2))
						{
							countSize++;
						}
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if ((stringToDouble(ptr_count->value[k], d) && d >= d2))
							countSize++;
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if ((stringToDouble(ptr_push->value[k], d) && d >= d2))
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d >= d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d >= d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
					}
				}
			}
			if (countError != 0)
				return countError;
			else
				return 0;
		}

		//For "NE" case
		if ((convert(str[1]) == "NE" || convert(str[1]) == "!=") && (!stringToDouble(str[0], d0)) && (!stringToDouble(str[2], d2))) //If str[2] is badly formed
		{
			records.resize(0);
			return -1;
		}
		if ((convert(str[1]) == "NE" || convert(str[1]) == "!=") && (!stringToDouble(str[0], d0)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if ((stringToDouble(ptr_count->value[k], d) && (stringToDouble(str[2], d2)) && d != d2))
						countSize++;
					if (!(stringToDouble(ptr_count->value[k], d)))
						countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_count->value[k], d) && d != d2))
						{
							countSize++;
						}
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if ((stringToDouble(ptr_count->value[k], d) && d != d2))
							countSize++;
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if ((stringToDouble(ptr_push->value[k], d) && d != d2))
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d != d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d != d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
					}
				}
			}
			if (countError != 0)
				return countError;
			else
				return 0;
		}

		//For "EQ" case
		if ((convert(str[1]) == "EQ" || convert(str[1]) == "==") && (!stringToDouble(str[0], d0)) && (!stringToDouble(str[2], d2))) //If str[2] is badly formed
		{
			records.resize(0);
			return -1;
		}
		if ((convert(str[1]) == "EQ" || convert(str[1]) == "==") && (!stringToDouble(str[0], d0))) //&& (stringToDouble(str[2], d2)))
		{
			//Reset records
			if (records.size() != 0)
			{
				records.resize(0);
			}
			//Count number of element satisfies the query
			for (int i = 0; i < MAX_SIZE; i++)
			{
				HashTable* ptr_count = table[i];
				if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
				{
					if ((stringToDouble(ptr_count->value[k], d) && (stringToDouble(str[2], d2)) && d == d2))
						countSize++;
					if (!(stringToDouble(ptr_count->value[k], d)))
						countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
				}
				else
				{
					while (ptr_count->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_count->value[k], d) && d == d2))
						{
							countSize++;
						}
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
						ptr_count = ptr_count->nextNode;
					}
					if (ptr_count->nextNode == nullptr && ptr_count->value[k] != "")
					{
						if ((stringToDouble(ptr_count->value[k], d) && d == d2))
							countSize++;
						if (!(stringToDouble(ptr_count->value[k], d)))
							countError++; //Increase countError because it does not satisfy the query, if countError != 0, it would be used to return
					}
				}
			}
			//Resize vector
			records.resize(countSize);
			int index_records = 0;
			for (int j = 0; j < MAX_SIZE; j++)
			{
				HashTable* ptr_push = table[j];
				if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
				{
					if ((stringToDouble(ptr_push->value[k], d) && d == d2))
					{
						for (int a = 0; a < size; a++)
						{
							records[index_records].push_back(ptr_push->value[a]);
						}
						index_records++;
					}
				}
				else
				{
					while (ptr_push->nextNode != nullptr)
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d == d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
						ptr_push = ptr_push->nextNode;
					}
					if (ptr_push->nextNode == nullptr && ptr_push->value[k] != "")
					{
						if ((stringToDouble(ptr_push->value[k], d) && (stringToDouble(str[2], d2)) && d == d2))
						{
							if (index_records <= countSize - 1)
							{
								for (int a = 0; a < size; a++)
								{
									records[index_records].push_back(ptr_push->value[a]);
								}
							}
							else if (index_records == countSize && countError == 0)
								return 0;
							else if (index_records == countSize && countError != 0)
								return countError;
							index_records++;
						}
					}
				}
			}
			if (countError != 0)
				return countError;
			else
				return 0;
		}

		//If all of cases above are failed, then input should be badly formed
		records.resize(0);
		return -1;
	}
	else
	{
		records.resize(0);
		return -1;
	}	
}

//Calculate hash value
int hashValue(string keyField)
{
	unsigned int h = hash<string>()(keyField); //Produces hash value 
	int index = h % MAX_SIZE; //Converts to our own number
	
	return index;
}

//Convert string to double value
bool stringToDouble(string s, double& d)
{
	char* end;
	d = std::strtof(s.c_str(), &end);
	return end == s.c_str() + s.size() && !s.empty();
}

//Convert string to upper case
string convert(string s)
{
	for (int i = 0; i < s.length(); i++)
	{
		s[i] = toupper(s[i]);
	}
	return s;
}

