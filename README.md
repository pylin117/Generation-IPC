# Generation-IPC
作業系統概論練習: 熟悉如何利用fork()系統呼叫產生新的process，以及process彼此之間如何使用POSIX shared memory互相傳遞資料。
 * [作業目的] 熟悉如何利用fork()系統呼叫產生新的process，以及process彼此之間如何使用POSIX shared memory互相傳遞資料。
 * [作業內容] 模擬船艦對戰的過程是由一個父行程產生一個子行程，然後雙方分別在自己一個4×4的地區中任意擺放一艘2×1的砲艇，輪流炸射對方。程式需要能從命令列讀入一個參數A1 用來表示基本功能 (A1=0)，或是進階功能(A1=1)。
