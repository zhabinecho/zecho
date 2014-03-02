#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include "table.h"

struct table{
	int size;
	int (*cmp)(const void *x, const void *y);
	unsigned (*hash)(const void *key);
	struct binding{
		struct binding *next;
		const void *key;
		void *value;
	}**buckets;
	int length;
	unsigned timestamp;
};

static int cmpatom(const void *x, const void *y)
{
	return x != y;
}

static unsigned hashatom(const void *key)
{
	return (unsigned long)key >> 2;
}

struct table *table_new(int hint,
	int cmp(const void *x, const void *y),
	unsigned hash(const void *key))
{
	struct table *tb;	
	int i;
	assert(hint >= 0);
	table = MALLOC(sizeof(*table) + hint * sizeof(table->buckets[0]));
	table->size = hint;
	table->cmp = cmp ? cmp : cmpatom;
	table->hash = hash ? hash : hashatom;
	table->buckets = (struct binding **)(table + 1);
	for(i = 0; i < table->size; i++)
	{
		table->buckets[i] = NULL;
	}
	table->length = 0;
	table->timestamp = 0;

	return table;
}

void *Table_get(struct table *tb, const void *key)
{
	int i;
	struct binding *p;
	i = (*tb->hash)(key) % tb->size;
	for(p = tb->buckets[i]; p; p = p->next)
	{
		if(tb->cmp(key, p->key) == 0)
			break;
	}

	return p ? p->value : NULL;
}

void *Table_put(struct table *tb, const void *key, void *value)
{
	int i;
	struct binding *p;
	void *prev;

	i = tb->hash(key) % tb->size;
	for(p = tb->buckets[i]; p; p = p->next)
	{
		if(tb->cmp(key, p->key) == 0)
			break;
	}
	if(p == NULL)
	{
		NEW(p);
		p->key = key;
		p->next = tb->buckets[i];
		tb->buckets[i] = p;
		tb->length++;
		prev = NULL;
	}
	else
	{
		prev = p->value;
	}
	p->value = value;

	return prev;
}

void *Table_remove(struct table *tb, const void *key)
{
	int i;
	struct binding **pp;
	i = tb->hash(key) % tb->size;
	for(pp = &tb->buckets[i]; *pp; pp = &(*pp)->next)
	{
		if(tb->cmp(key, (*pp)->key) == 0)
		{
			struct binding *p = *pp;
			void *value = p->value;
			*pp = p->next;
			FREE(p);
			tb->length --;
			return value;
		}
	}

	return NULL;
}

void Table_map(struct table *tb,
		void apply(const void *key, void **value, void *cl),
		void *cl)
{
	int i;
	struct binding *p;

	for(i = 0; i < tb->size; i++)
	{
		for(p = tb->buckets[i]; p; p = p->next)
		{
			apply(p->key, &p->value, cl);
		}
	}

	return;
}


