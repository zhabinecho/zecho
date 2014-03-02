#ifndef TABLE_INCLUDE_H
#define TABLE_INCLUDE_H

struct table;

extern struct table *table_new(int hint,
	int cmp(const void *x, const void *y),
	unsigned hash(const void *key));
extern void table_free(struct table *tb);
extern int table_length(struct table *tb);
extern void table_put(struct table *tb, const void *key, void *value);
extern void table_get(struct table *tb, const void *key);
extern void table_remove(struct table *tb, const void *key);
extern void table_map(struct tabel *tb,
	void apply(const void *key, void **value, void *cl),
	void *cl);
extern void **table_toarray(struct table *tb, void *end);

#endif
