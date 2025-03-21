#include <stdio.h>
#include <unistd.h>
#define MY_SYSCALL_NUM 450

void *my_get_physical_addresses(void *addr){

    void *paddr;

    paddr = (void *)syscall(MY_SYSCALL_NUM, addr);

    if (paddr == 0) {
        printf("failure: cannot get physical address\n");
        return NULL;
    } else {
        printf("The virtual address %p ---> The physical address: %p \n", addr, paddr);
    }

    return paddr;
}

int a[2000000];

int main() {
    int loc_a;
    void *phy_add;

    for (int i = 0; i < 2000000; i++) {
        phy_add = my_get_physical_addresses(&a[i]);
        if (phy_add == NULL) {
            printf("\nLazy allocation end at ");
            printf("element: a[%d]\n\n", i);
            break;
        }
    }

    phy_add = my_get_physical_addresses(&a[0]);
    printf("global element a[0]:\n");
    printf("Offset of logical address:[%p]   Physical address:[%p]\n", &a[0], phy_add);
    printf("========================================================================\n");

    phy_add = my_get_physical_addresses(&a[1007]);
    printf("global element a[1007]:\n");
    printf("Offset of logical address:[%p]   Physical address:[%p]\n", &a[1007], phy_add);
    printf("========================================================================\n");

    phy_add = my_get_physical_addresses(&a[1008]);
    printf("global element a[1008]:\n");
    printf("Offset of logical address:[%p]   Physical address:[%p]\n", &a[1008], phy_add);
    printf("========================================================================\n");

    phy_add = my_get_physical_addresses(&a[1999999]);
    printf("global element a[1999999]:\n");
    printf("Offset of logical address:[%p]   Physical address:[%p]\n", &a[1999999], phy_add);
    printf("========================================================================\n");
}

