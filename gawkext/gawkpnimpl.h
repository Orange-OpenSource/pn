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

#ifndef SRC_GAWKPNIMPL_H_
#define SRC_GAWKPNIMPL_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct str_list {
	struct str_list *prev;
	size_t len;
	char str[1];
};

enum number_type {
  FIXED_LINE,
  MOBILE,
  // In some regions (e.g. the USA), it is impossible to distinguish between
  // fixed-line and mobile numbers by looking at the phone number itself.
  FIXED_LINE_OR_MOBILE,
  // Freephone lines
  TOLL_FREE,
  PREMIUM_RATE,
  // The cost of this call is shared between the caller and the recipient, and
  // is hence typically less than PREMIUM_RATE calls. See
  // http://en.wikipedia.org/wiki/Shared_Cost_Service for more information.
  SHARED_COST,
  // Voice over IP numbers. This includes TSoIP (Telephony Service over IP).
  VOIP,
  // A personal number is associated with a particular person, and may be
  // routed to either a MOBILE or FIXED_LINE number. Some more information can
  // be found here: http://en.wikipedia.org/wiki/Personal_Numbers
  PERSONAL_NUMBER,
  PAGER,
  // Used for "Universal Access Numbers" or "Company Numbers". They may be
  // further routed to specific offices, but allow one number to be used for a
  // company.
  UAN,
  // Used for "Voice Mail Access Numbers".
  VOICEMAIL,
  // A phone number is of type UNKNOWN when it does not fit any of the known
  // patterns for a specific region.
  UNKNOWN
};

struct pn_info {
	uint32_t country_code;
	char country_name[2];
	enum number_type type;
	char *location;
	size_t location_len;
	uint32_t possible_short_number:1;
	uint32_t valid_short_number:1;
	uint32_t emergency_number:1;
};

int pn_init(void);

int pn_format(char *num, size_t num_len, char **res, size_t *res_len);

int pn_valid(char *num, size_t num_len);

struct str_list *pn_find(char *text, size_t text_len);

int set_format(char *str, size_t len);

int set_leniency(char *str, size_t len);

void set_country(char *str, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* SRC_GAWKPNIMPL_H_ */
