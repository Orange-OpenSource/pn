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

#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gawkapi.h>

#include "gawkpnimpl.h"

#define make_const_strz(str, result)	r_make_string(api, ext_id, str, (sizeof(str) - 1), 1, result)

int plugin_is_GPL_compatible;

gawk_api_t *api;
awk_ext_id_t ext_id;
const char *ext_version = NULL;

awk_value_t *do_pn_set_country(int num_actual_args, awk_value_t *result) {
	awk_value_t arg1;

	if (get_argument(0, AWK_STRING, &arg1)) {
		set_country(arg1.u.s.str, arg1.u.s.len);
	}
	make_null_string(result);
	return result;
}

awk_value_t *do_pn_set_format(int num_actual_args, awk_value_t *result) {
	awk_value_t arg1;

	if (get_argument(0, AWK_STRING, &arg1)) {
		set_format(arg1.u.s.str, arg1.u.s.len);
	}
	make_null_string(result);
	return result;
}

awk_value_t *do_pn_set_leniency(int num_actual_args, awk_value_t *result) {
	awk_value_t arg1;

	if (get_argument(0, AWK_STRING, &arg1)) {
		set_leniency(arg1.u.s.str, arg1.u.s.len);
	}
	make_null_string(result);
	return result;
}

awk_value_t *do_pn_format(int num_actual_args, awk_value_t *result) {
	awk_value_t arg1;

	char *res;
	size_t res_len;

	if ((get_argument(0, AWK_STRING, &arg1))
			&& pn_format(arg1.u.s.str, arg1.u.s.len, &res, &res_len))
	{
		make_const_string(res, res_len, result);
		free(res);
	} else {
		make_null_string(result);
	}

	return result;
}

awk_value_t *do_pn_valid(int num_actual_args, awk_value_t *result) {
	awk_value_t arg1;

	int res;

	if (get_argument(0, AWK_STRING, &arg1))
	{
		res = pn_valid(arg1.u.s.str, arg1.u.s.len);
		make_number((double)res, result);
	} else {
		make_null_string(result);
	}

	return result;
}

awk_value_t *do_pn_find(int num_actual_args, awk_value_t *result) {
	awk_value_t arg1, arg2;
	struct str_list *pnl, *tmp;
	awk_value_t index, value;
	int i;

	int res;

	if (get_argument(0, AWK_STRING, &arg1) && get_argument(1, AWK_ARRAY, &arg2))
	{
		clear_array(arg2.u.a);
		pnl = pn_find(arg1.u.s.str, arg1.u.s.len);
		i = 0;
		while(pnl) {
			make_const_string(pnl->str, pnl->len, &index);
			make_number((double)1, &value);
			set_array_element(arg2.u.a, &index, &value);
			tmp = pnl;
			pnl = pnl->prev;
			free(tmp);
			i++;
		}
	} else {
		make_null_string(result);
	}

	return result;
}

awk_value_t *do_pn_info(int num_actual_args, awk_value_t *result) {
	awk_value_t arg1, arg2;
	awk_value_t index, value;

	struct pn_info res;

	memset(&res, 0, sizeof(res));

	if (get_argument(0, AWK_STRING, &arg1) && get_argument(1, AWK_ARRAY, &arg2))
	{
		clear_array(arg2.u.a);
		if (pn_info(arg1.u.s.str, arg1.u.s.len, &res)) {
			make_const_strz("prefix", &index);
			make_number((double)res.country_code, &value);
			set_array_element(arg2.u.a, &index, &value);

			make_const_strz("country", &index);
			make_const_string(res.country_name, 2, &value);
			set_array_element(arg2.u.a, &index, &value);

			make_const_strz("type", &index);
			make_number((double)res.type, &value);
			set_array_element(arg2.u.a, &index, &value);

			make_const_strz("location", &index);
			make_const_string(res.location, res.location_len, &value);
			set_array_element(arg2.u.a, &index, &value);
			if (res.location_len != 0) {
				free(res.location);
			}

			make_const_strz("possible_short_number", &index);
			make_number((double)res.possible_short_number, &value);
			set_array_element(arg2.u.a, &index, &value);

			make_const_strz("valid_short_number", &index);
			make_number((double)res.valid_short_number, &value);
			set_array_element(arg2.u.a, &index, &value);

			make_const_strz("emergency_number", &index);
			make_number((double)res.emergency_number, &value);
			set_array_element(arg2.u.a, &index, &value);
		}
	} else {
		make_null_string(result);
	}

	return result;
}

static awk_ext_func_t func_table[] = {
    { "pn_format", do_pn_format, 1 },
	{ "pn_valid", do_pn_valid, 1 },
	{ "pn_find", do_pn_find, 2 },
	{ "pn_info", do_pn_info, 2 },
	{ "pn_set_country", do_pn_set_country, 1 },
	{ "pn_set_format", do_pn_set_format, 1 },
	{ "pn_set_leniency", do_pn_set_leniency, 1 },
};

static awk_bool_t init_func(void) {
	return pn_init() ? awk_true : awk_false;
}

dl_load_func(func_table, "phonenumber", "phonenumber")
