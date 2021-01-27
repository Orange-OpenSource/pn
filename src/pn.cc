/*
 * pn - a command-line tool for phone number manipulation
 *
 * Copyright 2017 Orange
 * <camille.oudot@orange.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <getopt.h>
#include <string.h>
#include <limits>
#include <phonenumbers/phonenumbermatcher.h>
#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/phonenumbermatch.h>
#include <phonenumbers/shortnumberinfo.h>
#include <phonenumbers/phonemetadata.pb.h>
#include <phonenumbers/geocoding/phonenumber_offline_geocoder.h>

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

bool parse_leniency(char *s, enum PhoneNumberMatcher::Leniency *l)
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
			if (!parse_leniency(optarg, &matcher_leniency)) {
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

const char *num_type_descr(PhoneNumberUtil::PhoneNumberType pnt)
{
	switch (pnt) {
	case PhoneNumberUtil::FIXED_LINE:
		return "fixed line";
		break;
	case PhoneNumberUtil::MOBILE:
		return "mobile";
		break;
	case PhoneNumberUtil::FIXED_LINE_OR_MOBILE:
		return "fixed line or mobile";
		break;
	case PhoneNumberUtil::TOLL_FREE:
		return "toll free";
		break;
	case PhoneNumberUtil::PREMIUM_RATE:
		return "premium rate";
		break;
	case PhoneNumberUtil::SHARED_COST:
		return "shared cost";
		break;
	case PhoneNumberUtil::VOIP:
		return "voip";
		break;
	case PhoneNumberUtil::PERSONAL_NUMBER:
		return "personal number";
		break;
	case PhoneNumberUtil::PAGER:
		return "pager";
		break;
	case PhoneNumberUtil::UAN:
		return "uan";
		break;
	case PhoneNumberUtil::VOICEMAIL:
		return "voicemail";
		break;
	case PhoneNumberUtil::UNKNOWN:
	default:
		return "unknown";
		break;
	}
}

int info(int argc, char *argv[])
{
	int c;
	while ((c = getopt(argc, argv, "c:")) != -1) {
		switch((char)c) {
		case 'c':
			country_code = optarg;
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
	const string raw_num = argv[optind];

	if (pnu->Parse(raw_num, country_code, &pn) != PhoneNumberUtil::NO_PARSING_ERROR) {
		cerr << "could not parse number: " << argv[optind] << endl;
		return 1;
	}

	const ShortNumberInfo sni;
	const PhoneNumberOfflineGeocoder pnog;
	const Locale loc("C");
	string s;

	pnu->GetRegionCodeForCountryCode(pn.country_code(), &s);
	cout << "country code: " << pn.country_code() << " (" << s << ")" << endl;
	cout << "number type: " << num_type_descr(pnu->GetNumberType(pn)) << endl;
	cout << "location: " << pnog.GetDescriptionForNumber(pn, loc) << endl;
	cout << "possible short number: " << (sni.IsPossibleShortNumber(pn) ? "true" : "false") << endl;
	cout << "valid short number: " << (sni.IsValidShortNumber(pn) ? "true" : "false") << endl;
	cout << "emergency number: "<< (sni.IsEmergencyNumber(raw_num, country_code) ? "true" : "false") << endl;

	return 0;
}

int valid(int argc, char *argv[])
{
	int c, ret;
	bool verbose = false;
	string msg;
	while ((c = getopt(argc, argv, "c:v")) != -1) {
		switch((char)c) {
		case 'c':
			country_code = optarg;
			break;
		case 'v':
			verbose = true;
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
	const string raw_num = argv[optind];

	if (pnu->Parse(raw_num, country_code, &pn) != PhoneNumberUtil::NO_PARSING_ERROR) {
		switch (pnu->Parse(raw_num, country_code, &pn)) {
		case PhoneNumberUtil::INVALID_COUNTRY_CODE_ERROR:
			msg = "invalid country code";
			break;
		case PhoneNumberUtil::NOT_A_NUMBER:
			msg = "not a number";
			break;
		case PhoneNumberUtil::TOO_SHORT_AFTER_IDD:
			msg = "too short after international dialing prefix";
			break;
		case PhoneNumberUtil::TOO_SHORT_NSN:
			msg = "too short";
			break;
		case PhoneNumberUtil::TOO_LONG_NSN:
			msg = "too long";
			break;
		default:
			msg = "unknown error";
			break;
		}
		if (verbose) {
			cout << msg << endl;
		}
		return 1;
	}

	const ShortNumberInfo sni;

	if (!pnu->IsValidNumber(pn)) {
		if (sni.IsValidShortNumber(pn)) {
			if (verbose) {
				cout << "valid short number" << endl;
			}
			return 0;
		}
		if (verbose) {
			cout << "invalid number" << endl;
		}
		return 1;
	}

	if (verbose) {
		cout << "valid number" << endl;
	}
	return 0;
}

int dialout(int argc, char *argv[])
{
	int c, ret;
	bool verbose = false;
	string msg;
	while ((c = getopt(argc, argv, "c:v")) != -1) {
		switch((char)c) {
		case 'c':
			country_code = optarg;
			break;
		case 'v':
			verbose = true;
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
	const string raw_num = argv[optind];

	if (pnu->Parse(raw_num, "ZZ", &pn) != PhoneNumberUtil::NO_PARSING_ERROR) {
		cerr << "invalid input number " << raw_num << " (must a valid number in e164 format, e.g. +124567890)" << endl;
		return 1;
	}

	string result;

	pnu->FormatOutOfCountryCallingNumber(pn, country_code, &result);

	if (result == "") {
		cerr << "couldn't determine the dial out sequence for " << raw_num << endl;
	}

	cout << result << endl;
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
	} else if (strcmp(argv[1], "info") == 0) {
		return info(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "valid") == 0) {
		return valid(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "dialout") == 0) {
		return dialout(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "-h") == 0) {
		//usage();
	} else {
		cerr << "invalid command: " << argv[1] << endl;
		return 1;
	}
}
