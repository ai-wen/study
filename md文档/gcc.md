# 由 PTHREAD_MUTEX_RECURSIVE 引出的...

pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
error : ‘PTHREAD_MUTEX_RECURSIVE’ undeclared (first use in this function)

## std=c89 std=c99 std=c11 使用这些gcc的编译选项的时候会报错
PTHREAD_MUTEX_RECURSIVE 变量只是 POSIX 说明应该支持的变量，但是很多版本的 linux 并没有定义此变量，它们只有 PTHREAD_MUTEX_RECURSIVE_NP (the np stands for non-portable)，但是两者意义是相同的。
PTHREAD_MUTEX_RECURSIVE is what posix says should be supported, but some versions of glibc have only PTHREAD_MUTEX_RECURSIVE_NP (the np stands for non-portable), when they have the same meaning.

## 在linux平台上gcc编译器一般默认使用 gun 版本
C99 (GNU Dialect) (-std=gnu99)
C11 (GNU Dialect) (-std=gnu11)

在 工程属性中 C/C++ => Language => C99
在 工程属性中 C/C++ => Language => Command Line =>Additional Options 下添加头文件路径
-I ~/projects/tsrs10/include  -I ~/projects/tsrs10/libtsrs10/apilib/openssl/include -I ~/projects/tsrs10/libtsrs10/apilib/openssl/crypto/include 

在 工程属性中 Linker => Input => Library Dependencies 下添加 依赖库， libsm.so => sm   libpthread.so=>pthread  不用添加 -l前缀

应用后 可以在 Command Line 下查看Linker下所有的配置


## 宏 GNU_SOURCE 在vs编译器中暂时未成功
means the compiler will use the GNU standard of compilation, the superset of all other standards under GNU C libraries.

```cpp
gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/5/lto-wrapper
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 5.4.0-6ubuntu1~16.04.12' --with-bugurl=file:///usr/share/doc/gcc-5/README.Bugs --enable-languages=c,ada,c++,java,go,d,fortran,objc,obj-c++ --prefix=/usr --program-suffix=-5 --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-libmpx --enable-plugin --with-system-zlib --disable-browser-plugin --enable-java-awt=gtk --enable-gtk-cairo --with-java-home=/usr/lib/jvm/java-1.5.0-gcj-5-amd64/jre --enable-java-home --with-jvm-root-dir=/usr/lib/jvm/java-1.5.0-gcj-5-amd64 --with-jvm-jar-dir=/usr/lib/jvm-exports/java-1.5.0-gcj-5-amd64 --with-arch-directory=amd64 --with-ecj-jar=/usr/share/java/eclipse-ecj.jar --enable-objc-gc --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.12)

```