#include "functional.h"
#include "tasks.h"
#include "tests.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/***************************** MY FUNCTIONS *****************************/

boolean check_grade(void *student) {
	if (((student_t *)student)->grade >= 5.0)
		return true;

	return false;
}

void number_distructor(void *number) {
	free(((number_t *)number)->string);
}

void get_name(void *name, void *student) {
	student_t *stud = (student_t *)student;
	*((char **)name) = strdup(stud->name);
}

void create_number(void *number, void **integers) {
	number_t *num = (number_t *)number;
	num->string = malloc(20);
	sprintf(num->string, "%d.%d", *(int *)integers[0], *(int *)integers[1]);
}

void suma(void *sum, void *list) {
	*(int *)sum += *(int *)list;
}

void check_sum(void *out_sum, void **lists)
{
	int *sum = calloc(1, sizeof(int));
	array_t *list = (array_t *)lists[0];
	int *int_list = (int *)lists[1];

	*sum = *((int *)reduce(suma, sum, *list));

	if (*sum >= *int_list)
		*(boolean *)out_sum = true;
	else
		*(boolean *)out_sum = false;

	free(sum);
}

void reverse_list(void *new_list, void *elem) {
	array_t *reversed = (array_t *)new_list;

	reversed->len--;
	memcpy(reversed->data + reversed->len * reversed->elem_size, elem,
		   reversed->elem_size);
}

void get_even_index(void *new_list, void *elem) {
	array_t *list = (array_t *)new_list;

	if (list->len % 2 == 0)
		memcpy(list->data + list->len / 2 * list->elem_size,
			   elem, list->elem_size);
	else
		list->destructor(elem);

	list->len++;
}

/********************************* TASKS *********************************/

array_t reverse(array_t list) {
	array_t reversed_list;
	reversed_list.data = malloc(list.len * list.elem_size);
	reversed_list.len = list.len;
	reversed_list.elem_size = list.elem_size;
	reversed_list.destructor = list.destructor;

	reduce(reverse_list, &reversed_list, list);
	reversed_list.len = list.len;

	return reversed_list;
}

array_t create_number_array(array_t integer_part, array_t fractional_part) {
	array_t number_list = map_multiple(create_number, sizeof(number_t),
									   number_distructor, 2, integer_part,
									   fractional_part);

	return number_list;
}

array_t get_passing_students_names(array_t list) {
	array_t passing_students = filter(check_grade, list);
	return map(get_name, sizeof(char *), NULL, passing_students);
}

array_t check_bigger_sum(array_t list_list, array_t int_list) {
	array_t sum_list = map_multiple(check_sum, sizeof(boolean), NULL, 2,
									list_list, int_list);

	return sum_list;
}

array_t get_even_indexed_strings(array_t list) {
	array_t even_indexed_strings;
	even_indexed_strings.data = malloc(list.len * list.elem_size);
	even_indexed_strings.len = 0;
	even_indexed_strings.elem_size = list.elem_size;
	even_indexed_strings.destructor = list.destructor;

	reduce(get_even_index, &even_indexed_strings, list);
	even_indexed_strings.len  = (even_indexed_strings.len + 1) / 2;

	free(list.data);

	return even_indexed_strings;
}

array_t generate_square_matrix(int n) {
	(void)n;
	return (array_t){0};
}
