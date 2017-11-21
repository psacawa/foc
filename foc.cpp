#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <ncursesw/ncurses.h>
#include <unistd.h>
#include <clocale>

using namespace std;

void getDocuments (map<char,string>&);
void showDocuments (map <char, string> &doc);
void raiseDocument (map <char, string> &doc);
void processInput (map <char, string> &doc);

int main () {
	map <char,string> openDocuments;
//	nie naprawia błędne wyświetlanie polskich znaków
	setlocale(LC_CTYPE, "");
	initscr ();
	curs_set (0);
	noecho ();
	while (true) {
		getDocuments (openDocuments);
		showDocuments (openDocuments);
		processInput (openDocuments);
	}
	endwin ();
}

void getDocuments (map<char,string>& ret) {
	FILE* fp;
	char io [200];
	ret.clear ();
	set <string> doc;
	fp = popen ("wmctrl -l", "r");
	if (fp == NULL)
		exit (1);
	while (fgets (io, sizeof (io) -1, fp) != NULL) {
		if (strstr (io, "Okular") != NULL) {
			string str = string (io);
			string::size_type start = 0;
			for (int c = 0; c!= 4; ++c)
				start = str.find (' ', start)+1;
			string::size_type end;
			if ((end = str.find (".pdf")) != string::npos)
				end += strlen (".pdf");
			else if ((end = str.find (".djvu")) != string::npos)
				end += strlen (".djvu");
			else
				end = string::npos; // przydało by się naprawić ten przypadek
			string title = str.substr (start, end-start);
			doc.insert (title);
//			ret.insert (pair <char,string> ('a' + nfiles++, title));
		}
	}
	int nfiles = 0;
	for (set<string>::iterator it = doc.begin (); it != doc.end (); ++it)
		ret.insert (pair <char,string> ('a' + nfiles++, *it));

}

void showDocuments (map <char, string> &doc) {
	if (doc.size () == 0) {
		mvprintw (0,0, "Brak dokumentów do wyświetlenia");
		return;
	}
	for (map<char,string>::iterator it = doc.begin (); it != doc.end (); it++) {
		move (it->first - 'a', 0);
		printw ((it->first + ("\t" + it->second)).c_str ());
	}
}

void raiseDocument (string doc) {
	string command  ="wmctrl -a \"" + doc + "\"";
	system (command.c_str ());
}

void processInput (map <char, string> &doc) {
	int inp = getch ();
	clear ();
	if (inp == 4) {
		endwin();
		exit (0);
	} else if (inp == ' ') {
		clear ();
		return;
	}
	if (inp - 'a' < (int) doc.size ()) {
		raiseDocument (doc[inp]);
	}
}
