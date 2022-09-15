# Pretend_HideVirtualMemory
   - 利用物理内存映射，实现虚拟内存的伪隐藏

## 简单内容：
   - 使用这种方法首先进程必须有SeLockMemoryPrivilege权限。
   - 相关内容资料：[Address Windowing Extensions](https://docs.microsoft.com/en-us/windows/win32/memory/address-windowing-extensions "Address Windowing Extensions")
   - 首先使用AllocateUserPhysicalPages分配一块物理内存
   - 然后使用MapUserPhysicalPages将这块物理内存映射到虚拟内存。
   - 向这块虚拟内存中写入数据
   - 解除映射。
   - 然后搜索进程内存，会发现这块内存找不到了。

## 为什么是AWE
   - 其实文件映射可以实现类似效果，但是为什么用AWE相关函数呢？
   - 首先文件映射并不是完全没法外部读出，外部进程一样可以映射。
   - 但是MapUserPhysicalPages就不一样了，系统为了防止乱映射物理地址，内部有很多检测，这导致用户层下，MapUserPhysicalPages只能映射自己进程用AllocateUserPhysicalPages分配出的物理内存，用户层下外部基本没法实现直接读出这个物理内存中的数据。
