#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/pid.h>
#include <linux/sched/signal.h>
#include <linux/sched/mm.h>
#include <linux/mm.h>
#include <linux/pgtable.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/version.h>
#include <asm/pgtable.h>
#include <linux/huge_mm.h>

#ifndef PMD_SIZE
#define PMD_SIZE(1UL << PMD_SHIFT)
#endif
#ifndef PUD_SIZE
#define PUD_SIZE (1UL << PUD_SHIFT)
#endif



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("CSE330 Project 4 - memory_manager");

static int pid = -1;
module_param(pid, int, 0444);
MODULE_PARM_DESC(pid, "pid of target process");

static unsigned long long addr = 0;
module_param(addr, ullong, 0444);
MODULE_PARM_DESC(addr, "virtual address in target process");

static struct task_struct *hold_task_by_pid(int tpid) {
	struct task_struct *t = NULL;
	rcu_read_lock();
	t = pid_task(find_vpid(tpid), PIDTYPE_PID);
	if (t) {
		get_task_struct(t);
	}
	rcu_read_unlock();
	return t;
}

static void drop_task(struct task_struct *t) {
	if (t) {
		put_task_struct(t);
	}
}

static int translate_addr(struct mm_struct *mm, unsigned long long vaddr, phys_addr_t *paddr_out, unsigned long *swapid_out) {
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *ptep;
	pte_t pteval;
	int rc = 2;

	if (!mm) {
		return 2;

	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
	mmap_read_lock(mm);
#else
	down_read(&mm->mmap_sem);
#endif
	pgd = pgd_offset(mm, vaddr);
	if (!pgd || pgd_none(*pgd) || pgd_bad(*pgd)) {
		goto out;
	}

	p4d = p4d_offset(pgd, vaddr);
	if (!p4d || p4d_none(*p4d) || p4d_bad(*p4d)) {
		goto out;
	}

	pud = pud_offset(p4d, vaddr);
	if (!pud || pud_none(*pud) || pud_bad(*pud)) {
		goto out;
	}

#ifdef pud_trans_huge
	if (pud_prsent(*pud) && pud_trans_huge(*pud(( {
		phys_addr_t pfn = (phys_addr_t)pud_pfn(*pud);
		phys_addr_t off = (phys_addr_t)(vaddr & (PUD_SIZE - 1));
		*paddr_out = (pfn << PAGE_SHIFT) | off;
		rc = 0;
		goto out;
	}
#endif

	pmd = pmd_offset(pud, vaddr);
	if (!pmd || pmd_none(*pmd) || pmd_bad(*pmd)) {
		goto out;
	}

#ifdef pmd_trans_huge
	if (pmd_present(*pmd) && pmd_trans_huge(*pmd)) {
		phys_addr_t pfn = (phys_addr_t)pmd_pfn(*pmd);
		phys_addr_t off = (phys_addr_t)(vaddr & (PMD_SIZE - 1));
		*paddr_out = (pfn << PAGE_SHIFT) | off;
		rc = 0;
		goto out;
	}
#endif

	ptep = pte_offset_kernel(pmd, vaddr);
	if (!ptep) {
		goto out;
	}

	pteval = *ptep;

	if (pte_present(pteval)) {
		phys_addr_t pfn = (phys_addr_t) pte_pfn(pteval);
		phys_addr_t off = (phys_addr_t) (vaddr & (PAGE_SIZE - 1));
		*paddr_out = (pfn << PAGE_SHIFT) | off;
		rc = 0;
	}

	else if (!pte_none(pteval)) {
		swp_entry_t se = pte_to_swp_entry(pteval);
		*swapid_out = se.val;
		rc = 1;
	}

	else {
		rc = 2;
	}

out:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
	mmap_read_unlock(mm);
#else
	up_read(&mm->mmap_lock);
#endif
	return rc;
}

static void print_mem(int tpid, unsigned long long vaddr, phys_addr_t paddr) {
	pr_info("[CSE330-Memory-Manager] PID %d: virtual address 0x%llx physical address 0x%llx  swap identifier [NA]\n", tpid, vaddr, (unsigned long long) paddr);

}

static void print_swap(int tpid, unsigned long long vaddr, unsigned long swapid) {
	pr_info("[CSE330-Memory-Manager] PID %d: virtual address 0x%llx physical address [NA] swap identifier 0x%lx\n", tpid, vaddr, swapid);
}

static void print_invalid(int tpid, unsigned long long vaddr) {
	pr_info("[CSE330-Memory-Manager] PID %d: virtual address 0x%llx physical address [NA] swap identifier [NA]\n", tpid, vaddr);
}

static int __init memory_manager_init(void) {
	struct task_struct *task = NULL;
	struct mm_struct *mm = NULL;
	phys_addr_t paddr = 0;
	unsigned long swapid = 0;
	int kind;

	if (pid < 0) {
		print_invalid(pid, addr);
		return 0;
	}

	task = hold_task_by_pid(pid);
	if (!task) {
		print_invalid(pid, addr);
		return 0;
	}

	mm = get_task_mm(task);
	if (!mm) {
		print_invalid(pid, addr);
		drop_task(task);
		return 0;
	}

	kind = translate_addr(mm, addr, &paddr, &swapid);

	if (kind == 0) {
		print_mem(pid, addr, paddr);
	}

	else if (kind == 1) {
		print_swap(pid, addr, swapid);
	}

	else {
		print_invalid(pid, addr);
	}

	mmput(mm);
	drop_task(task);
	return 0;
}

static void __exit memory_manager_exit(void) {
	//nothing to clean
}

module_init(memory_manager_init);
module_exit(memory_manager_exit);
