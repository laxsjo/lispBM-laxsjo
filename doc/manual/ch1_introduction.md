
# Chapter 1: Introduction to programming in LispBM 

LispBM (from now on called LBM) is a lisp dialect that was implemented
to be run on small resource constrained systems. The look-and-feel of
LispBM has been very much influenced by the series of videos based on
the SICP book (Structure and Interpretation of Computer Programs) by
Harold Abelson, Gerald Jay Sussman and Julie Sussman. The awesome
series of videos about lisp programming can be found
[here](https://www.youtube.com/playlist?list=PL8FE88AA54363BC46). Note
that LBM is not 100% compatible with all code you see in the video series 
but this is quite OK, there are many slightly different flavors of lisps. 

LBM itself implements the concurrency, communication and a basic set
of lisp functionality such as arithmetic. The idea with LBM is that it
should be embedded into some other embedded system, or other,
application and functionality specific to that application is exposed
to LBM via so-called extensions. As a result of that it is for example
not possible to print something from LBM. Printing is a simple example
of something that can be implemented in many different ways on many
different platforms (uart, bluetooth USB-CDC). It is up to the person
integrating LBM into a system to provide these interfaces.

That said, there is an example REPL (Read Evaluate Print Loop) that
can be run on X86 Linux and that is what will be used in this
introductory chapter to get started. 

## Building the example REPL

Clone the LispBM repository from [GitHub](https://github.com/svenssonjoel/lispBM).

```
git clone https://github.com/svenssonjoel/lispBM.git
``` 
Then go into the `lispBM` directory and the `repl` subdirectory.

``` 
cd lispBM
cd repl
``` 

Now you have the choice of compiling the REPL either a 32bit application 
or a 64bit one. To compile as a 32bit application just type `make`, this 
requires that you have 32bit libraries installed on you Linux system. The 
other alternative is to compile as 64bit using `make all64`. 

``` 
make 
``` 
or 

```
make all64
``` 

If all goes well there should now be an executable called `repl` in 
the current directory. To start the repl type:

```
./repl
``` 

which should present the following prompt: 


``` 
Array extensions loaded
String extensions loaded
Math extensions loaded
Extension added.
Extension added.
Lisp REPL started!
Type :quit to exit.
     :info for statistics.
     :load [filename] to load lisp source.
# 
```

If you try out the `:info` command you are presented with some 
information about the current state of the runtime system. 

```
# :info
--(LISP HEAP)-----------------------------------------------
Heap size: 16384 Bytes
Used cons cells: 0
Free cons cells: 2048
GC counter: 0
Recovered: 0
Recovered arrays: 0
Marked: 0
--(Symbol and Array memory)---------------------------------
Memory size: 2048 Words
Memory free: 1916 Words
Allocated arrays: 0
Symbol table size: 676 Bytes
```

There is a set of LBM functions mostly for basic operations on lists that 
the REPL will load dynamically when first used. 

```
# :prelude
``` 
The LBM repl will answer something like this: 

``` 
<< Context 138 finished with value foldl >>
stack max:  40
stack size: 256
stack sp:   0
``` 

If the `:info` command is run again we can see that the "prelude" library 
occupies some amount of heap memory. One of these functions is called `iota`
and it creates a list enumerating numbers from 0 up to the argument provided. 

To test if this dynamic loading of the library function works type `(iota 1024)`
and press enter. 

``` 
# (iota 1024)
loading: (iota 1024)
started ctx: 144
<< Context 144 finished with value (0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37
38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60
61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83
84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104
105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121
122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138
139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155
156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172
173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189
190 191 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206
207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223
224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240
241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 256 257
258 259 260 261 262 263 264 265 266 267 268 269 270 271 272 273 274
275 276 277 278 279 280 281 ... >>
stack max:  13
stack size: 256
stack sp:   0
```
If things seem to work so far, lets go on and play some with the repl. 

## Playing with the REPL 

The REPL allows you to enter expressions and have them evaluated. 
For example we can type `(+ 1 2)` at the prompt and get a response.

``` 
# (+ 1 2)
loading: (+ 1 2)
started ctx: 144
<< Context 144 finished with value 3 >>
stack max:  11
stack size: 256
stack sp:   0
```

The REPL informs us that it is loading `(+ 1 2)`, and that it started
a context with ID 144 to evaluate the expression in.  When then
context then finishes execution the REPL presents `<< Context 144
finished with value 3 >>` which means that the result of `(+ 1 2)` was
computed to be `3`. While computing this the evaluator used 11
elements of the stack and after finishing the stack pointer is back at
position 0.

The context that is being created and that evaluates the expression is 
related to the concurrent nature of LBM and will be explored in more 
detail later. 

The example REPL provides an extension for printing things, for example strings:

```
# (print "hello world!\n")
loading: (print "hello world!\n")
started ctx: 141
hello world!
<< Context 141 finished with value t >>
stack max:  11
stack size: 256
stack sp:   0
``` 

The program above implements "hello world" as you can see on line 3 of the 
output presented above. The print function baked into the REPL is capable 
of printing a lot of different LBM values, not just strings. 
for example: 

```
# (print 10)
loading: (print 10)
started ctx: 147
10<< Context 147 finished with value t >>
stack max:  11
stack size: 256
stack sp:   0
```

You can print many things at once by providing more arguments to print. 

```
# (print "hello world!\n")
loading: (print "hello world!\n")
started ctx: 141
hello world!
<< Context 141 finished with value t >>
stack max:  11
stack size: 256
stack sp:   0
```
We will see what else print can print as we progress through the manual. 

Now let's see if we can implement a small game right in the REPL! Inspired 
by the book "Land of Lisp" we can try a somewhat simplified version 
of the "guess-my-number" game that can be typed directly into the repl 
in small number of steps. 

The objective of the game is to have the computer guess what number you,
the user, is thinking about. 

First we specify in what range of numbers in which we are allowed to pick 
a random number. 

Enter the following into the REPL and hit enter: 
```
define small 1
``` 
and the REPL replies: 

``` 
# (define small 1)
loading: (define small 1)
started ctx: 162
<< Context 162 finished with value small >>
stack max:  11
stack size: 256
stack sp:   0
```
Our number must be larger than or equal to 1. 

```
define big 100
```
Our number must be smaller than or equal to 100.

`define` associates a variable with a value. Here we have defined `small` to 
be 1 and `big` to be 100. We can ask the REPL about these values now if we want
by typing `small` or `big` into the REPL and pressing enter. 


``` 
# small
loading: small
started ctx: 162
<< Context 162 finished with value 1 >>
stack max:  11
stack size: 256
stack sp:   0
```

To get a guess from the computer we call a function called
`guess-my-number` that is implemented as follows: 

```
(define guess-my-number (lambda () (/ (+ small big) 2)))
```
This time define is used to associate a name with a function. The 
function itself is created using `lambda`. This function takes no 
arguments, this is what the `()` means following `lambda`. After the 
empty list of arguments comes the body of function that computes (small + big) / 2. 
which means the computer will guess in the middle of the range. 

Now we can ask the computer to take a guess by typing
`(guess-my-number)` at the REPL prompt. The parenthesis around
`guess-my-number` means function application.

``` 
# (guess-my-number)
loading: (guess-my-number)
started ctx: 162
<< Context 162 finished with value 50 >>
stack max:  12
stack size: 256
stack sp:   0
``` 
Now, if the computer's guess is wrong we can help it by saying `(bigger)` or `(smaller)`. 
Let's implement these functions. 

```
(define smaller (lambda () (define big (- (guess-my-number) 1))))
```
The `smaller` function takes no arguments and works but setting the upper end 
of the range for the computer to guess in to the current guess - 1. The current 
guess is obtained by calling the `guess-my-number` function. `define` is used 
in the function body here to re-define the value of `big`.

The `bigger` function works similarly but moves the lower end of the range 
up to the current guess. 

```
(define bigger (lambda () (define small (+ (guess-my-number) 1))))
```

Now we have all the functions we need to play the game. 

Let's say that we think about the number 7 and ask the computer to guess: 

```
# (guess-my-number)
loading: (guess-my-number)
started ctx: 166
<< Context 166 finished with value 50 >>
stack max:  12
stack size: 256
stack sp:   0
``` 

50, thats to high, type `(smaller)` into the REPL and press enter. 
The we ask the computer to guess again 

```
# (guess-my-number)
loading: (guess-my-number)
started ctx: 166
<< Context 166 finished with value 25 >>
stack max:  12
stack size: 256
stack sp:   0
``` 
25, that is still too high, `(smaller)`, enter. 
Guess again computer!

``` 
# (guess-my-number)
loading: (guess-my-number)
started ctx: 166
<< Context 166 finished with value 12 >>
stack max:  12
stack size: 256
stack sp:   0
``` 

12, too high again, `(smaller)`, enter. 
Guess again!

```
# (guess-my-number)
loading: (guess-my-number)
started ctx: 166
<< Context 166 finished with value 6 >>
stack max:  12
stack size: 256
stack sp:   0
``` 

6, is too small! `(bigger)`, enter.
Guess please ;)

``` 
# (guess-my-number)
loading: (guess-my-number)
started ctx: 166
<< Context 166 finished with value 9 >>
stack max:  12
stack size: 256
stack sp:   0
``` 

No, no, not 9. `(smaller)`, enter.
`(guess-my-number)` 

```
# (guess-my-number)
loading: (guess-my-number)
started ctx: 166
<< Context 166 finished with value 7 >>
stack max:  12
stack size: 256
stack sp:   0
```
Yay! Go computer!


## LBM Syntax and Semantics

Languages in the Lisp family use the same data structure to represent
programs as they do to organize data. This data structure is the *list*. 
As a result the syntax relating to lists is very important get down early. 
Languages that use the same representation for data and code, are said to 
have a property called homoiconicity. The homoiconicity property is valuable 
in situations where you are doing "meta-programming", writing programs 
that result in programs, as for example when writing macros. In the very 
beginning it can, however, be a bit confusing! But hang in there and 
the benefits will become clear over time. 


### Lists 

Lists in LBM are enclosed in parentheses and can be arbitrarily nested. 
So `(1 2 3)` is a list and `(1 (2 3) 4)` is a list where the second element 
is again a list. Now, if we try to write `(1 2 3)` into the REPL and hit enter, 
the REPL wont be happy with us!

``` 
# (1 2 3)
loading: (1 2 3)
started ctx: 144
***	Error: eval_error

<< Context 144 finished with value eval_error >>
stack max:  11
stack size: 256
stack sp:   4
```

Typing `(1 2 3)` into the REPL resulted in an eval error. This is because 
the default way in which lists are understood by LBM is as code. And 
`(1 2 3)` is not a valid LBM program. 

When a list such as `(a b c)` (for any a,b,c) is entered into the REPL
the LBM evaluator will assume that this is an application of the function 
`a` to the arguments `b` and `c`. The list `(+ 1 2)` on the other hand 
is a valid program as the first element of the list is the addition function
and it can be applied to the arguments `1` and `2`. 

```
# (+ 1 2)
loading: (+ 1 2)
started ctx: 144
<< Context 144 finished with value 3 >>
stack max:  11
stack size: 256
stack sp:   0
```

So, if lists that we give to the evaluator are assumed to be code, how
do we create a list of data? We will see many answers to this
questions throughout this manual but to begin with we will use a
function application to create a list of data. LBM provides a function
called `list` that takes an arbitrary number of arguments and returns
a list containing those values. Now the list `(list 1 2 3)` is
something that makes sense to the LBM evaluator, because the first
element is the function `list`.

```
# (list 1 2 3)
loading: (list 1 2 3)
started ctx: 144
<< Context 144 finished with value (1 2 3) >>
stack max:  11
stack size: 256
stack sp:   0
```

In a function application the arguments are evaluated before the function 
at the head of the list is applied to them.

### Symbols

Symbols are very fundamental building blocks of LBM programs
and data. A symbol is made up from a number of characters and we have
seen some examples already, for example `list` or `guess-my-number`
from the dive-in-intro. Symbols are used to name data or functions 
but can also be used as values in themselves. For this introduction 
to symbols we will focus on their usefulness for giving names to things. 

To associate a value with a symbol, we use `define`. In this use-case 
of symbols, you can think of them as basically variables. To define a 
variable give, for example, the list `(define a 10)` to the REPL. 

```
# (define a 10)
loading: (define a 10)
started ctx: 144
<< Context 144 finished with value a >>
stack max:  11
stack size: 256
stack sp:   0
```

This sets up an association between the symbol `a` and the value 10. And 
now if we enter `a` into the REPL and press enter, the REPL will reply 
with `10`. 

```
# a
loading: a
started ctx: 148
<< Context 148 finished with value 10 >>
stack max:  11
stack size: 256
stack sp:   0
```

### An important concept with an unremarkable name: Quote 

The `quote` operation is very important as it will turn up a lot as 
we move ahead. Quote is written as `'` in code and it tells the evaluator 
to **not** evaluate the following expression. As we saw earlier, anything we 
enter into the REPL will be evaluated, and because of that we could not type 
in `(1 2 3)` without getting an error. Typing in `'(1 2 3)` is perfectly 
fine and results in the list `(1 2 3)`. 

```
# '(1 2 3)
loading: '(1 2 3)
started ctx: 148
<< Context 148 finished with value (1 2 3) >>
stack max:  11
stack size: 256
stack sp:   0
```

Any LBM expression can be quoted and essentially then viewed as a
datastructure rather than as code. For example you can quote any
symbol:

```
# `i-am-a-symbol
loading: `i-am-a-symbol
started ctx: 148
<< Context 148 finished with value i-am-a-symbol >>
stack max:  11
stack size: 256
stack sp:   0
```

Quoted symbols is very common to see when writing programs that use
symbols not as variables but as data in themself. A symbol can for
example be used as a tag or label. For example if you want to keep
track of your apples and bananas, you could create a datastructure
like this `(list (list 'apples 10) (list 'bananas 15))`.

```
# (list (list 'apples 10) (list 'bananas 15))
loading: (list (list 'apples 10) (list 'bananas 15))
started ctx: 151
<< Context 151 finished with value ((apples 10) (bananas 15)) >>
stack max:  14
stack size: 256
stack sp:   0
```

Another example is that you can quote an addition expression such as
`(+ 1 2)`:

```
# '(+ 1 2)
loading: '(+ 1 2)
started ctx: 151
<< Context 151 finished with value (+ 1 2) >>
stack max:  11
stack size: 256
stack sp:   0
```

the result of this quoted addition is a list with the elements `+`,`1`
and `2`. 

Quote is a bit interesting as it does not follow the pattern of a
function application, which is a list where the first element is
assumed to be function.  The quote mark, `'` is placed before the
thing it "operates" on and it is not not enclosed in parenthesis! Now,
this is just syntax and as the LBM code is read by the parser all
things of the form `'a` is expanded into `(quote a)` and `(quote a)`
fits the familiar pattern very well. Still, `quote` is not quite a
function and this is because when quoting something we want the thing
we quote to be returned to us unevaluated! In a function application
the arguments are evaluated.  So `quote` falls into a small set of
operations called **special forms** which are the topic of the next
subsection.

### Special forms

The special forms of LBM are a bit like the set of keywords you find
in many languages. The special forms are each identified via a symbol
and they are "applied" in a similar way to functions. That is, if you
want to apply a special form, that special form symbol will be first
in a list. The special part of a special form is that an application
of a special form does **not** need to follow the same behavior as a
function application (where all arguments are evaluated before the
application). Why this is needed will become clear as we go through 
whats special about each of the LBM special forms below. There aren't
many of them, only 12, so fortunately not a lot of "special" behavior 
to memorize. 

We have already seen the special forms `define` and `quote`

**DEFINE** 

`define` takes two arguments, a symbol and a value. `define` 
does not evaluate the first argument, because if it did that could 
potentially result in something that is not a symbol anymore. Also, 
if we imagine that define did evaluate the first argument, the result 
would be that it ends up being impossible to associate any value with 
any symbol! 

---

Imagine for a second that in the expression `(define a 10)`, the a
would be evaluated. Evaluating a symbol should result in a value if
there is a value associated with the symbol (symbols are also values,
so it could be another symbol) say the value is 5. Defining 5 to be 10 
doesn't make sense. The other option is that there is no value associated 
with the symbol `a`, in that case we get an error because `a` could not 
be evaluated.

---