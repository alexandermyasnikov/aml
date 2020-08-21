
# AML

Author: Alexander Myasnikov

mailto:myasnikov.alexander.s@gmail.com

git:https://gitlab.com/amyasnikov/aml

Version: 0.1



### Цели:

* Описать необходимый минимальный набор инструкций, необходимый для трансляции высокоуровнего языка.



### Особенности:

* Процедуры работают только с аргументами, локальными даннами и глобальными функциями.
* Все данные в code  выровнены по u8.
* Все данные в data  выровнены по u8.
* Все данные в stack выровнены по u8.
* Все данные в regs  выровнены по u64.



### TODO:

* Добавить макросы как в assembler.



### Пример кода: // TODO

```
(FUNC sum
  (BLOCK
    (SET ret (INT 1))
    (SET ret (CALL __add (ARG 1) (ARG 2)))
    (RETURN ret)))

(FUNC main
  (BLOCK
    (SET tmp
      (CALL sum
        (CALL sum 1 2)
        (INT 10)))
    (CALL print tmp)))
```



### GENERATION

```
CODE:

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



