#include <linux/kernel.h>//用來printk
#include <linux/syscalls.h>
#include <linux/mm.h>       // Data structure and functions related to memory management
#include <asm/pgtable.h>
#include <linux/pid.h>      // To obtain process info
#include <linux/sched.h>    // Definition of process control area
#include <linux/highmem.h>  // Functions related to page table operations
#include <asm/page.h>       // Functions related to page table operations

SYSCALL_DEFINE1(my_get_physical_addresses, void *, vaddr) {
    struct task_struct *task = current; // 獲取當前進程
    struct mm_struct *mm = task->mm;     // 獲取當前進程的內存管理結構
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    struct page *page;
    unsigned long physical_address;
    //none 和 bad 是內核頁表操作中的兩個重要概念，用於檢查頁表條目的狀態，分別代表頁表條目的「不存在」和「無效」。
    
    
    //查詢pgd
    pgd = pgd_offset(mm, (unsigned long)vaddr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
	    printk("pgd not found!!!\n");
        return 0;
    }
   
    //查詢p4d
    p4d = p4d_offset(pgd, (unsigned long)vaddr);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
	    printk("p4d not found!!!\n");
        return 0;
    }
 
    //查詢pud
    pud = pud_offset(p4d, (unsigned long)vaddr);
    if (pud_none(*pud) || pud_bad(*pud)) {
	    printk("pud not found!!!\n");
        return 0;
    }
    
    //查詢pmd
    pmd = pmd_offset(pud, (unsigned long)vaddr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
	    printk("pmd not found!!!\n");
        return 0;
    }
   

   //查詢pte
    pte = pte_offset_map(pmd, (unsigned long)vaddr);
    if (pte_none(*pte)) {
	    printk("pte not found!!!\n");
        return 0;
    }
   

     //取得當前process對應的page
    page = pte_page(*pte);
    if (!page) {
	    printk("page not found!!!\n");
        return 0;
    }
   

    //page丟進page_to_pfn()得到頁框索引，然後向左移12bits
    //~PAGE_MASK：將 PAGE_MASK 取反。page大小是 4KB，PAGE_MASK 就是 0xFFFFF000，
    //而 ~PAGE_MASK 就會是 0x00000FFF。這樣一來，vaddr & ~PAGE_MASK 只保留了virtual memory 的 Offset。
    //index跟virtual memory的offset做OR 相當於合併得到實體記憶體位置
    physical_address = (page_to_pfn(page) << PAGE_SHIFT) | ((unsigned long)vaddr & ~PAGE_MASK);
    
    return (void *)physical_address;

}

