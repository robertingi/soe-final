Part(b)

Sometimes commonly used functions get paired together without them being reliant upon each other because of how often they are called together, perhaps coincidentally. False positives then can occur when either of those paired functions are called without the other one in the pair being called, which the tool will then perceive as a bug.


bug: apr_hook_debug_show in ap_hook_default_port, pair: (apr_array_push, apr_hook_debug_show), support: 28, confidence: 87.50%
(4 lines)

This is a good example of a coincidental pairing as these functions do not share any similarities. They have most likely been paired together because they were called within the same scope often enough to warrant a bug, but as apr_array_push is a function that inserts a new element into an array and apr_hook_debug_show is a debugging tool, it's obvious to see that they are completely independant from eachother and therefore the pairing bug is a false positive.

bug: apr_thread_mutex_lock in apr_dbd_mutex_lock, pair: (apr_thread_mutex_lock, apr_thread_mutex_unlock), support: 43, confidence: 95.56%
(6 lines)

At first this looks like a bug, as every thread lock should be corresponded by a thread unlock, but as apr_dbd_mutex_lock is just a wrapper for apr_thread_mutex_lock, it would not be rational for there to be a  apr_thread_mutex_unlock call within the same wrapper, so this bug is an example of a false positive.

Part(c)

Our algorithm is as such: when pairs are being stored, if one of the functions is a scope, the function is expanded into the functions in it's scope, and the algorithm will then continue to pair the newly added functions. The new algorithm with inter-procedural analysis can be run by adding the -IPA flag to pipair, after the filename.
While there was not much of an improvement in performance, it seems to have found a lot more bugs (false positives or not, we're not sure)

