# AML

* Author: Alexander Myasnikov
* mailto: myasnikov.alexander.s@gmail.com
* git: https://gitlab.com/amyasnikov/aml
* version: 0.5



### Сборка

```
cmake -S. -B./build && cmake --build ./build -j8
```



### Запуск тестов

```
./build/aml/aml_tests
```



### Запуск

Компиляция исходного кода в байт-код:

```
./build/aml/aml --cmd=compile --input=resources/sample.aml --output=resources/tmp.aml.binary
```

Исполнение байт-кода:

```
./build/aml/aml --cmd=exec --input=resources/tmp.aml.binary
```


### Цели:

* Описать необходимый минимальный набор инструкций, необходимый для трансляции высокоуровнего языка.



### Общие сведения:

* AML - стековый язык.
* Данные в code  выровнены по границе uint8_t.
* Данные в stack выровнены по uint64_t.



### Ограничения:

* Тип данных - только int64_t.
* Отсутствуют проверки типов на этапе компиляции.



### Пример кода:

Смотри sample.aml




### Стадия 1. Лексический анализатор

Для простоты разборы используется lisp синтаксис.

Исходный текст разбивается на следующие токены:

* whitespace
* lp
* rp
* key_arg
* key_block
* key_call
* key_defn
* key_defvar
* key_func
* key_if
* key_int
* key_syscall
* key_var
* integer
* ident



### Стадия 2.a. Синтаксический анализатор (LISP)

Из списка токенов строится дерево в соответствии с грамматикой:

```
lisp_tree: node
node: LP node* RP | key | integer | ident
```

Эта стадия добавлена для упрощения разбора



### Стадия 2.b. Синтаксический анализатор

Из lisp_tree строится stmt в соответствии с грамматикой:

```
stmt:    program
program: func+   expr
expr:    ARG     <digit>
expr:    BLOCK   expr+
expr:    CALL    expr    expr*
expr:    DEFVAR  <name>  expr
expr:    IF      expr    expr expr
expr:    INT     <digit>
expr:    SYSCALL expr+
expr:    VAR     <name>
func:    DEFN    expr    expr
```



### Стадия 3 Семантический анализатор

* Добавить проверку типов. // TODO



### Стадия 4 Генерация промежуточного кода

Дерево stmt переводится в ПОЛИЗ.

Инструкции ПОЛИЗ:
* arg
* call
* exit
* jmp
* pop_jif
* push8
* ret
* syscall

Правила перевода stmt в ПОЛИЗ:

```
(arg <digit>) ->
    arg

(block expr1 expr2 ... exprN) ->
    TODO

(call expr_name expr1 expr2 ... exprN) ->
    CODE(expr_name)
    CODE(exprN)
    ...
    CODE(expr2)
    CODE(expr1)
    push8 N
    call

(defvar <name>) ->
    TODO

(func <name>) ->
    push8 <offset>

(if expr_if expr_then expr_else) ->
    CODE(expr_if)
    pop_jif <M1>
    CODE(expr_then)
    jmp <M2>
M1: CODE(expr_else)
M2:

(int <digit>) ->
    push8 <digit>

(defn <name> expr_body) ->
    CODE(expr_body)
    ret

(syscall expr1 expr2 ... exprN) ->
    CODE(exprN)
    ...
    CODE(expr2)
    CODE(expr1)
    push8 N
    syscall

(var <name>) ->
    push8 <offset>
```



### Стадия 5 Оптимизация кода

// TODO



### Стадия 6 Генерация кода

Перевод ПОЛИЗ выражения в тетрады или триады. // TODO



### Выполнение кода

Для выполнения нужен созданный бинарный код, содержащий ПОЛИЗ команды и указатель на стартовое выражение.

Выполнение команд осуществляется на стеке в соответствии с правилами:

```
exit -> завершение выполнения

arg <offset>:
  stack: ... -> ... MEMORY[offset]

call:
  stack: ... <argN> ... <arg2> <arg1> <N> -> <argN> ... <arg2> <arg1> <N> <rpb> <rip>
  rip = argN
  rbp = stack.size

jmp <label>:
  rip = <label>

pop_jif <label>:
  stack: ... res -> ...
  rip = res ? <label> : rip

push8 <digit>:
  stack: ... -> ... <digit>

ret:
  stack: <argN> ... <arg2> <arg1> <N> <rbp_old> <rbp_old> <ret> -> ... <ret>
  rip = rip_old
  rbp = rbp_old

syscall:
  stack: <argN> ... <arg2> <arg1> <N> -> <ret>
```



### Пример генерации кода

```
Исходный код:

(deffunc sum
  (syscall
    (INT 1)
    (ARG 1)
    (ARG 2))

(deffunc main
  (call sum
    (call sum
      (INT 10)
      (INT 11)
      (INT 12))
    (call sum
      (INT 20)
      (INT 21))))

(call main)

ПОЛИЗ:

10:        <main> 0 CALL ret exit
20:<sum>:  $2 $1 1 3 syscall ret
30:<main>: <sum> <sum> 12 11 10 3 call <sum> 21 20 2 call 2 call ret
           30    31    32 33 34 35 36  37    38 39 40 41  42 43   44

Выполнение:

| rbp | stack                                                               | rip | code        |
|-----|---------------------------------------------------------------------|-----|-------------|
| 0   |                                                                     | 10  | PUSH <main> |
| 0   | <main>                                                              | 11  | PUSH 0      |
| 0   | <main> 0                                                            | 12  | CALL        | + rbp:rip
| 2   | <main> 0 rbp:0 rip:13                                               | 30  | PUSH <sum>  |
| 2   | <main> 0 rbp:0 rip:13 <sum>                                         | 31  | PUSH <sum>  |
| 2   | <main> 0 rbp:0 rip:13 <sum> <sum>                                   | 32  | PUSH 12     |
| 2   | <main> 0 rbp:0 rip:13 <sum> <sum> 12                                | 33  | PUSH 11     |
| 2   | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11                             | 34  | PUSH 10     |
| 2   | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10                          | 35  | PUSH 3      |
| 2   | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10 3                        | 36  | CALL        |
| 10  | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10 3 rbp:2 rip:37           | 20  | PUSH $2     | $N = rbp - 1 - N
| 10  | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10 3 rbp:2 rip:37 11        | 21  | PUSH $1     |
| 10  | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10 3 rbp:2 rip:37 11 10     | 22  | PUSH 1      |
| 10  | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10 3 rbp:2 rip:37 11 10 1   | 23  | PUSH 3      |
| 10  | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10 3 rbp:2 rip:37 11 10 1 3 | 24  | SYSCALL     |
| 2   | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10 3 rbp:2 rip:37 21        | 25  | RET         |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21                                      | 37  | PUSH <sum>  |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21 <sum>                                | 38  | PUSH 21     |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21                             | 39  | PUSH 20     |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20                          | 40  | PUSH 2      |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20 2                        | 41  | CALL        |
| 15  | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20 2 rbp:2 rip:42           | 20  | PUSH $2     |
| 15  | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20 2 rbp:2 rip:42 21        | 21  | PUSH $1     |
| 15  | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20 2 rbp:2 rip:42 21 20     | 22  | PUSH 1      |
| 15  | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20 2 rbp:2 rip:42 21 20 1   | 23  | PUSH 3      |
| 15  | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20 2 rbp:2 rip:42 21 20 1 3 | 24  | SYSCALL     |
| 15  | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20 2 rbp:2 rip:42 41        | 25  | RET         |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21 41                                   | 42  | PUSH 2      |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21 41 2                                 | 43  | CALL        |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44                    | 20  | PUSH $2     |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 21                 | 21  | PUSH $1     |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 21 41              | 22  | PUSH 1      |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 21 41 1            | 23  | PUSH 3      |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 21 41 1 3          | 24  | SYSCALL     |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 62                 | 25  | RET         |
| 2   | <main> 0 rbp:0 rip:13 62                                            | 13  | RET         |
| 0   | 62                                                                  | 14  | EXIT        |
```



