# underhanded-c-2015
My entry for the 2015 Underhanded C Contest

More info about the Underhanded C Contest:

http://www.underhanded-c.org
http://www.underhanded-c.org/#runners

The code was compiled using Clang under OS X 10.11.1, but should probably compile just fine under GCC or any ANSI C compatible compiler.

Spoiler for my submission:

This program is underhanded in the following ways:

(1) The program fails to release all “sample” structures back to the pool at the end of the match() function, but instead only frees the most recently used “sample” (see line 187)

(2) The pool_acquire() function will automatically ask for memory if it is exhausts it’s memory, but due to a bug, it goes to the error label instead of jumping to the done label.  This causes the caller of sample_create() to not initialize the “sample” struct.    So, when sample_match() is called and performs the actual material comparison, both the test and reference values are 0 from the memset() that occurs when a “sample” struct is allocated.

(3) Finally, because the memory “leak” is gradual, the first couple of tests perform as expected.  Even once the memory starts to become exhausted, there is always one item left in the pool.  This has the effect of always having 1 item successfully compared.    This means that in order for a comparison to be a false positive, the first item in the test needs to match the reference value.
