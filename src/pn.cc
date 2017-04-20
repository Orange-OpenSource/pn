#include <iostream>
#include <getopt.h>
#include <string.h>
#include <limits>
#include <phonenumbers/phonenumbermatcher.h>
#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/phonenumbermatch.h>

using namespace std;
using namespace i18n::phonenumbers;

string country_code = "ZZ";
enum PhoneNumberUtil::PhoneNumberFormat number_format = PhoneNumberUtil::E164;
enum PhoneNumberMatcher::Leniency matcher_leniency = PhoneNumberMatcher::VALID;


bool parse_format(char *s, enum PhoneNumberUtil::PhoneNumberFormat *f)
{
	if (strcmp(s, "e164") == 0) {
		*f = PhoneNumberUtil::E164;
	} else if (strcmp(s, "int") == 0) {
		*f = PhoneNumberUtil::INTERNATIONAL;
	} else if (strcmp(s, "nat") == 0) {
		*f = PhoneNumberUtil::NATIONAL;
	} else if (strcmp(s, "teluri") == 0) {
		*f = PhoneNumberUtil::RFC3966;
	} else {
		return false;
	}
	return true;
}

bool parse_leninency(char *s, enum PhoneNumberMatcher::Leniency *l)
{
	if (strcmp(s, "possible") == 0) {
		*l = PhoneNumberMatcher::POSSIBLE;
	} else if (strcmp(s, "valid") == 0) {
		*l = PhoneNumberMatcher::VALID;
	} else if (strcmp(s, "strict") == 0) {
		*l = PhoneNumberMatcher::STRICT_GROUPING;
	} else if (strcmp(s, "exact") == 0) {
		*l = PhoneNumberMatcher::EXACT_GROUPING;
	} else {
		return false;
	}
	return true;
}

int format(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "c:f:")) != -1) {
		switch((char)c) {
		case 'c':
			country_code = optarg;
			break;
		case 'f':
			if (!parse_format(optarg, &number_format)) {
				cerr << "invalid number format: " << optarg << endl;
				return 1;
			}
			break;
		default:
			return 1;
			break;
		}
	}

	if (optind == argc) {
		cerr << "missing the phone number as argument" << endl;
		return 1;
	}

	PhoneNumberUtil *pnu = PhoneNumberUtil::GetInstance();
	PhoneNumber pn;
	if (pnu->Parse(argv[optind], country_code, &pn) != PhoneNumberUtil::NO_PARSING_ERROR) {
		cerr << "could not parse number: " << argv[optind] << endl;
		return 1;
	}

	string s;
	pnu->Format(pn, number_format, &s);
	std::cout << s << std::endl;

	return 0;
}

int find(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "c:f:l:")) != -1) {
		switch((char)c) {
		case 'c':
			country_code = optarg;
			break;
		case 'f':
			if (!parse_format(optarg, &number_format)) {
				cerr << "invalid number format: " << optarg << endl;
				return 1;
			}
			break;
		case 'l':
			if (!parse_leninency(optarg, &matcher_leniency)) {
				cerr << "invalid leninency: " << optarg << endl;
				return 1;
			}
			break;
		default:
			return 1;
			break;
		}
	}

	if (optind == argc) {
		cerr << "missing the text to parse as argument" << endl;
		return 1;
	}

	PhoneNumberUtil *pnu = PhoneNumberUtil::GetInstance();
	PhoneNumberMatcher pnm(*pnu, argv[optind], country_code, matcher_leniency, numeric_limits<int>::max());
	PhoneNumberMatch match;
	string s;

	while (pnm.HasNext())
	{
		pnm.Next(&match);
		pnu->Format(match.number(), number_format, &s);
		std::cout << s << std::endl;

	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		// TODO usage
		return 1;
	}

	if (strcmp(argv[1], "format") == 0) {
		return format(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "find") == 0) {
		return find(argc - 1, &argv[1]);
	} else {
		cerr << "invalid command: " << argv[1] << endl;
		return 1;
	}
}
