//Ethan Clark <ejc49@zips.uakron.edu>
//JSON Parser

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <memory>
#include <map>


//Concatenates two different int values to form one larger int
int concat(int p, int q) {
	int hold = 1;
	while (hold <= q) hold *= 10;
	return (p * hold + q);
}

//Returns digits past the decimal point for number concatenation
double toDouble(int p, int q) {
	double hold = p;
	double hold2 = q;
	hold += (hold2 / 10);
	return hold;
}

//Converts JSON text to int value
int toInt(std::string::iterator f) {
	char c = *f;
	int hold = c - '0';
	return hold;
}


//Classes
struct value {
	virtual ~value() { }
	virtual void parse(std::string::iterator &f, std::vector<std::unique_ptr<value>> &v, int &weight) { }
};

struct Null : value {
	Null() = default;

	virtual void parse(std::string::iterator &f, std::vector<std::unique_ptr<value>> &v, int &weight) {
		int hold = v.size() + 1;
		v.resize(hold);
		v[hold].reset(new Null());
		f + 4;
		++weight;
	}
};

struct Boolean : value {
	bool cond;

	Boolean() = default;
	Boolean(const bool & b) : cond(b) { }

	void parse(std::string::iterator &f, std::vector<std::unique_ptr<value>> &v, int &weight) {
		if (*f == 't') {
			int hold = v.size() + 1;
			v.resize(hold);
			v[hold].reset(new Boolean(true));
			f + 4;
			++weight;
		}

		else if (*f == 'f') {
			int hold = v.size() + 1;
			v.resize(hold);
			v[hold].reset(new Boolean(false));
			f + 5;
			++weight;
		}
	}
};

struct Num : value {
	double ber;

	Num() = default;

	virtual void parse(std::string::iterator &f, std::vector<std::unique_ptr<value>> &v, int &weight) {
		int x;
		Num y;
		while (isdigit(*f)) {
			x = toInt(f);
			y.ber = concat(y.ber, x);
			f++;
			if (*f == '.') {
				++f;
				while (isdigit(*f)) {
					int p = toInt(f);
					y.ber = toDouble(y.ber, p);
					++f;
				}
			}
		}
		Num * z = new Num();
		z = &y;
		int hold = v.size() + 1;
		v.resize(hold);
		v[hold].reset(z);
		++weight;
		
	}
};

struct String : value, std::string {
	using std::string::string;

	virtual void parse(std::string::iterator &f, std::vector<std::unique_ptr<value>> &v, int &weight) {
		String text;
		while (*f != '\"') {
			text += *f;
			++f;
		}
		String * x = new String();
		x = &text;
		int hold = v.size() + 1;
		v.resize(hold);
		v[hold].reset(x);
		++weight;
	}
};

struct Array : value, std::vector<value *> {
	Array() = default;
	~Array() {
		for (value * v : *this) delete v;
	}

	using std::vector<value *>::push_back;

	virtual void parse(std::string::iterator &f, std::vector<std::unique_ptr<value>> &v, int &weight) {

	}
};

struct Object : value, std::map<std::string, value *> {
	using std::map<std::string, value *>::map;
	using std::map<std::string, value *>::insert;

	virtual void parse(std::string::iterator &f, std::vector<std::unique_ptr<value>> &v, int &weight) {

	}
};


//Skips spaces and unnecessary special characters
void skip(std::string::iterator & f, std::string::iterator l) {
	while ((f != l) || std::isspace(*f) || (*f == ',') || (*f == ':')) ++f;
}


//Application Implementation
int main() {

	//Obtains a string of JSON data from a text file
	//NOTE: User must provide file format as "fileName.txt"

	int weight = 0;
	std::string name, json;
	std::vector<std::unique_ptr<value>> v;

	std::cout << "Enter JSON txt file:\n";
	std::cin >> name;
	std::ifstream file(name);

	if (file.is_open()) {
		while (std::getline(file, json)) file >> json;
		file.close();
	}

	else {
		std::cout << "\nFile not Found\n";
		return 0;
	}

	std::string::iterator f = json.begin();
	std::string::iterator l = json.end();

	while (f != l) {

		if (*f == 'n') {
			Null hold;
			hold.parse(f, v, weight);
		}

		else if (*f == 't' || *f == 'f') {
			Boolean hold;
			hold.parse(f, v, weight);
		}

		else if (*f == '"') {
			String hold;
			hold.parse(f, v, weight);
		}

		else if (isdigit(*f)) {
			Num hold;
			hold.parse(f, v, weight);
		}

		else if (*f == '[') {

		}

		else if (*f == '{') {

		}

	}


	return 0;
}