#include "functional.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void for_each(void (*func)(void *), array_t list)
{
	for (int i = 0; i < list.len; i++)
		func(list.data + i * list.elem_size);
}

array_t map(void (*func)(void *, void *),
			int new_list_elem_size,
			void (*new_list_destructor)(void *),
			array_t list)
{
	array_t new_list = {0};
	new_list.len = list.len;
	new_list.elem_size = new_list_elem_size;
	new_list.data = malloc(list.len * new_list_elem_size);
	new_list.destructor = new_list_destructor;

	for (int i = 0; i < list.len; i++)
		func(new_list.data + i * new_list_elem_size, list.data +
			 i * list.elem_size);

	if (list.destructor)
		for_each(list.destructor, list);

	free(list.data);

	return new_list;
}

array_t filter(boolean(*func)(void *), array_t list)
{
	array_t new_list = {0};
	new_list.len = 0;
	new_list.elem_size = list.elem_size;
	new_list.data = malloc(list.len * list.elem_size);
	new_list.destructor = list.destructor;

	for (int i = 0; i < list.len; i++)
		if (func(list.data + i * list.elem_size)) {
			memcpy(new_list.data + new_list.len * new_list.elem_size,
				   list.data + i * list.elem_size,
				   list.elem_size);
			new_list.len++;
		} else if (list.destructor) {
			list.destructor(list.data + i * list.elem_size);
		}

	free(list.data);

	return new_list;
}

void *reduce(void (*func)(void *, void *), void *acc, array_t list)
{
	for (int i = 0; i < list.len; i++)
		func(acc, list.data + i * list.elem_size);

	return acc;
}

void for_each_multiple(void(*func)(void **), int varg_c, ...)
{
	array_t *lists = malloc(varg_c * sizeof(array_t));
	va_list args;
	va_start(args, varg_c);

	int min_len = -1;
	for (int i = 0; i < varg_c; i++) {
		array_t list = va_arg(args, array_t);
		lists[i] = list;

		if (min_len == -1 || list.len < min_len)
			min_len = list.len;
	}

	va_end(args);

	for (int i = 0; i < min_len; i++) {
		void *data[varg_c];
		for (int j = 0; j < varg_c; j++)
			data[j] = lists[j].data + i * lists[j].elem_size;

		func(data);
	}

	free(lists);
}

array_t map_multiple(void (*func)(void *, void **),
					 int new_list_elem_size,
					 void (*new_list_destructor)(void *),
					 int varg_c, ...)
{
	array_t *lists = malloc(varg_c * sizeof(array_t));
	va_list args;
	va_start(args, varg_c);

	int min_len = -1;
	for (int i = 0; i < varg_c; i++) {
		array_t list = va_arg(args, array_t);
		lists[i] = list;

		if (min_len == -1 || list.len < min_len)
			min_len = list.len;
	}

	va_end(args);

	array_t new_list = {0};
	new_list.len = min_len;
	new_list.elem_size = new_list_elem_size;
	new_list.data = malloc(min_len * new_list_elem_size);
	new_list.destructor = new_list_destructor;

	for (int i = 0; i < min_len; i++) {
		void *data[varg_c];
		for (int j = 0; j < varg_c; j++)
			data[j] = lists[j].data + i * lists[j].elem_size;

		func(new_list.data + i * new_list_elem_size, data);
	}

	for (int i = 0; i < varg_c; i++) {
		if (lists[i].destructor)
			for_each(lists[i].destructor, lists[i]);
		free(lists[i].data);
	}

	free(lists);

	return new_list;
}

void *reduce_multiple(void(*func)(void *, void **), void *acc, int varg_c, ...)
{
	array_t *lists = malloc(varg_c * sizeof(array_t));
	va_list args;
	va_start(args, varg_c);

	int min_len = -1;
	for (int i = 0; i < varg_c; i++) {
		array_t list = va_arg(args, array_t);
		lists[i] = list;

		if (min_len == -1 || list.len < min_len)
			min_len = list.len;
	}

	va_end(args);

	for (int i = 0; i < min_len; i++) {
		void *data[varg_c];
		for (int j = 0; j < varg_c; j++)
			data[j] = lists[j].data + i * lists[j].elem_size;

		func(acc, data);
	}

	free(lists);
	return acc;
}
