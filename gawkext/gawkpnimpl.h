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
  FIXED_LINE_OR_MOBILE,
  TOLL_FREE,
  PREMIUM_RATE,
  SHARED_COST,
  VOIP,
  PERSONAL_NUMBER,
  PAGER,
  UAN,
  VOICEMAIL,
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

int pn_info(char *num, size_t num_len, struct pn_info* res);

int set_format(char *str, size_t len);

int set_leniency(char *str, size_t len);

void set_country(char *str, size_t len);

void pn_dialout(char *num, size_t num_len, char *country_code, size_t cc_len, char **res, size_t *res_len);

#ifdef __cplusplus
}
#endif

#endif /* SRC_GAWKPNIMPL_H_ */
