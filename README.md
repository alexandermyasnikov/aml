
# AML

Author: Alexander Myasnikov

mailto:myasnikov.alexander.s@gmail.com

git:https://gitlab.com/amyasnikov/aml

Version: 0.1



### Цели:

* Описать необходимый минимальный набор инструкций, необходимый для трансляции высокоуровнего языка.



### Архитектура:

* Процедуры работают только с аргументами, локальными даннами и глобальными функциями.
* Данные в code  выровнены по u8.
* Данные в stack выровнены по u64.
* // Данные в regs  выровнены по u64.



### TODO:

* Добавить макросы как в assembler.



### Пример кода: // TODO

```
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

(call main) -> 0 <main> CALL

10:        <main> 0 CALL ret exit
20:<sum>:  $2 $1 1 3 syscall ret
30:<main>: <sum> <sum> 12 11 10 3 call <sum> 21 20 2 call 2 call ret
           30    31    32 33 34 35 36  37    38 39 40 41  42 43   44

| rbp | stack                                                               | rip | code        |
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
| 2   | <main> 0 rbp:0 rip:13 <sum> <sum> 12 11 10 3 rbp:2 rip:37 21        | 25  | ret         |
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
| 15  | <main> 0 rbp:0 rip:13 <sum> 21 <sum> 21 20 2 rbp:2 rip:42 41        | 25  | ret         |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21 41                                   | 42  | PUSH 2      |
| 2   | <main> 0 rbp:0 rip:13 <sum> 21 41 2                                 | 43  | CALL        |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44                    | 20  | PUSH $2     |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 21                 | 21  | PUSH $1     |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 21 41              | 22  | PUSH 1      |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 21 41 1            | 23  | PUSH 3      |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 21 41 1 3          | 24  | SYSCALL     |
| 8   | <main> 0 rbp:0 rip:13 <sum> 21 41 2 rbp:2 rip:44 62                 | 25  | ret         |
| 2   | <main> 0 rbp:0 rip:13 62                                            | 13  | ret         |
| 0   | 62                                                                  | 14  | exit        |
```


### GENERATION

```
REGS:
  rip
  rbp
  rsp

(call <name> <expr1> <expr2> ... <exprN>) ->
  PUSH <name>
  GEN expr1
  GEN expr2
  ...
  GEN exprN
  PUSH <N>
  CALL

(int <digit>) ->
  PUSH <digit>

(arg <digit>) ->
  XXX TODO

(BLOCK a b c) ->
  GEN a
  GEN b
  GEN c

(SET name value) ->
  LABEL name
  PUSH 0
  GEN value
  OP_SAVE

(FUNC name body) ->
  ;

(__add arg1 arg2) ->
  PUSH arg1
  OP_LOAD
  PUSH arg2
  OP_LOAD
  OP_ADD


{ <stack>           ; <code>         } -> { <stack>     }
{                   ; PUSH1 b1:opnd1 } -> { opnd1       }
{ b1:opnd1          ; POP1           } -> {             }
{ b8:opnd1          ; LOAD           } -> { M[opnd1]    }
{ b8:opnd1 b1:opnd2 ; SAVE           } -> {             }   M[opnd1] = opnd2
{ b1:opnd2 b1:opnd2 ; ADD1           } -> { opnd1+opnd2 }

```



