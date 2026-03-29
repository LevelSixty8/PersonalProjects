#include <linux/list.h>
#include <linux/slab.h>
#include "dm_cache.h"

void my_cache_hit(struct block_device *cache_blkdev, struct cacheblock *block, struct list_head *lru_head) {
	list_move_tail(&block->list, lru_head);
	do_read(cache_blkdev, block->cache_block_addr);
}

struct cacheblock* my_cache_miss(struct block_device *src_blkdev, struct block_device *cache_blkdev, sector_t src_blkaddr, struct list_head *lru_head) {
	struct cacheblock *victim;
	victim = list_first_entry(lru_head, struct cacheblock, list);

	list_move_tail(&victim->list, lru_head);

	do_read(src_blkdev, src_blkaddr);
	return victim;
}

void init_lru(struct list_head *lru_head, unsigned int num_blocks) {
	unsigned int i;
	INIT_LIST_HEAD(lru_head);

	for (i = 0; i < num_blocks; i++) {
		struct cacheblock *blk;
		blk = kvmalloc(sizeof(*blk), GFP_KERNEL);
		if (!blk) {
			break;
		}

		blk->src_block_addr = 0;
		blk->cache_block_addr = (sector_t)i << BLOCK_SHIFT;

		INIT_LIST_HEAD(&blk->list);
		list_add_tail(&blk->list, lru_head);
	}
}

void free_lru(struct list_head *lru_head) {
	while (!list_empty(lru_head)) {
		struct cacheblock *blk;

		blk = list_first_entry(lru_head, struct cacheblock, list);
		list_del(&blk->list);
		kvfree(blk);
	}
}
