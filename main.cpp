#include "Table.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdlib>

using namespace std;

bool stringToDouble(string s, double& d);

int main()
{
	vector<string> cols = {
			"customer", "product", "price", "location"
	};
	Table t("customer", cols);
	assert(t.good());
	assert(t.insert("Patel 12345 42.54 Westwood"));
	assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
	assert(t.insert("   Hoang  12345 30.46 'Santa Monica' "));
	assert(t.insert("Patel\t67890\t142.75  \t \t\t  \tHollywood"));
	//assert(t.insert("Patel\t12453\t143.99  \t \t\t  \tElsinore"));
	//assert(t.insert("   Hoang  543 29 'San Diego' "));
	assert(!t.insert("Figueroa 54321 59.95"));
	cout << "First Passed" << endl;
	vector<vector<string>> v;
	t.find("Patel", v);
	assert(v.size() == 2);
	vector<vector<string>> expected = {
		{ "Patel", "12345", "42.54", "Westwood" },
		{ "Patel", "67890", "142.75", "Hollywood" },
	};
	assert((v[0] == expected[0] && v[1] == expected[1]) ||
		(v[0] == expected[1] && v[1] == expected[0]));
	//vector<vector<string>> v;
	assert(t.select("location < Westwood", v) == 0);
	assert(v.size() == 2);
	vector<vector<string>> expected1 = {
		{ "Hoang", "12345", "30.46", "Santa Monica" },
		{ "Patel", "67890", "142.75", "Hollywood" }
	};
	assert((v[0] == expected1[0] && v[1] == expected1[1]) ||
		(v[0] == expected1[1] && v[1] == expected1[0]));
	cout << "Second Passed" << endl;

	vector<vector<string>> v2;
	assert(t.select("price lt 40", v2) == 0);
	assert(v2.size() == 2);
	vector<vector<string>> expected2 = {
		{ "O'Reilly", "34567", "4.99", "Westwood" },
		{ "Hoang", "12345", "30.46", "Santa Monica" }
	};
	assert((v2[0] == expected2[0] && v2[1] == expected2[1]) ||
		(v2[0] == expected2[1] && v2[1] == expected2[0]));
	cout << "Third Passed" << endl;

	//vector<vector<string>> v2;
	assert(t.select("price lt 40k", v2) == -1);
	assert(v2.size() == 0);
	cout << "Fourth Passed" << endl;

	vector<string> cols1 = { "N", "Z" };
	Table t1("Z", cols1);
	assert(t1.good());
	assert(t1.insert("UCLA 90095"));
	assert(t1.insert("Caltech 91125"));
	vector<vector<string>> v_1;
	t1.find("90095", v_1);
	assert(v_1.size() == 1);
	assert(v_1[0][0] == "UCLA" && v_1[0][1] == "90095");
	assert(t1.select("Z > 90210", v_1) == 0);
	assert(v_1.size() == 1);
	assert(v_1[0][0] == "Caltech" && v_1[0][1] == "91125");

	vector<string> uclacols = { "last name", "first name", "major", "UID", "GPA" };
	Table ucla("UID", uclacols);
	assert(ucla.good());
	assert(ucla.insert("Bruin Jose 'COG SCI' 304567890 3.4"));
	assert(ucla.insert("Bruin Josephine 'COM SCI' 605432109 3.8"));
	assert(ucla.insert("Trojan Tommy LOSING 000000000 1.7"));
	// Troy brought a wooden horse full of Greek soldiers inside the
	// city walls and lost the Trojan War.  How strange to look up to
	// gullible losers as role models.
	assert(ucla.select("GPA GE 3.2", v) == 0);
	assert(v.size() == 2);
	assert(v[0][0] == v[1][0] && v[0][1] != v[1][1]);

	vector<string> firstTest = { "Name", "GPA", "Year", "Address", "School" };
	Table first("Year", firstTest);
	assert(first.good());
	assert(first.insert("'Nha Do' 3.8 2021 'Palm Springs' UCLA"));
	assert(first.insert("'Nha Do' 3.51 2020 Temecula 'Cal Poly'"));
	assert(first.insert("'Nha Do' 2.87 2021 Pasadena ' '"));
	assert(first.insert("'Bruin' 3.41 2022 'San Diego' UCSD"));
	assert(first.insert("'Trojan' 3.2 2019 'Long Beach' CSULB"));
	assert(first.insert("'Triton' 3.0 2023 ' ' ' '"));

	vector<vector<string>> vFirstTest;
	first.find("2021", vFirstTest);
	assert(vFirstTest.size() == 2);
	vector<vector<string>> expectedFirstTest = {
		{"Nha Do", "3.8", "2021", "Palm Springs", "UCLA"},
		{"Nha Do", "2.87", "2021", "Pasadena", " "}
	};
	assert((vFirstTest[0] == expectedFirstTest[0] && vFirstTest[1] == expectedFirstTest[1]) ||
		((v[0] == expected[1] && v[1] == expected[0])));
	cout << "Fifth Passed" << endl;

	assert(first.select("GPA <= 3.0", v) == 0);
	assert(v.size() == 2);

	cout << "Sixth Passed" << endl;

	vector<string> secondTest = { "item name", "price" };
	Table second("item name", secondTest);
	assert(second.good());
	assert(second.insert("'chocolate bar' 1.69"));
	assert(second.insert("coffee 7.99"));
	assert(second.insert("hummus 3.49"));
	assert(second.insert("'spring roll' 4.99"));
	vector<vector<string>> vSecondTest;
	assert(second.select("price LT 5", vSecondTest) == 0);
	assert(vSecondTest.size() == 3);  // chocolate bar, hummus and spring roll
	assert(second.insert("pretzels 1.W9"));
	assert(second.insert("noodle 10K"));
	assert(second.insert("boba covid19"));
	assert(second.select("price LT H", vSecondTest) == -1);
	assert(vSecondTest.size() == 0);
	assert(second.select("price LT 5", vSecondTest) == 3);  // 3 because pretzels 1.W9, noodle 10k and boba covid19
	assert(vSecondTest.size() == 3);  // chocolate bar and hummus

	cout << "Seventh Passed" << endl;

	cout << "All Passed" << endl;

	cout << "DONE" << endl;
}
