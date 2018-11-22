# SpectrePocKernelModule
A CTF styled Poc to extract a secretflag from the kernel space  using spectre v1


1. Install linuxheader
2. Make
3. sudo insmod spectreModule.ko
4. cat /proc/leakSecretByteAddress to make sure that address is being returned
5. Run precompile ./read to verify the secret byte is leaked from kernel space

Write your own code to leak the secret byte or the whole secret string with the help of skeletel.c