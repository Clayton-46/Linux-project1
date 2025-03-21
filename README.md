# 完整筆記

https://hackmd.io/@clayton46/ryXWqWrWJe/edit

# 建立System Code #

1.在linux-5.15.137裡面建立資料夾

>mkdir my_get_physical_addresses

![image](https://github.com/user-attachments/assets/db0b2065-caeb-4d9b-8a82-4e612fffb413)



2.進入所建的資料夾

> cd my_get_physical_addresses

![image](https://github.com/user-attachments/assets/47bcf608-91d1-4510-939b-07242250f112)

3.新增system call

> touch my_get_physical_addresses.c

![image](https://github.com/user-attachments/assets/3c73cdcb-2949-4033-8607-f7b1e6992a53)

4.編輯my_get_physical_addresses.c

>vim my_get_physical_addresses.c


# Linear address 轉成 Physical address 流程圖
![image](https://github.com/user-attachments/assets/55d5ceb4-fceb-4ac7-a66b-74fa29b5971f)

![image](https://github.com/user-attachments/assets/f886bcb8-7930-4710-8ca4-db6c1f7cfb3f)

# trace code

(1)

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/syscalls.h#L223

這個連結可以知道SYSCALL_DEFINEx

(2)

https://elixir.bootlin.com/linux/v5.15.137/source/arch/x86/include/asm/current.h

這個連結可以知道

#define current get_current()

將 current 定義為 get_current() 函數的結果，這樣在程式碼中可以簡單地通過 current 獲取當前的任務結構指標。

(3)

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/sched.h#L758

這個連結可以知道task_struct結構裡面有mm_struct

![image](https://github.com/user-attachments/assets/70984b54-941b-43db-a091-e31f58964dee)



進入mm找以下連結:

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/mm_types.h#L779

找到pgd_t *pgd;

![image](https://github.com/user-attachments/assets/7bcd7f34-3071-483d-9b07-6a0a38c011eb)


進入pgd 找到以下兩個連結:

![image](https://github.com/user-attachments/assets/87d93322-bb38-4550-8984-1bcf052497e5)

![image](https://github.com/user-attachments/assets/be4a7c0b-c9da-4ae3-bbcd-4264ad32d19a)

(4)

https://elixir.bootlin.com/linux/v5.15.137/source/arch/x86/include/asm/pgtable.h

https://elixir.bootlin.com/linux/v5.15.137/source/include/linux/pgtable.h

這兩個連結可以知道

提供頁表操作的輔助函數 (pgd_offset, p4d_offset, pud_offset,pmd_offset,pgd_none pgd_bad,p4d_none p4d_bad,pud_none pud_bad,pmd_none pmd_bad,pte_none,pte_offset_map)

(一). none

>pgd_none(*pgd), p4d_none(*p4d), 等等。

這些函數用於檢查特定頁表條目是否「不存在」。

當頁表條目無法映射到有效的物理頁時，內核會設置該條目為 none 狀態。

使用 none 檢查可以避免訪問無效的頁表條目。

(二). bad

>pgd_bad(*pgd), p4d_bad(*p4d), 等等。

這些函數用於檢查頁表條目是否「無效」。

無效的頁表條目可能是由於內核或硬件錯誤而損壞的。

通過 bad 檢查可以確保頁表條目是正常的，以防止訪問不正確的內存位置。

(5)

https://elixir.bootlin.com/linux/v5.15.137/source/tools/testing/scatterlist/linux/mm.h#L52

這個連結可以知道

#define PAGE_SIZE (4096)

#define PAGE_SHIFT (12)

#define PAGE_MASK (~(PAGE_SIZE-1))

#define page_to_pfn(page) ((unsigned long)(page) / PAGE_SIZE)

5.編輯完my_get_physical_addresses.c程式碼保存並退出
在my_get_physical_addresses的資料夾下建立Makefile
建立完並編輯，內容為:

![image](https://github.com/user-attachments/assets/85a6d1f6-6df9-41fe-b713-f4f8d558690b)



6.回到linux-5.15.137並編輯Makefile

![image](https://github.com/user-attachments/assets/0212a276-9885-4de3-8f6c-7bee436d6212)

找到core-y並在最後面新增my_get_physical_addresses

![image](https://github.com/user-attachments/assets/ff51424a-7fd7-44bb-84b4-4bd12b10a941)

7.在syscall_64.tbl中添加新的systemcall

為450 common my_get_physical_addresses sys_my_get_physical_addresses

>vim arch/x86/entry/syscalls/syscall_64.tbl

![image](https://github.com/user-attachments/assets/dac72f5e-b4b4-4ddc-876b-fa95324decac)



8.編輯linux-5.15.137/include/linux/syscalls.h

>vim include/linux/syscalls.h
    
>#在檔案最下方加入

>asmlinkage long sys_my_get_physical_addresses(void *);    //要求的檔案型別


9.編譯kernel

>make -j12

10.安裝kernel

>make modules_install -j12

>make install -j12


# Q1 

![image](https://github.com/user-attachments/assets/9f09a3f9-0fd7-4e87-9196-6831d7401ce2)

PID：pid=1295是Parent Process的pid，表示目前運行的 Parent Process。

PID：pid=1296是Child Process的pid。

Virtual Address：0x55f574189010 是global variable global_a 對應的Virtual Address，為Process內存中Logical Memory位置，由系統分配。

Physical Address：0x11300e010是global_a對應的 Physical Address，為Physical Memory的實際地址。

(每次執行時Physical Address與Virtual Address不固定，主要原因與系統的安全性機制及內存管理策略有關)

以上程式碼表現出copy-on-write的影響，從result可以看見在Before Fork、After Fork by parent、After by child中，global_a的logical address和physical address都相同，而最後copy on write in child的Physical Address 發生變化：當Child Process修改了 global_a 的值後，對應的 Physical Address 變成了0x115493010，表示觸發了Copy on Write，在 Child Process 中，當 global_a 被修改時，Kernel 會分配一個新的 Physical Address，這樣 Parent Process 和 Child Process 不再使用同一個 Physical Memory Page。因此 Child Process 的 Physical Address 變成0x115493010。

# Q1_Bonus

![image](https://github.com/user-attachments/assets/c1c0c714-621d-4c92-b393-854e6996a18c)

execlp會取代目前執行的代碼，執行新的程式碼，而execlp 是在當前進度上執行另一個程式，並不會創建新的ID，所以pid保持不變。

但執行execlp時系統會重建目前的記憶體空間以載入新程式的檔案，所以logical address會有變化。

# Q2 

![image](https://github.com/user-attachments/assets/e633267b-dc11-4fc1-809e-0cbc11a11a12)

從結果可以看出只有a[0]有Physical address

a[1999999]並沒有Physical address

便可得知loader並沒有在一開始就將Physical address分配給陣列中的所有元素

# Q2_Bonus

![image](https://github.com/user-attachments/assets/a09c9967-f11f-48f1-bbff-90753e838e45)

由結果可得知因為Lazy allocation的原因，所以系統只分配實體記憶體空間到a[1007]，從a[1008]開始就沒有被分配到實體記憶體空間

Lazy allocation:當程式宣告變數或請求記憶體空間時，系統並不會立即分配實體記憶體空間，而是等到真正使用該記憶體時才進行分配。

主要目的是提高效能和節省記憶體資源，當程式可能不會使用到所有宣告的記憶體時。透過Lazy allocation，系統可以避免不必要的記憶體佔用，並減少啟動時間或初始化時間





