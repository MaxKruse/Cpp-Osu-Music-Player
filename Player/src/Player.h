#define OPPAI_IMPLEMENTATION
#include "oppai.c"

std::ostream& operator<<(std::ostream& os, const parser& p)
{
	os << "Parser Data [" << '\n';
	os << "Artist: " << p.artist << '\n';
	os << "Artist_Unicode: " << p.artist_unicode << '\n';
	os << "Creator: " << p.creator << '\n';
	os << "Title: " << p.title << '\n';
	os << "Title Unicode: " << p.title_unicode << '\n';
	os << "Version: " << p.version;

	return os;
}