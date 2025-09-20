# sup-galilee-tp-cpp
TP de C++ pour Sup Galilée\
Some C++ coding exercise I had to do for school, and which I'm re-doing from scratch.
I've mostly used English in this repo, but you might
find some stuff in French too. Haven't really set a rule for myself here.

## Journal/misc
### 2025/09/11
Just realized the instructions indicated the use of C++17, but I'm using `std::format` in my code, which
is C++20. The instructions say that so that you have to use `unordered_map`'s `find()` method rather
than `contains()`. I've decided to follow the latter, but I'm maintaining the use of `std::format` nonetheless.

### 2025/09/17
Realized/remembered the instructions for this had some mistakes in, which confused me for a bit.\
\+ It feels like the whole thing is a little obfuscated on purpose, which makes sense for a teacher
wanting to evaluate their students' abilities, but also a little frustrating, especially in combination
with the mistakes that find their way into the given instructions every now and then.

### 2025/09/18
Finished exo 3, on to exo 4.

### 2025/09/19
Quick note : I allowed myself some changes to the instructions (some method names, but I also added some of my own),
as it felt more practical.\
Update : finished reading exo 6, and I don't feel like the algorithm implentation that's presented for the parse()
function is actually correct. Might make my own implementation.\
Update : in theory it should work with the provided examples though, or any exemple, as long as no state has two
distinct outgoing edges labeled with the same character (because otherwise, if you're looking for that character,
which corresponding outgoing edge do you actually pick ?). Might just start with that specific implementation, and
then add my own more complete implementation, if I feel like it.