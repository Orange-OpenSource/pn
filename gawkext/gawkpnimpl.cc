/*
 * gawkpn - a gawk extension for phone number manipulation
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
#include <string>
#include <limits>

#include <string.h>

#include <phonenumbers/phonenumbermatcher.h>
#include <phonenumbers/phonenumberutil.h>
#include <phonenumbers/phonenumbermatch.h>
#include <phonenumbers/shortnumberinfo.h>
#include <phonenumbers/phonemetadata.pb.h>
#include <phonenumbers/geocoding/phonenumber_offline_geocoder.h>

#include "gawkpnimpl.h"

using namespace std;
using namespace i18n::phonenumbers;

static PhoneNumberUtil *pnu;
static const ShortNumberInfo sni;
const PhoneNumberOfflineGeocoder pnog;
const Locale loc("C");

static enum PhoneNumberUtil::PhoneNumberFormat number_format = PhoneNumberUtil::E164;
static enum PhoneNumberMatcher::Leniency matcher_leniency = PhoneNumberMatcher::VALID;
static string country_code = "ZZ";


extern "C" int set_format(char *str, size_t len)
{
	if (strncmp(str, "e164", len) == 0) {
		number_format = PhoneNumberUtil::E164;
	} else if (strncmp(str, "int", len) == 0) {
		number_format = PhoneNumberUtil::INTERNATIONAL;
	} else if (strncmp(str, "nat", len) == 0) {
		number_format = PhoneNumberUtil::NATIONAL;
	} else if (strncmp(str, "teluri", len) == 0) {
		number_format = PhoneNumberUtil::RFC3966;
	} else {
		return 0;
	}
	return 1;
}

extern "C" int set_leninency(char *str, size_t len)
{
	if (strncmp(str, "possible", len) == 0) {
		matcher_leniency = PhoneNumberMatcher::POSSIBLE;
	} else if (strncmp(str, "valid", len) == 0) {
		matcher_leniency = PhoneNumberMatcher::VALID;
	} else if (strncmp(str, "strict", len) == 0) {
		matcher_leniency = PhoneNumberMatcher::STRICT_GROUPING;
	} else if (strncmp(str, "exact", len) == 0) {
		matcher_leniency = PhoneNumberMatcher::EXACT_GROUPING;
	} else {
		return 0;
	}
	return 1;
}

extern "C" void set_country(char *str, size_t len) {
	country_code.assign(str, len);
}

extern "C" int pn_init(void) {
	pnu = PhoneNumberUtil::GetInstance();

	if(pnu == NULL) {
		return 0;
	}

	return 1;
}

extern "C" int pn_format(char *num, size_t num_len, char **res, size_t *res_len) {
	std::string snum, sres;

	snum.assign(num, num_len);
	PhoneNumber pn;
	if (pnu->Parse(snum, country_code, &pn) != PhoneNumberUtil::NO_PARSING_ERROR) {
		return 0;
	}

	pnu->Format(pn, number_format, &sres);

	*res = strdup(sres.c_str());
	*res_len = sres.length();
	return 1;
}

extern "C" void *pn_parse(char *num, size_t num_len) {
	std::string snum, sres;

	snum.assign(num, num_len);
	PhoneNumber *pn = new PhoneNumber();
	if (pnu->Parse(snum, country_code, pn) != PhoneNumberUtil::NO_PARSING_ERROR) {
		return (void *)pn;
	} else {
		return NULL;
	}
}

extern "C" int pn_valid(char *num, size_t num_len) {
	std::string snum, sres;

	snum.assign(num, num_len);
	PhoneNumber pn;
	if (pnu->Parse(snum, country_code, &pn) != PhoneNumberUtil::NO_PARSING_ERROR) {
		return 0;
	}

	if (!pnu->IsValidNumber(pn)) {
		if (sni.IsValidShortNumber(pn)) {
			return 2;
		}
		return 0;
	}
	return 1;
}

extern "C" struct str_list *pn_find(char *text, size_t text_len) {
	std::string stext, sres;
	struct str_list *elem, *tmp;
	PhoneNumberMatch match;
	PhoneNumber pn;

	stext.assign(text, text_len);

	PhoneNumberMatcher pnm(*pnu, stext, country_code, matcher_leniency, numeric_limits<int>::max());

	elem = NULL;
	while (pnm.HasNext())
	{
		pnm.Next(&match);
		pnu->Format(match.number(), number_format, &sres);
		tmp = (str_list*)malloc(sizeof(struct str_list) + sres.length() - 1);
		sres.copy(tmp->str, sres.length(), 0);
		tmp->len = sres.length();
		tmp->prev = elem;
		elem = tmp;
	}

	return elem;
}

enum number_type get_type(PhoneNumberUtil::PhoneNumberType pnt)
{
	switch (pnt) {
	case PhoneNumberUtil::FIXED_LINE:
		return FIXED_LINE;
		break;
	case PhoneNumberUtil::MOBILE:
		return MOBILE;
		break;
	case PhoneNumberUtil::FIXED_LINE_OR_MOBILE:
		return FIXED_LINE_OR_MOBILE;
		break;
	case PhoneNumberUtil::TOLL_FREE:
		return TOLL_FREE;
		break;
	case PhoneNumberUtil::PREMIUM_RATE:
		return PREMIUM_RATE;
		break;
	case PhoneNumberUtil::SHARED_COST:
		return SHARED_COST;
		break;
	case PhoneNumberUtil::VOIP:
		return VOIP;
		break;
	case PhoneNumberUtil::PERSONAL_NUMBER:
		return PERSONAL_NUMBER;
		break;
	case PhoneNumberUtil::PAGER:
		return PAGER;
		break;
	case PhoneNumberUtil::UAN:
		return UAN;
		break;
	case PhoneNumberUtil::VOICEMAIL:
		return VOICEMAIL;
		break;
	case PhoneNumberUtil::UNKNOWN:
	default:
		return UNKNOWN;
		break;
	}
}

extern "C" int pn_info(char *num, size_t num_len, struct pn_info* res) {
	string snum, s;

	snum.assign(num, num_len);
	PhoneNumber pn;
	if (pnu->Parse(snum, country_code, &pn) != PhoneNumberUtil::NO_PARSING_ERROR) {
		return 0;
	}

	res->country_code = pn.country_code();

	pnu->GetRegionCodeForCountryCode(pn.country_code(), &s);

	s.copy(res->country_name, 2, 0);

	res->type = get_type(pnu->GetNumberType(pn));

	s = pnog.GetDescriptionForNumber(pn, loc);

	res->location = (char*)malloc(s.length());
	res->location_len = s.length();
	s.copy(res->location, res->location_len, 0);
	res->possible_short_number = sni.IsPossibleShortNumber(pn) ? 1 : 0;
	res->valid_short_number = sni.IsValidShortNumber(pn) ? 1 : 0;
	res->emergency_number = sni.IsEmergencyNumber(snum, country_code) ? 1 : 0;

	return 1;
}
