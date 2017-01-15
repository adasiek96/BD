#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "sqlite3.h"

using namespace std;

#define N 100

string string_of_time(pair<int, int> para) {
	string HH = "", MM = "", W = "";
	if (para.first <= 9) HH += '0';
	HH += to_string(para.first);
	if (para.second <= 9) MM += '0';
	MM += to_string(para.second);
	W += HH;
	W += ":";
	W += MM;
	return W;
}

bool file_exists(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}

bool correct_bus_stop_or_line(int k, int bus_stop) {
	return bus_stop < k && bus_stop > 0;
}

void read_parse_int(int* x) {
	while (!(cin >> *x)) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input.  Try again: ";
	}
}

void read_parse_hour(pair<int, int>* para) {
	int x, y;
	x = para->first;
	y = para->second;
	string time, HH = "", MM = "";
	while (true) {
		cin >> time;
		if (time.size() == 5) {
			HH = "";
			MM = "";
			HH += time[0];
			HH += time[1];
			MM += time[3];
			MM += time[4];
			if ((char)48 <= HH[0] && HH[0] <= (char)57 && (char)48 <= MM[0] && MM[0] <= (char)57 &&
				(char)48 <= HH[1] && HH[1] <= (char)57 && (char)48 <= MM[1] && MM[1] <= (char)57) {
				x = stoi(HH);
				y = stoi(MM);
				if (x < 24 && y < 60 && x >= 0 && y >= 0) {
					*para = make_pair(x, y);
					return;
				}
			}
		}
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input.  Try again: ";
	}
}

void read_bus_stops(string przystanki[N]) {
	fstream fin;
	fin.open("przystanki_no_pl.txt", ios::in);
	for (int i = 0; i < N; i++) getline(fin, przystanki[i]);
	fin.close();
}

void parse_list_of_bus_stops(string przystanki[N], int tab[200], int* k) {
	cout << "Choose a bus stop from the list below by typing the proper number." << endl;
	int l = 0;
	string file;
	for (int i = 0; i < N; i++) {
		file = "tmp/__";
		file += przystanki[i][0];
		file += przystanki[i][1];
		file += przystanki[i][2];
		file += przystanki[i][3];
		file += "_01.db";
		if (file_exists(file)) {
			cout << "(" << *k << ") ";
			for (size_t j = 5; j < przystanki[i].size(); j++)
				cout << przystanki[i][j];
			cout << ", 01" << endl;
			tab[(*k)] = l;
			(*k)++;
		}
		l++;
		file[12] = '2';
		if (file_exists(file)) {
			cout << "(" << *k << ") ";
			for (size_t j = 5; j < przystanki[i].size(); j++)
				cout << przystanki[i][j];
			cout << ", 02" << endl;
			tab[(*k)] = l;
			(*k)++;
		}
		l++;
	}
	cout << "Choose a bus stop from the list above by typing the proper number." << endl;
	cout << "My choice is:   ";
}

void parse_result_of_bus_stops(string przystanki[N], int tab[200], int* k, int bus_stop, string* bus_stop_nr_id) {
	cout << "You have chosen the bus stop:   ";
	string name;
	*bus_stop_nr_id = "__";
	for (int j = 0; j < 4; j++)
		*bus_stop_nr_id += przystanki[tab[bus_stop] / 2][j];
	for (size_t j = 5; j < przystanki[tab[bus_stop] / 2].size(); j++)
		name += przystanki[tab[bus_stop] / 2][j];
	if (tab[bus_stop] % 2 == 0) {
		name += ", 01";
		*bus_stop_nr_id += "_01";
	}
	else {
		name += ", 02";
		*bus_stop_nr_id += "_02";
	}
	cout << name << endl;
	//cout << *bus_stop_nr_id << endl;
}

bool parse_result_of_hour(pair<int, int>* from, pair<int, int>* to) {
	cout << "Choose an hour (from, to). The hour format is HH:MM, for example: 07:48." << endl;
	cout << "from:   ";
	read_parse_hour(from);
	cout << "to:     ";
	read_parse_hour(to);
	if (*from < *to) {
		cout << "You have chosen the time from " << string_of_time(*from) << " to " << string_of_time(*to) << "." << endl;
		return true;
	}
	cout << "Error: 'from' must be less than 'to'. Try again." << endl;
	return false;
}

void parse_result_of_1_hour(pair<int, int>* from, pair<int, int>* to) {
	cout << "Choose an hour. The hour format is HH:MM, for example: 07:48." << endl;
	cout << "departures from:   ";
	read_parse_hour(from);
	*to = *from;
	to->first++;
	if (to->first == 24) {
		to->first = 23;
		to->second = 59;
	}
	cout << "You have chosen the time from " << string_of_time(*from) << " to " << string_of_time(*to) << "." << endl;
}

void read_bus_lines(string linie[N], string bus_stop_nr_id) {
	sqlite3 *db;
	string path = "tmp/";
	path += bus_stop_nr_id;
	path += ".db";
	sqlite3_open(path.c_str(), &db);

	const char *sqlGetTables = "SELECT name FROM sqlite_master WHERE type='table';";
	char **results = NULL;
	char *error;
	int rows, columns;

	sqlite3_get_table(db, sqlGetTables, &results, &rows, &columns, &error);
	for (int i = 1; i <= rows; i++)
		linie[i - 1] = results[i];
	linie[rows] = "END";

	sqlite3_free_table(results);
	sqlite3_close(db);
}

void parse_list_of_bus_lines(string linie[N], int* k) {
	cout << "Choose a bus line from the list below by typing the proper number." << endl;
	for (int i = 0; i < N; i++) {
		if (linie[i] == "END") break;
		cout << "(" << i + 1 << ") ";
		for (size_t j = 1; j < linie[i].size(); j++)
			if (linie[i][j] == '_') cout << "-";
			else cout << linie[i][j];
		(*k)++;
		cout << endl;
	}
	cout << "My choice is:   ";
}

void parse_result_of_bus_lines(string linie[N], int bus_line, string* bus_line_name) {
	cout << "You have chosen the line:   ";
	string name = "";
	*bus_line_name += linie[bus_line - 1];
	for (size_t i = 1; i < linie[bus_line - 1].size(); i++)
		if (linie[bus_line - 1][i] != '_') name += linie[bus_line - 1][i];
		else name += '-';
	cout << name << endl;
	//cout << *bus_line_name << endl;
}

void query(vector<string>* times, const string& bus_stop, pair<int, int> from, pair<int, int> to, const string& line) {
	sqlite3 *db;
	string path = "tmp/";
	path += bus_stop;
	path += ".db";
	sqlite3_open(path.c_str(), &db);

	ostringstream oss;
	oss << "SELECT czas FROM " << line << " ";
	oss << "WHERE czas >= " << "'" << string_of_time(from) << "' ";
	oss << "AND czas <= " << "'" << string_of_time(to) << "'";
	string var = oss.str();
	const char *sqlGetTables = var.c_str();
	char **results = NULL;
	char *error;
	int rows, columns;

	sqlite3_get_table(db, sqlGetTables, &results, &rows, &columns, &error);
	for (int i = 1; i <= rows; i++)
		(*times).push_back(results[i]);

	sqlite3_free_table(results);
	sqlite3_close(db);
}

void answer_1(string& bus_stop, pair<int, int> from, pair<int, int> to, string& line) {
	cout << endl;

	vector<string> times;
	query(&times, bus_stop, from, to, line);
	int time = (60 * to.first + to.second) - (60 * from.first + from.second);
	double waiting_time = 1.0 * time / times.size();
	cout << "Average waiting time between " << string_of_time(from) << " and " << string_of_time(to) << " is ";
	if (times.size() != 0) cout << (int)(round(waiting_time / 2));
	else cout << "more than " << time;
	cout << " minutes." << endl;

	cout << "Press 'Enter' to continue..." << endl;
	cin.ignore().get();
}

void answer_2(string& bus_stop, pair<int, int> from, pair<int, int> to, string& line, string linie[N]) {
	cout << endl;

	vector<vector<string>> times;
	size_t sum_of_sizes = 0;
	int i = 0;
	while (linie[i] != "END") {
		vector<string> new_vector;
		times.push_back(new_vector);
		query(&times[i], bus_stop, from, to, linie[i]);
		sum_of_sizes += times[i].size();
		i++;
	}
	vector<string> new_vector;
	query(&new_vector, bus_stop, from, to, line);
	double p = 1.0 * new_vector.size() / sum_of_sizes;
	cout << "The probability, that the next bus arriving is the bus you have seleted from the" << endl;
	cout << "list, equals ";
	if (new_vector.size() == 0) cout << "0." << endl;
	else cout << std::fixed << std::setprecision(2) << p << "." << endl;

	cout << "Press 'Enter' to continue..." << endl;
	cin.ignore().get();
}

void answer_3(string& bus_stop, pair<int, int> from, pair<int, int> to, string linie[N]) {
	cout << endl;

	vector<pair<string, string>> times;
	int i = 0;
	while (linie[i] != "END") {
		vector<string> new_vector;
		query(&new_vector, bus_stop, from, to, linie[i]);
		for (auto v : new_vector) {
			pair<string, string> para = make_pair(v, linie[i]);
			times.push_back(para);
		}
		i++;
	}
	sort(times.begin(), times.end());
	string tt = "";
	for (auto t : times) {
		if (tt != t.first) {
			if (tt != "") cout << endl;
			for (int j = 0; j < 5; j++) cout << t.first[j];
			cout << " :   ";
			tt = t.first;
		}
		for (size_t j = 1; j < t.second.size(); j++) cout << t.second[j];
		cout << " ";
	}
	cout << endl;

	cout << "Press 'Enter' to continue..." << endl;
	cin.ignore().get();
}

void answer_4(string& bus_stop, pair<int, int> from, pair<int, int> to, string linie[N]) {
	cout << endl;

	size_t sum_of_sizes = 0;
	int i = 0;
	int time = (60 * to.first + to.second) - (60 * from.first + from.second);
	while (linie[i] != "END") {
		vector<string> new_vector;
		query(&new_vector, bus_stop, from, to, linie[i]);
		double res = 1.0 * time / new_vector.size();
		for (size_t j = 1; j < linie[j].size(); j++) cout << linie[i][j];
		if (new_vector.size() != 0 && new_vector.size() != 1)
			cout << " :   ariving every " << (int)(round(res)) << " minutes" << endl;
		else if (new_vector.size() == 1)
			cout << " :   ariving less often than every " << (int)(round(res)) << " minutes" << endl;
		else
			cout << " :   not arriving during this period of time" << endl;
		sum_of_sizes += new_vector.size();
		i++;
	}
	double res = 1.0 * time / sum_of_sizes;
	if (sum_of_sizes != 0 && sum_of_sizes != 1) cout << "any line :   ariving every " << (int)(round(res)) << " minutes" << endl;
	else if (sum_of_sizes == 1) cout << " :   ariving less often than every " << (int)(round(res)) << " minutes" << endl;
	else cout << "any line :   not arriving during this period of time" << endl;

	cout << "Press 'Enter' to continue..." << endl;
	cin.ignore().get();
}

void answer_5(string& bus_stop_name, string& bus_stop, string linie[N]) {
	cout << endl;

	cout << bus_stop_name << endl;
	cout << "available lines: ";
	int i = 0;
	while (linie[i] != "END") {
		for (size_t j = 1; j < linie[i].size(); j++) cout << linie[i][j];
		cout << " ";
		i++;
	}
	cout << endl;

	cout << "Press 'Enter' to continue..." << endl;
	cin.ignore().get();
}

void answer_6(string& bus_stop, string& line) {
	cout << endl;

	vector<string> time;
	pair<int, int> from = make_pair(0, 0);
	pair<int, int> to = make_pair(23, 59);
	query(&time, bus_stop, from, to, line);

	string tt = "";
	for (auto t : time) {
		string temp = "";
		temp += t[0];
		temp += t[1];
		if (tt != temp) {
			if (tt != "") cout << endl;
			cout << temp;
			cout << " :   ";
			tt = temp;
		}
		string temp2 = "";
		temp2 += t[3];
		temp2 += t[4];
		cout << temp2 << " ";
	}
	cout << endl;

	cout << "Press 'Enter' to continue..." << endl;
	cin.ignore().get();
}

void option_1() {
	int bus_stop, bus_line, tab[200], k = 1, k2 = 1;
	pair<int, int> from, to;
	string bus_stop_nr_id, bus_line_name, przystanki[N], linie[N];


	read_bus_stops(przystanki);

	parse_list_of_bus_stops(przystanki, tab, &k);

	while (true) {
		read_parse_int(&bus_stop);
		if (correct_bus_stop_or_line(k, bus_stop)) break;
		else cout << "Incorrect number. Try again:   ";
	}

	parse_result_of_bus_stops(przystanki, tab, &k, bus_stop, &bus_stop_nr_id);


	while (true) {
		bool b = parse_result_of_hour(&from, &to);

		if (b) break;

	}

	read_bus_lines(linie, bus_stop_nr_id);

	parse_list_of_bus_lines(linie, &k2);

	while (true) {
		read_parse_int(&bus_line);
		if (correct_bus_stop_or_line(k2, bus_line)) break;
		else cout << "Incorrect number. Try again:   ";
	}

	parse_result_of_bus_lines(linie, bus_line, &bus_line_name);

	answer_1(bus_stop_nr_id, from, to, bus_line_name);

}

void option_2() {
	int bus_stop, bus_line, tab[200], k = 1, k2 = 1;
	pair<int, int> from, to;
	string bus_stop_nr_id, bus_line_name, przystanki[N], linie[N];


	read_bus_stops(przystanki);

	parse_list_of_bus_stops(przystanki, tab, &k);

	while (true) {
		read_parse_int(&bus_stop);
		if (correct_bus_stop_or_line(k, bus_stop)) break;
		else cout << "Incorrect number. Try again:   ";
	}

	parse_result_of_bus_stops(przystanki, tab, &k, bus_stop, &bus_stop_nr_id);


	parse_result_of_1_hour(&from, &to);

	read_bus_lines(linie, bus_stop_nr_id);

	parse_list_of_bus_lines(linie, &k2);

	while (true) {
		read_parse_int(&bus_line);
		if (correct_bus_stop_or_line(k2, bus_line)) break;
		else cout << "Incorrect number. Try again:   ";
	}

	parse_result_of_bus_lines(linie, bus_line, &bus_line_name);

	answer_2(bus_stop_nr_id, from, to, bus_line_name, linie);

}

void option_4(int v) {
	int bus_stop, tab[200], k = 1;
	pair<int, int> from, to;
	string bus_stop_nr_id, przystanki[N], linie[N];


	read_bus_stops(przystanki);

	parse_list_of_bus_stops(przystanki, tab, &k);

	while (true) {
		read_parse_int(&bus_stop);
		if (correct_bus_stop_or_line(k, bus_stop)) break;
		else cout << "Incorrect number. Try again:   ";
	}

	parse_result_of_bus_stops(przystanki, tab, &k, bus_stop, &bus_stop_nr_id);


	parse_result_of_1_hour(&from, &to);

	read_bus_lines(linie, bus_stop_nr_id);

	if (v == 4) answer_4(bus_stop_nr_id, from, to, linie);
	else if (v == 3) answer_3(bus_stop_nr_id, from, to, linie);

}

void option_3() {
	option_4(3);
}

void option_5() {
	int bus_stop, tab[200], k = 1;
	string bus_stop_nr_id, przystanki[N], linie[N];


	read_bus_stops(przystanki);

	parse_list_of_bus_stops(przystanki, tab, &k);

	while (true) {
		read_parse_int(&bus_stop);
		if (correct_bus_stop_or_line(k, bus_stop)) break;
		else cout << "Incorrect number. Try again:   ";
	}

	parse_result_of_bus_stops(przystanki, tab, &k, bus_stop, &bus_stop_nr_id);

	read_bus_lines(linie, bus_stop_nr_id);

	string p = "";
	for (size_t j = 5; j < przystanki[tab[bus_stop] / 2].size(); j++) p += przystanki[tab[bus_stop] / 2][j];
	if (tab[bus_stop] % 2 == 0) p += ", 01";
	else p += ", 02";
	answer_5(p, bus_stop_nr_id, linie);

}

void option_6() {
	int bus_stop, bus_line, tab[200], k = 1, k2 = 1;
	string bus_stop_nr_id, bus_line_name, przystanki[N], linie[N];

	read_bus_stops(przystanki);

	parse_list_of_bus_stops(przystanki, tab, &k);

	while (true) {
		read_parse_int(&bus_stop);
		if (correct_bus_stop_or_line(k, bus_stop)) break;
		else cout << "Incorrect number. Try again:   ";
	}

	parse_result_of_bus_stops(przystanki, tab, &k, bus_stop, &bus_stop_nr_id);

	read_bus_lines(linie, bus_stop_nr_id);

	parse_list_of_bus_lines(linie, &k2);

	while (true) {
		read_parse_int(&bus_line);
		if (correct_bus_stop_or_line(k2, bus_line)) break;
		else cout << "Incorrect number. Try again:   ";
	}

	parse_result_of_bus_lines(linie, bus_line, &bus_line_name);

	answer_6(bus_stop_nr_id, bus_line_name);

}

void option_0() {
	cout << "Tha databases are being updated. It may take up to 30 minutes." << endl;
	cout << "Be patient, please..." << endl;
	system("./update.sh");
	if (file_exists("status.txt"))
		cout << "Database updated successfully." << endl;
	else
		cout << "Error while updating databases." << endl;
	cout << endl;
}

int main() {
	cout << "Welcome to ZTM Checker!" << endl << endl;

	while (true) {
		int number;

		cout << "Choose an option from the list below by typying number from 1 to 7." << endl;
		cout << "(1) average waiting time for a bus/tram depending on an hour (from, to), a bus" << endl;
		cout << "    stop and a bus line" << endl;
		cout << "(2) probability of coming across the specific bus/tram line depending on hour" << endl;
		cout << "    (in 1 hour time)" << endl;
		cout << "(3) all upcoming connections (in 1 hour time) from the specific bus stop " << endl;
		cout << "(4) frequency of bus arriving (averagely) on the specific bus stop depending on" << endl;
		cout << "    an hour (in 1 hour time)" << endl;
		cout << "(5) list of bus/tram lines depending on the specific bus stop" << endl;
		cout << "(6) timetable of a bus/tram depending on a bus stop and a bus line" << endl;
		cout << "(0) update the hole database (attention: may take several minutes!)" << endl;
		cout << "My choise is:   ";

		read_parse_int(&number);

		switch (number) {
		case 1:
			option_1();
			break;
		case 2:
			option_2();
			break;
		case 3:
			option_3();
			break;
		case 4:
			option_4(4);
			break;
		case 5:
			option_5();
			break;
		case 6:
			option_6();
			break;
		case 0:
			option_0();
			break;
		default:
			cout << endl << "Wrong number! Please try again." << endl << endl;
			break;

		}
	}

	cout << endl << endl;
	cout << "Press 'Enter' to continue..." << endl;
	cin.ignore().get();
	return 0;

}
