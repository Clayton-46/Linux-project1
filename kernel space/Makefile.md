下載 Kernel Source

到 Kernel Archive 下載 kernel source，選擇 5.15.137 kernel。

# 進入 root 模式

> sudo su

# 也可以用這個指令來下載kernel

wget -P ~/ https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.137.tar.xz
雖然可以下載最新版本的kernel是6.5.9,但考慮到最新版本在compile kernel時可能會出現一些error是stack overflow查不到的新型error所以不推薦,所以這邊還是用5.15.137做展示

解壓縮

# 把檔案解壓縮到 /usr/src 目錄底下

tar -xvf linux-5.15.137.tar.xz -C /usr/src
更新apt

sudo apt update && sudo apt upgrade -y
先把會用到的套件安裝好

# 這個套件可以幫我們 build 出 kernel-pakage

sudo apt install build-essential libncurses-dev libssl-dev libelf-dev bison flex -y
安裝vim

sudo apt install vim -y
清除安裝的package

sudo apt clean && sudo apt autoremove -y
新增 syscall
建立資料夾

# 把目錄轉到剛剛解壓縮完的 kernel 檔案夾

cd /usr/src/linux-5.15.137

# 在裡面創建一個名叫 hello 的資料夾

mkdir hello

# 把目錄轉到 hello 資料夾

cd hello
建立 hello.c

vim hello.c
hello.c 的內容







#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(hello){
    printk("Hello world!\n");
    return 0;
}
於同一個目錄下再建立一個 Makefile

vim Makefile
Makefile 的內容

obj-y := hello.o
接著回上個目錄改 Makefile

# 回上個目錄

cd ..

# 打開此目錄下的 Makefile

vim Makefile
可以找到 core-y 如下圖

註: vim編輯檔案的時候,可以用輸入 ?core-y 來找到core-y這個關鍵字



在最後面補上 hello 這樣 kernel 在編譯時才會到 hello 目錄

core-y += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ hello/
接下來要去修改 syscall_64.tbl 檔

vim arch/x86/entry/syscalls/syscall_64.tbl
在最後一行添加上 system call，然後請把編號記住, 等一下會用



接著編輯 syscalls.h 檔，將 syscall 的原型添加進檔案 (#endif之前)

# 把目錄轉回去
cd /usr/src/linux-5.15.137

vim include/linux/syscalls.h

#加入一行(這行直接加在檔案的最下面就好)

註: 在vim編輯檔案的時候 可以按shift+G就會跳到最後一行

asmlinkage long sys_hello(void);


編譯 kernel
可以用 make localmodconfig 來設定組態

make localmodconfig

#這邊會跳出一大堆問你要不要裝的套件,全部按enter跳過就好
ps:很多教學都會教你make menuconfig來設定.config檔,但那個.config檔太大,會導致kernel編譯要很久,所以建議用make localmodconfig只抓kernel需要的config,這樣可以大幅降低kernel編譯時間

以i7-10700這顆cpu來說,用make menuconfig要30幾分鐘,但make localmodconfig這樣只要7分鐘,跟鬼一樣= =(白眼之間要空格)

查看這台虛擬機是幾核心

nproc


然後直接make

# 這個代表用12核心去編譯kernel
make -j12
要記得如果你的核心有16核的話,不要用16核去編譯,盡量抓1/2~3/4

ex:16核就用8核~12核去編,比較不會當機

把kernel安裝上去

sudo make modules_install -j12

再打

sudo make install -j12
更新作業系統的bootloader成新的kernel

sudo update-grub
虛擬機要重新開機

reboot
重開機後再看看 kernel 版本有沒有變新


uname -r