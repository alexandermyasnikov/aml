
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
(func product
  (if
    (call greater
      (arg 1)
      (int 1))
    (call mult
      (arg 1)
      (call product
        (call sub
          (arg 1)
          (int 1))))
    (int 1)))

(func main
  (call product
    (call add (int 2) (int 5))))
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



