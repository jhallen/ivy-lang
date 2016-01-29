/* Hash table

   Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

IVY is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  


IVY; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdlib.h>
#include <string.h>
#include "hash.h"

Hash_table *htmk(int starting_size)
{
	Hash_table *ht = malloc(sizeof(Hash_table));
	ht->size = starting_size - 1;
	ht->nentries = 0;
	ht->table = calloc(starting_size, sizeof(Hash_entry *));
	mk_allocator(ht->free_entries, sizeof(Hash_entry));
	return ht;
}

void htrm(Hash_table *h)
{
	rm_allocator(h->free_entries);
	free(h->table);
	free(h);
}

/* Find an entry */

void *htfindhval(Hash_table *h, char *name, unsigned hval)
{
	Hash_entry *e;
	for (e = h->table[hval & h->size]; e; e=e->next)
		if (!strcmp(e->name, name))
			return e->val;
	return NULL;
}

void *htfind(Hash_table *h, char *name)
{
	return htfindhval(h, name, hash(name));
}

/* Expand hash table */

void htexpand(Hash_table *h)
{
	/* Allocate new table */
	unsigned new_size = (h->size + 1) * 2 - 1;
	Hash_entry **new_table = calloc(h->size + 1, sizeof(Hash_entry *));
	/* Copy entries from old table to new */
	int x;
	for (x = 0; x != h->size + 1; ++x) {
		Hash_entry *e;
		while ((e = h->table[x])) {
			h->table[x] = e->next;
			e->next = new_table[e->hash_val & new_size];
			new_table[e->hash_val & new_size] = e;
		}
	}
	/* Replace old table with new */
	free(h->table);
	h->table = new_table;
	h->size = new_size;
}

/* Add an entry */

void htaddhval(Hash_table *h, char *name, unsigned hval, void *val)
{
	Hash_entry *e;
	e = al_item(h->free_entries);
	e->name = name;
	e->val = val;
	e->hash_val = hval;
	e->next = h->table[hval & h->size];
	h->table[hval & h->size] = e;
	if (++h->nentries == h->size/2 + h->size/4)
		htexpand(h);
}

void htadd(Hash_table *h, char *name, void *val)
{
	htaddhval(h, name, hash(name), val);
}

/* Compute hash value */

#define hnext(accu, c) (((accu) << 4) + ((accu) >> 28) + (c))

unsigned hash(char *s)
{
	unsigned accu = 0;
	while (*s)
		accu = hnext(accu, *(unsigned char *)s++);
	return accu;
}
