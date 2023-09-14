# armyknife-scheme

This is a tiny and incomplete scheme implementation for the purposes
of showing how a very readable tail recursive interpreter can be
created in C (as long as gcc or clang is used).

Essentially we focus on a small subset of the special forms (that are
interpreted instead of say macro expanded) and of course calls to
primitives or closures. Likewise the "reader" supports a massive
subset of the syntax that a real scheme should provide and even the
printer is pretty sad.

scheme is a garbage collected language but we don't implement one
yet. Certainly, the easiest option looks like using
Boehm–Demers–Weiser garbage collector.

## Reader Syntax

```
;;; line level comments

;;; a typical simple expression
(define identity-function (lambda (x) x))

;;; strings
"an ascii only string. no unescaping is done"

;;; long syntax for quote
(qoute mysymbol)

;;; short syntax for symbols
'mysymbol

;;; positive integers (we don't yet handle -)
12345
0x12345
0b1111

(exit)

```

## Supported Special Forms

* self evaluating data such as numbers and strings
* variable references
* set!
* define
* if
* quote
* lambda
* begin

## built-in primitives

* eval
* interaction-environment
* +, -, *, / (signed 64bit integers only)
* cons, car, cdr
* string-append
* exit

## Status

I've just begun to split out the scheme interpreter I was using to
drive a VM I was writing after realizing that I could just use "chibi
scheme" instead of building an incomplete and slow scheme. On the
otherhand polishing what I created so far may make this is fun piece
of code for others to read.

I may slowly add some of R7RS. I would love to be able to use an
existing scheme reader written in scheme and only use the weak reader
I wrote to bootstrap the real reader.

# Conclusion

Hopefully you will enjoy reading this code. Besides being woefully
incomplete, I'm pretty sure this scheme will be extremely slow (though
startup times should be lightning fast). I may slowly add parts of
R7RS. It would be 

